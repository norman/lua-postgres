#include <string.h>
#include <ctype.h>
#include "postgres.h"

static result* get_result(lua_State *L) {
    return (result *) luaL_checkudata(L, 1, RESULT_METATABLE);
}

/**
 * Invoked by Lua when result is garbage collected.
 */
static int result_gc(lua_State *L) {
    result *res = get_result(L);
    PQclear(res->pg_result);
    res->pg_result = NULL;
    return 0;
}

/**
 * While iterating through a row, push each field value onto a previously
 * initialized Lua table.
 */
static void push_value(lua_State *L, result *res, int i) {
    if (PQgetisnull(res->pg_result, res->current_row, i-1)) {
        lua_pushnil(L);
    }
    else {
        lua_pushstring(L, PQgetvalue(res->pg_result, res->current_row, i-1));
    }
}

// All result_* functions are exported to Lua.

static int result_fetch(lua_State *L) {
    int i;
    result *res = get_result(L);
    if (res->current_row >= res->num_rows) {
        lua_pushnil(L);
        return 1;
    }
    lua_newtable(L);
    for (i = 1; i <= res->num_cols; i++) {
        push_value(L, res, i);
        lua_rawseti(L, -2, i);
    }
    res->current_row++;
    return 1;
}

static int result_fetch_assoc(lua_State *L) {
    int i;
    result *res = get_result(L);
    if (res->current_row >= res->num_rows) {
        lua_pushnil(L);
        return 1;
    }
    lua_newtable(L);
    for (i = 1; i <= res->num_cols; i++) {
        lua_pushstring(L, PQfname(res->pg_result, i-1));
        push_value(L, res, i);
        lua_rawset(L, -3);
    }
    res->current_row++;
    return 1;
}

static int result_num_rows(lua_State *L) {
    result *res = get_result(L);
    lua_pushinteger(L, res->num_rows);
    return 1;
}

static const luaL_Reg methods[] = {
    {"fetch",       result_fetch},
    {"fetch_assoc", result_fetch_assoc},
    {"num_rows",    result_num_rows},
    {NULL, NULL}
};

// Public functions

int new_result(lua_State *L, PGresult *pg_result) {
    result *r = ((result *) (lua_newuserdata(L, sizeof(result))));
    luaL_getmetatable(L, RESULT_METATABLE);
    lua_setmetatable(L, -2);
    r->pg_result   = pg_result;
    r->current_row = 0;
    r->num_cols = PQnfields(pg_result);
    r->num_rows = PQntuples(pg_result);
    return 1;
}

void register_result_methods(lua_State *L) {
    luaL_newmetatable(L, RESULT_METATABLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, methods);
    lua_pushcfunction(L, result_gc);
    lua_setfield(L, -2, "__gc");
    lua_pushcfunction(L, result_num_rows);
    lua_setfield(L, -2, "__len");
    lua_pop(L, 2);
}