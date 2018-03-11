#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define LUAUDP_SOCKET "luaudp.socket"
#define MAX_BUFFER 5000

int luaudp_new(lua_State *L)
{
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
    {
        luaL_error(L, "Socket creation error: %s", strerror(errno));
    }

    *((int *)lua_newuserdata(L, sizeof(s))) = s;
    luaL_getmetatable(L, LUAUDP_SOCKET);
    lua_setmetatable(L, -2);
    return 1;
}
int luaudp_bind(lua_State *L)
{
    int s = *(int *)luaL_checkudata(L, 1, LUAUDP_SOCKET);

    size_t ip_size;
    const char *ip = luaL_checklstring(L, 2, &ip_size);
    int port = luaL_checknumber(L, 3);

    if (port <= 0 || port > 65535)
    {
        luaL_error(L, "Port number out of range");
    }

    // Bind socket
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons((u_short)port);
    addr.sin_addr.s_addr = inet_addr(ip);
    if (bind(s, (const struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        luaL_error(L, "Socket bind error: %s", strerror(errno));
    }

    return 0;
}
int luaudp_read(lua_State *L)
{
    int s = *(int *)luaL_checkudata(L, 1, LUAUDP_SOCKET);
    size_t size = luaL_checknumber(L, 2);
    size_t read_size;

    luaL_argcheck(L, size > 0 && size <= MAX_BUFFER, 2, "Read size out of range.");

    char *buffer = malloc(size);
    luaL_argcheck(L, buffer != NULL, 2, "Buffer alloc fail.");

    struct sockaddr src_addr;
    socklen_t addrlen = sizeof(src_addr);
    if ((read_size = recvfrom(s, buffer, size, 0, (struct sockaddr *)&src_addr, &addrlen)) == -1)
    {
        free(buffer);
        luaL_error(L, "Read error: %s", strerror(errno));
    }

    lua_createtable(L, read_size, 0);
    for (int i = 0; i < read_size; i++)
    {
        lua_pushinteger(L, buffer[i]);
        lua_rawseti(L, -2, i + 1);
    }
    lua_pushstring(L, inet_ntoa(((struct sockaddr_in *)&src_addr)->sin_addr));
    lua_pushinteger(L, ntohs(((struct sockaddr_in *)&src_addr)->sin_port));

    free(buffer);
    return 3;
}
int luaudp_write(lua_State *L)
{
    int s = *(int *)luaL_checkudata(L, 1, LUAUDP_SOCKET);
    luaL_checktype(L, 2, LUA_TTABLE);
    size_t size = luaL_len(L, 2);

    size_t ip_size;
    const char *ip = luaL_checklstring(L, 3, &ip_size);
    int port = luaL_checknumber(L, 4);

    luaL_argcheck(L, ip_size >= 8, 3, "Invalid IP");

    if (port <= 0 || port > 65535)
    {
        luaL_error(L, "Port number out of range");
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons((u_short)port);
    addr.sin_addr.s_addr = inet_addr(ip);

    char *buffer = malloc(size);
    luaL_argcheck(L, buffer != NULL, 2, "Buffer alloc fail.");
    for (int i = 0; i < size; i++)
    {
        lua_rawgeti(L, 2, i + 1);
        buffer[i] = lua_tointeger(L, -1);
    }

    if (sendto(s, buffer, size, 0, (struct sockaddr *)&addr, sizeof(addr)) != size)
    {
        free(buffer);
        luaL_error(L, "Send error: %s", strerror(errno));
    }

    free(buffer);
    return 0;
}

int luaudp_close(lua_State *L)
{
    int *s = (int *)luaL_checkudata(L, 1, LUAUDP_SOCKET);
    if (*s)
        close(*s);
    *s = 0;
    return 0;
}

static const struct luaL_Reg libluaudp_funtions[] = {
    {"new", luaudp_new},
    {NULL, NULL} /* sentinel */
};
static const struct luaL_Reg libluaudp_socket_methods[] = {
    {"bind", luaudp_bind},
    {"read", luaudp_read},
    {"write", luaudp_write},
    {"close", luaudp_close},
    {"__gc", luaudp_close},
    {NULL, NULL} /* sentinel */
};

int luaopen_libluaudp(lua_State *L)
{
    luaL_newmetatable(L, LUAUDP_SOCKET);
    /* Duplicate the metatable on the stack (We know have 2). */
    lua_pushvalue(L, -1);
    /* Pop the first metatable off the stack and assign it to __index
     * of the second one. We set the metatable for the table to itself.
     * This is equivalent to the following in lua:
     * metatable = {}
     * metatable.__index = metatable
     */
    lua_setfield(L, -2, "__index");

    /* Set the methods to the metatable that should be accessed via object:func */
    luaL_setfuncs(L, libluaudp_socket_methods, 0);
    /* Register the object.func functions into the table that is at the top of the
     * stack. */
    luaL_newlib(L, libluaudp_funtions);
    return 1;
}