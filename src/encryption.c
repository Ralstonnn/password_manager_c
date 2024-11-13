#include "common.h"
#include "paths.h"
#include "terminal.h"
#include "validations.h"
#include <openssl/evp.h>
#include <stdio.h>

#define BASE64_ENCODING_TABLE                                                  \
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
#define CIPHER_ALGORITHM "AES-128-CBC"
#define KEY_LEN 16
#define IV_LEN 16
#define BASE64_DECODING_TABLE_LENGTH 80
#define BASE64_ENCODING_TABLE_LENGTH 64
#define BASE64_ENCODING_TABLE_LOWEST_CHAR 43

static unsigned char KEY[KEY_LEN + 1], IV[IV_LEN + 1];
static int BASE64_DECODING_TABLE[BASE64_DECODING_TABLE_LENGTH] = {
    62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1,
    -1, -1, -1, -1, -1, -1, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    -1, -1, -1, -1, -1, -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
    36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};

int generate_key_and_iv();
int validate_key_iv();

int encryption_init() {
    FILE *file_key = fopen(KEY_PATH, "r");
    FILE *file_iv = fopen(IV_PATH, "r");

    if (file_key == NULL || file_iv == NULL) {
        if (file_key != NULL)
            fclose(file_key);
        if (file_iv != NULL)
            fclose(file_iv);

        return generate_key_and_iv();
    }

    fgets((char *)KEY, KEY_LEN + 1, file_key);
    fgets((char *)IV, IV_LEN + 1, file_iv);
    fclose(file_key);
    fclose(file_iv);

    if (!validate_key_iv()) {
        clear_screen();
        enable_raw_mode();

        printf("Something is wrong with encryption!\nPress (r) to enter the "
               "password again or any other key to exit\n");
        char ch = getchar();

        disable_raw_mode();

        if (ch == 'r') {
            return generate_key_and_iv();
        } else {
            return 0;
        }
    }

    return 1;
}

int validate_key_iv() {
    return get_str_length((char *)KEY) == KEY_LEN &&
           get_str_length((char *)IV) == IV_LEN;
}

int save_key_iv_to_file() {
    FILE *file_key = fopen(KEY_PATH, "w");
    FILE *file_iv = fopen(IV_PATH, "w");

    if (file_key == NULL || file_iv == NULL) {
        if (file_key != NULL)
            fclose(file_key);
        if (file_iv != NULL)
            fclose(file_iv);

        return 0;
    }

    fprintf(file_key, "%s", (char *)KEY);
    fclose(file_key);
    fprintf(file_iv, "%s", (char *)IV);
    fclose(file_iv);

    return 1;
}

int get_password_encryption(char *out, int len) {
    int pass_len;
    int is_retry = 0;
    int is_password_hidden = 1;
    int is_valid = 1;

    do {
        clear_screen();

        if (!is_valid) {
            printf("The password is not valid!\n");
            printf("Password should not be empty\n");
            printf("Password can use letters, numbers and characters: "
                   "!@#$%%&*^\n\n");
        }

        printf("Enter a password to use for encryption(%i): ",
               PASSWORD_BUFFER_SIZE - 1);
        pass_len = get_password_from_stdin(out, len);

        if (!(is_valid = !is_string_empty(out) && validate_password(out)))
            continue;

        while (1) {
            clear_screen();
            printf("Enter a password to use for encryption(%i): ",
                   PASSWORD_BUFFER_SIZE - 1);

            if (is_password_hidden) {
                for (int i = 0; i < pass_len; i++)
                    printf("*");
            } else {
                printf("%s", out);
            }

            printf(
                "\nPress (h) to show password, (r) to type the password again "
                "and (enter) to continue\n");

            enable_raw_mode();
            char ch = getchar();

            if (ch == 'h') {
                printf("\nEntered password: %s", out);
                is_password_hidden = !is_password_hidden;
                is_retry = 0;
            } else if (ch == 'r') {
                is_retry = 1;
                disable_raw_mode();
                break;
            } else if (ch == '\n') {
                is_retry = 0;
                disable_raw_mode();
                break;
            }
            disable_raw_mode();
        }
    } while (is_retry || !is_valid);

    return pass_len;
}

