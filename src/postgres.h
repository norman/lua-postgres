#include <lua.h>
#include <lauxlib.h>
#include <libpq-fe.h>

#define LUA_POSTGRES_VERSION "0.0.1"
#define CONNECTION_METATABLE "Lua Postgres Connection"
#define RESULT_METATABLE     "Lua Postgres Result"
#define FIELD_METATABLE     "Lua Postgres Field"

#define RESULT_CLOSED_ERROR "Attempted to access a closed result"

enum connection_state { CONN_OPEN, CONN_CLOSED, CONN_NEW, CONN_FAILED };
enum result_state { RESULT_OPEN, RESULT_CLOSED };

typedef struct {
    PGconn     *pg_conn;
    const char *connect_string;
    int        state;
} connection;

typedef struct {
    PGresult  *pg_result;
    int       current_row, num_fields, num_tuples, state;
} result;

typedef struct {
  PGresult   *pg_result;
  int        number;
  const char *name;
} field;

void register_connection_methods(lua_State *L);
void register_result_methods(lua_State *L);
void register_field_methods(lua_State *L);

int new_result(lua_State *L, PGresult *result);
int new_field(lua_State *L, PGresult *result, int number, const char *name);
