#include "../lua/lua.h"
#include "../lua/lualib.h"
#include "../lua/lauxlib.h"

#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

// conn = netlib.connect(ip, port)
int netlib_connect(lua_State *L) {
    const char *ip = luaL_checkstring(L, 1);
    const int port = luaL_checkinteger(L, 2);

    int conn = socket(AF_INET, SOCK_STREAM, 0);
    if (conn == -1)
        luaL_error(L, "connect:socket(): %s\n", strerror(errno));

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = { .s_addr = inet_addr(ip) }
    };

    if (connect(conn, (struct sockaddr*)&addr, sizeof(addr)) != 0)
        luaL_error(L, "connect:connect(): %s\n", strerror(errno));
    
    lua_pushinteger(L, conn);
    return 1;
}

// len = netlib.write(conn, "hello, world !\n")
int netlib_write(lua_State *L) {
    size_t data_len = 0;
    const int conn   = luaL_checkinteger(L, 1);
    const char *data = luaL_checklstring(L, 2, &data_len);

    int len = write(conn, data, data_len);

    lua_pushinteger(L, len);
    return 1;
}

// data = netlib.read(conn, 100)
int netlib_read(lua_State *L) {
    const int conn          = luaL_checkinteger(L, 1);
    const int buffer_len    = luaL_checkinteger(L, 2);

    char *buffer = malloc(buffer_len);
    if (!buffer)
        luaL_error(L, "read:malloc(): couldnt alloc memory\n");

    int len = read(conn, buffer, buffer_len);

    lua_pushlstring(L, buffer, len);
    return 1;
}

int netlib_close(lua_State *L) {
    const int conn = luaL_checkinteger(L, 1);
    
    close(conn);

    return 0;
}

static const struct luaL_Reg netlib [] = {
    {"connect", netlib_connect},
    {"close", netlib_close},
    {"write", netlib_write},
    {"read", netlib_read},
    {NULL           , NULL}
};

int luaopen_netlib(lua_State *L)
{
    luaL_newlib(L, netlib);
    lua_pushvalue(L, -1);
    lua_setglobal(L, "netlib");
    return 1;
}
