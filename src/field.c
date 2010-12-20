#include "postgres.h"

int new_field(lua_State *L, result *result, int number, const char *name) {
    field *f = lua_newuserdata(L, sizeof(field));
    luaL_getmetatable(L, LPG_FIELD_METATABLE);
    lua_setmetatable(L, -2);
    f->result    = result;
    f->number    = number;
    f->name      = name;
    return 1;
}

static field* get_field(lua_State *L) {
    return (field *) luaL_checkudata(L, 1, LPG_FIELD_METATABLE);
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
    lua_pushnumber(L, PQftable(f->result->pg_result, f->number));
    return 1;
}

static int field_table_column_number(lua_State *L) {
    field *f = get_field(L);
    lua_pushnumber(L, PQftablecol(f->result->pg_result, f->number));
    return 1;
}

static int field_is_binary(lua_State *L) {
    field *f = get_field(L);
    lua_pushboolean(L, PQfformat(f->result->pg_result, f->number) == 1);
    return 1;
}

static int field_type_oid(lua_State *L) {
    field *f = get_field(L);
    lua_pushnumber(L, PQftype(f->result->pg_result, f->number));
    return 1;
}

static void do_oid_query(lua_State *L, field *f, const char *query, Oid oid) {
    lua_pushfstring(L, query, oid);
    char *sql = (char *) lua_tolstring(L, -1, NULL);
    PGresult *pg_result = PQexec(f->result->conn->pg_conn, sql);
    if (PQresultStatus(pg_result) != PGRES_TUPLES_OK) {
        luaL_error(L, PQerrorMessage(f->result->conn->pg_conn));
    }
    lua_pushstring(L, PQgetvalue(pg_result, 0, 0));
    PQclear(pg_result);
}

static int field_type_name(lua_State *L) {
    field *f = get_field(L);
    do_oid_query(L, f, LPG_FIELD_TYPE_NAME_QUERY, PQftype(f->result->pg_result, f->number));
    return 1;
}

static int field_table_name(lua_State *L) {
    field *f = get_field(L);
    do_oid_query(L, f, LPG_FIELD_TABLE_NAME_QUERY, PQftable(f->result->pg_result, f->number));
    return 1;
}

static int field_size(lua_State *L) {
    field *f = get_field(L);
    lua_pushnumber(L, PQfsize(f->result->pg_result, f->number));
    return 1;
}

static int field_modifier(lua_State *L) {
    field *f = get_field(L);
    lua_pushnumber(L, PQfmod(f->result->pg_result, f->number));
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
    {"type_name",           field_type_name},
    {"table_name",          field_table_name},
    {NULL, NULL}
};

void register_field_methods(lua_State *L) {
    luaL_newmetatable(L, LPG_FIELD_METATABLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, methods);
    /* lua_pushcfunction(L, result_gc); */
    /* lua_setfield(L, -2, "__gc"); */
    lua_pop(L, 2);
}
