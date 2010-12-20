#include "postgres.h"

/**
 * Get this library's version.
 */
static int module_version(lua_State *L) {
    lua_pushstring(L, LPG_VERSION);
    return 1;
}

/**
 * Allocates a new connection but does not yet connect to the server.
 */
static int module_connection(lua_State *L) {
    const char *connect_string = luaL_optstring(L, 1, "dbname = postgres");
    connection *p = ((connection *) (lua_newuserdata(L, sizeof(connection))));
    luaL_getmetatable(L, LPG_CONNECTION_METATABLE);
    lua_setmetatable(L, -2);
    p->pg_conn        = NULL;
    p->state          = LPG_CONN_NEW;
    p->connect_string = connect_string;
    return 1;
}

/**
 * Top-level module functions.
 */
static const struct luaL_Reg functions [] = {
    {"version", module_version},
    {"connection", module_connection},
    {NULL, NULL}
};

void register_module_functions(lua_State *L) {
    luaL_register(L, "postgres", functions);
}

/**
 * Lua module setup.
 */
int luaopen_postgres(lua_State *L) {
    register_module_functions(L);
    register_connection_methods(L);
    register_result_methods(L);
    register_field_methods(L);
    return 1;
}
