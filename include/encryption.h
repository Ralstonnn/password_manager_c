#ifndef ENCRYPTION_H
#define ENCRYPTION_H

int encrypt_str(unsigned char *message, unsigned char *outbuf);
int decrypt_str(const char *encoded_msg, unsigned char *outbuf);
int encryption_init();

#endif