int generate_key_and_iv() {
    char password[PASSWORD_BUFFER_SIZE];
    int pass_len = get_password_encryption(password, PASSWORD_BUFFER_SIZE);

    const EVP_MD *digest = EVP_sha256();
    int keyiv_len = KEY_LEN + IV_LEN;
    unsigned char keyiv[keyiv_len];

    if (!PKCS5_PBKDF2_HMAC(password, pass_len, NULL, 0, 1000, digest, keyiv_len,
                           keyiv)) {
        return 0;
    }

    unsigned int i = 0;
    for (i = 0; i < KEY_LEN; i++) {
        KEY[i] = keyiv[i];
    }
    for (i = KEY_LEN; i < KEY_LEN + IV_LEN; i++) {
        IV[i - KEY_LEN] = keyiv[i];
    }

    if (!validate_key_iv()) {
        return 0;
    }

    return save_key_iv_to_file();
}

void base64_generate_decoding_table() {
    int i;

    for (i = 0; i < BASE64_DECODING_TABLE_LENGTH; i++) {
        BASE64_DECODING_TABLE[i] = -1;
    }
    for (i = 0; i < BASE64_ENCODING_TABLE_LENGTH; i++) {
        // 43 is the lowest char in encoding table (+) and should be at index 0
        BASE64_DECODING_TABLE[BASE64_ENCODING_TABLE[i] - BASE64_ENCODING_TABLE_LOWEST_CHAR] = i;
    }
}

int base64_encoded_size(int inlen) {
    int ret;

    ret = inlen;
    if (inlen % 3 != 0)
        ret += 3 - (inlen % 3);
    ret /= 3;
    ret *= 4;

    return ret;
}

int base64_decoded_size(const char *encoded) {
    int len, ret, i;

    if (encoded == NULL)
        return 0;

    len = get_str_length((char *)encoded);
    ret = len / 4 * 3;

    for (i = len; i-- > 0;) {
        if (encoded[i] == '=') {
            ret--;
        } else {
            break;
        }
    }

    return ret;
}

int base64_encode(unsigned char *str, int str_len, char *out) {
    int elen, i, j, v;

    if (str == NULL || str_len == 0)
        return 0;

    elen = base64_encoded_size(str_len);
    out[elen] = '\0';

    for (i = 0, j = 0; i < str_len; i += 3, j += 4) {
        v = str[i];
        v = i + 1 < str_len ? v << 8 | str[i + 1] : v << 8;
        v = i + 2 < str_len ? v << 8 | str[i + 2] : v << 8;

        out[j] = BASE64_ENCODING_TABLE[(v >> 18) & 0x3F];
        out[j + 1] = BASE64_ENCODING_TABLE[(v >> 12) & 0x3F];
        if (i + 1 < str_len) {
            out[j + 2] = BASE64_ENCODING_TABLE[(v >> 6) & 0x3F];
        } else {
            out[j + 2] = '=';
        }
        if (i + 2 < str_len) {
            out[j + 3] = BASE64_ENCODING_TABLE[v & 0x3F];
        } else {
            out[j + 3] = '=';
        }
    }

    return 1;
}

int base64_is_valid_char(char c) {
    if (c >= '0' && c <= '9')
        return 1;
    if (c >= 'A' && c <= 'Z')
        return 1;
    if (c >= 'a' && c <= 'z')
        return 1;
    if (c == '+' || c == '/' || c == '=')
        return 1;
    return 0;
}

