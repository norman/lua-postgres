/***
A Postgres connection.

@module postgres.connection
*/
#include "postgres.h"

static connection* get_connection(lua_State *L) {
    return (connection *) luaL_checkudata(L, 1, LPG_CONNECTION_METATABLE);
}

static int connect(lua_State *L, connection *conn) {
    PGconn *pg_conn;
    pg_conn = PQconnectdb(conn->connect_string);
    if (PQstatus(pg_conn) != CONNECTION_OK) {
        conn->state   = LPG_CONN_FAILED;
        lua_pushboolean(L, 0);
        lua_pushstring(L, PQerrorMessage(pg_conn));
        PQfinish(conn->pg_conn);
        return 0;
    }
    else {
        conn->pg_conn = pg_conn;
        conn->state   = LPG_CONN_OPEN;
        return 1;
    }
}

static void close(connection *conn) {
    if (conn->state == LPG_CONN_OPEN) {
        conn->state = LPG_CONN_CLOSED;
        PQfinish(conn->pg_conn);
        conn->pg_conn = NULL;
    }
}

// conn_* functions are exported as Lua methods.

/// Opens the connection to the Postgres server.
// @function open
// @return status - Whether or not the connection succeeded.
// @return message - If the connection failed to open, an error message.
// @usage
// local ok, err = conn:open()
// if err then error(err) end
static int conn_open(lua_State *L) {
    connection *conn = get_connection(L);
    luaL_argcheck(L, conn->state != LPG_CONN_OPEN, 1, "connection already open");
    luaL_argcheck(L, conn->state != LPG_CONN_CLOSED, 1, "connection closed");
    if (connect(L, conn)) {
        lua_pushboolean(L, 1);
        lua_pushnil(L);
    }
    return 2;
}

static int conn_close(lua_State *L) {
    connection *conn = get_connection(L);
    luaL_argcheck(L, conn->state != LPG_CONN_NEW, 1, "connection is new");
    luaL_argcheck(L, conn->state != LPG_CONN_CLOSED, 1, "connection already closed");
    close(conn);
    lua_pushnil(L);
    return 1;
}

static int process_result(lua_State *L, connection *conn, PGresult *pg_result) {
    if (pg_result && PQresultStatus(pg_result) == PGRES_COMMAND_OK) {
        /* no tuples returned */
        lua_pushnumber(L, atof(PQcmdTuples(pg_result)));
        PQclear(pg_result);
        return 1;
    }
    else if (pg_result && PQresultStatus(pg_result) == PGRES_TUPLES_OK)
        /* tuples returned */
        return new_result(L, conn, pg_result);
    else {
        /* error */
        PQclear(pg_result);
        lua_pushnil(L);
        lua_pushstring(L, PQerrorMessage(conn->pg_conn));
        return 2;
    }
}

static int execute_without_params(lua_State *L) {
    connection *conn = get_connection(L);
    const char *statement = luaL_checkstring(L, 2);
    PGresult *pg_result = PQexec(conn->pg_conn, statement);
    return process_result(L, conn, pg_result);
}

static int execute_with_params(lua_State *L) {
    connection *conn      = get_connection(L);
    const char *statement = luaL_checkstring(L, 2);
    char **params         = NULL;
    int num_params, i;

    if (lua_istable(L, 3)) {
        num_params = lua_objlen(L, 3);
        if (num_params > 0) {
            params = (char **) malloc(num_params * sizeof(char *));
            if(params == NULL) {
              fputs("Out of memory.\n", stderr);
              exit(EXIT_FAILURE);
            }
            for (i = 0; i < num_params; i++) {
                lua_rawgeti(L, 3, i + 1);
                params[i] = (char *) lua_tostring(L, -1);
                lua_pop(L, 1);
            }
        }
    }
    else if (lua_isnone(L, 3) || lua_isnil(L, 3)) {
        num_params = 0;
    }
    else {
        num_params = 1;
        params = (char **) malloc(sizeof(char *));
        params[0] = (char *) lua_tostring(L, 3);
    }
    PGresult *pg_result = PQexecParams(conn->pg_conn, statement, num_params, NULL,
        (const char * const *) params, NULL, NULL, 0);
    free(params);
    return process_result(L, conn, pg_result);
}

/// Executes a query.
// @function execute
// @param sql An SQL query
// @param params A single parameter, or a table of parameters
// @return result - A result userdata or nil on failure.
// @return message - If execution failed, an error message
// @usage
// local result, error = conn:execute("SELECT * FROM users WHERE name = $1", "joe")
// local result, error = conn:execute("SELECT * FROM users WHERE name = $1 AND age < $2", {"joe", 100})
static int conn_execute(lua_State *L) {
    connection *conn = get_connection(L);

    /* Lazy connect */
    if (conn->state == LPG_CONN_NEW) {
        if (!connect(L, conn)) {
            return 2;
        }
    }

    if (lua_gettop(L) == 2) {
        return execute_without_params(L);
    } else {
        return execute_with_params(L);
    }
}

/**
 * Invoked by Lua when connection is garbage collected.
 */
static int connection_gc(lua_State *L) {
    connection *conn = get_connection(L);
    close(conn);
    return 0;
}

static const luaL_Reg methods[] = {
    {"open",    conn_open},
    {"close",   conn_close},
    {"execute", conn_execute},
    {NULL, NULL}
};

/**
 * Push the methods into the Lua module.
 */
void register_connection_methods(lua_State *L) {
    luaL_newmetatable(L, LPG_CONNECTION_METATABLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, methods);
    lua_pushcfunction(L, connection_gc);
    lua_setfield(L, -2, "__gc");
    lua_pop(L, 2);
}
