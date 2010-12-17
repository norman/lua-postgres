#include "postgres.h"

int new_field(lua_State *L, PGresult *pg_result, int number, const char *name) {
    field *f = ((field *) (lua_newuserdata(L, sizeof(field))));
    luaL_getmetatable(L, FIELD_METATABLE);
    lua_setmetatable(L, -2);
    f->pg_result = pg_result;
    f->number    = number;
    f->name      = name;
    return 1;
}

static field* get_field(lua_State *L) {
    return (field *) luaL_checkudata(L, 1, FIELD_METATABLE);
}

static int field_name(lua_State *L) {
    field *f = get_field(L);
    lua_pushstring(L, f->name);
    return 1;
}

static int field_number(lua_State *L) {
    field *f = get_field(L);
    lua_pushinteger(L, f->number);
    return 1;
}

static int field_table_oid(lua_State *L) {
    field *f = get_field(L);
    lua_pushnumber(L, PQftable(f->pg_result, f->number));
    return 1;
}

static int field_table_column_number(lua_State *L) {
    field *f = get_field(L);
    lua_pushnumber(L, PQftablecol(f->pg_result, f->number));
    return 1;
}

static int field_is_binary(lua_State *L) {
    field *f = get_field(L);
    lua_pushboolean(L, PQfformat(f->pg_result, f->number) == 1);
    return 1;
}

static int field_type_oid(lua_State *L) {
    field *f = get_field(L);
    lua_pushnumber(L, PQftype(f->pg_result, f->number));
    return 1;
}

static int field_size(lua_State *L) {
    field *f = get_field(L);
    lua_pushnumber(L, PQfsize(f->pg_result, f->number));
    return 1;
}

static int field_modifier(lua_State *L) {
    field *f = get_field(L);
    lua_pushnumber(L, PQfmod(f->pg_result, f->number));
    return 1;
}

static const luaL_Reg methods[] = {
    {"is_binary",           field_is_binary},
    {"modifier",            field_modifier},
    {"name",                field_name},
    {"number",              field_number},
    {"size",                field_size},
    {"table_column_number", field_table_column_number},
    {"table_oid",           field_table_oid},
    {"type_oid",            field_type_oid},
    {NULL, NULL}
};

void register_field_methods(lua_State *L) {
    luaL_newmetatable(L, FIELD_METATABLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, methods);
    /* lua_pushcfunction(L, result_gc); */
    /* lua_setfield(L, -2, "__gc"); */
    lua_pop(L, 2);
}