int base64_decode(const char *encoded, unsigned char *out, int outlen) {
    int len, i, j, v;

    if (encoded == NULL || out == NULL)
        return 0;

    len = get_str_length((char *)encoded);
    if (outlen < base64_decoded_size(encoded) || len % 4 != 0)
        return 0;

    for (i = 0; i < len; i++) {
        if (!base64_is_valid_char(encoded[i])) {
            return 0;
        }
    }

    for (i = 0, j = 0; i < len; i += 4, j += 3) {
        v = BASE64_DECODING_TABLE[encoded[i] - 43];
        v = (v << 6) | BASE64_DECODING_TABLE[encoded[i + 1] - 43];
        v = encoded[i + 2] == '='
                ? v << 6
                : (v << 6) | BASE64_DECODING_TABLE[encoded[i + 2] - 43];
        v = encoded[i + 3] == '='
                ? v << 6
                : (v << 6) | BASE64_DECODING_TABLE[encoded[i + 3] - 43];

        out[j] = (v >> 16) & 0xFF;
        if (encoded[i + 2] != '=')
            out[j + 1] = (v >> 8) & 0xFF;
        if (encoded[i + 3] != '=')
            out[j + 2] = v & 0xFF;
    }

    return 1;
}

void cleanUp(EVP_CIPHER_CTX *ctx, EVP_CIPHER *cipher) {
    EVP_CIPHER_CTX_free(ctx);
    EVP_CIPHER_free(cipher);
}

int encrypt_str(unsigned char *message, unsigned char *outbuf) {
    EVP_CIPHER_CTX *ctx = NULL;
    EVP_CIPHER *cipher = NULL;

    ctx = EVP_CIPHER_CTX_new();
    cipher = EVP_CIPHER_fetch(NULL, CIPHER_ALGORITHM, NULL);

    if (ctx == NULL || cipher == NULL) {
        cleanUp(ctx, cipher);
        return 0;
    }

    int encoded_len;
    unsigned char encoded[1024];
    int len, final_len;

    if (!EVP_EncryptInit_ex2(ctx, cipher, KEY, IV, NULL)) {
        cleanUp(ctx, cipher);
        return 0;
    }

    if (!EVP_EncryptUpdate(ctx, encoded, &len, message,
                           get_str_length((char *)message))) {
        cleanUp(ctx, cipher);
        return 0;
    }

    if (!EVP_EncryptFinal(ctx, encoded + len, &final_len)) {
        cleanUp(ctx, cipher);
        return 0;
    }

    encoded_len = len + final_len;
    cleanUp(ctx, cipher);

    if (!base64_encode(encoded, encoded_len, (char *)outbuf)) {
        return 0;
    }

    return base64_encoded_size(encoded_len);
}

int decrypt_str(const char *encoded_msg, unsigned char *outbuf) {
    EVP_CIPHER_CTX *ctx = NULL;
    EVP_CIPHER *cipher = NULL;

    ctx = EVP_CIPHER_CTX_new();
    cipher = EVP_CIPHER_fetch(NULL, CIPHER_ALGORITHM, NULL);

    if (ctx == NULL || cipher == NULL) {
        cleanUp(ctx, cipher);
        return 0;
    }

    int len, final_len;
    const int encrypted_len = base64_decoded_size(encoded_msg);
    unsigned char encrypted_msg[encrypted_len];

    if (!base64_decode(encoded_msg, encrypted_msg, encrypted_len)) {
        return 0;
    }

    if (!EVP_DecryptInit_ex2(ctx, cipher, KEY, IV, NULL)) {
        cleanUp(ctx, cipher);
        return 0;
    }

    if (!EVP_DecryptUpdate(ctx, outbuf, &len, encrypted_msg, encrypted_len)) {
        cleanUp(ctx, cipher);
        return 0;
    }

    if (!EVP_DecryptFinal(ctx, outbuf + len, &final_len)) {
        cleanUp(ctx, cipher);
        return 0;
    }

    outbuf[len + final_len] = '\0';

    cleanUp(ctx, cipher);
    return 1;
}
