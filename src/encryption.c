#include "common.h"
#include "password_generator.h"
#include <openssl/evp.h>
#include <stdio.h>

#define BASE64_ENCODING_TABLE                                                  \
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
#define CIPHER_ALGORITHM "AES-128-CBC"
#define KEY_LEN 16
#define IV_LEN 16
#define KEY_PATH "../key_test"
#define IV_PATH "../iv_test"

static int BASE64_DECODING_TABLE[80] = {
    62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1,
    -1, -1, -1, -1, -1, -1, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
    -1, -1, -1, -1, -1, -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
    36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};

void base64_generate_decoding_table() {
    int i;

    for (i = 0; i < 80; i++) {
        BASE64_DECODING_TABLE[i] = -1;
    }
    for (i = 0; i < 64; i++) {
        // 43 is the lowest char in encoding table (+) and should be at index 0
        BASE64_DECODING_TABLE[BASE64_ENCODING_TABLE[i] - 43] = i;
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

void get_encryption_key_and_iv(unsigned char *out_key, unsigned char *out_iv) {
    FILE *file_key, *file_iv;

    file_key = fopen(KEY_PATH, "r");
    file_iv = fopen(IV_PATH, "r");

    if (file_key == NULL) {
        file_key = fopen(KEY_PATH, "w");
        if (file_key == NULL)
            return;
        generate_password((char *)out_key, KEY_LEN);
        fprintf(file_key, "%s", out_key);
        fclose(file_key);
    } else {
        fgets((char *)out_key, KEY_LEN + 1, file_key);
        fclose(file_key);
    }

    if (file_iv == NULL) {
        file_iv = fopen(IV_PATH, "w");
        if (file_iv == NULL)
            return;
        generate_password((char *)out_iv, IV_LEN);
        fprintf(file_iv, "%s", out_iv);
        fclose(file_key);
    } else {
        fgets((char *)out_iv, IV_LEN + 1, file_iv);
        fclose(file_iv);
    }
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
    const unsigned char key[KEY_LEN + 1];
    const unsigned char iv[IV_LEN + 1];
    int len, final_len;

    get_encryption_key_and_iv((unsigned char *)key, (unsigned char *)iv);

    if (!EVP_EncryptInit_ex2(ctx, cipher, key, iv, NULL)) {
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

    int len, final_len, encrypted_len;
    const unsigned char key[KEY_LEN + 1];
    const unsigned char iv[IV_LEN + 1];
    encrypted_len = base64_decoded_size(encoded_msg);
    unsigned char encrypted_msg[encrypted_len];

    get_encryption_key_and_iv((unsigned char *)key, (unsigned char *)iv);

    if (!base64_decode(encoded_msg, encrypted_msg, encrypted_len)) {
        return 0;
    }

    if (!EVP_DecryptInit_ex2(ctx, cipher, key, iv, NULL)) {
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
