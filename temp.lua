package.cpath = "./?.so"
package.path = string.format("test/?.lua;%s", package.path)

require "postgres"
local pretty = require "pl.pretty"

local config = require "test_config"
local conn, err = postgres.connection(config.connection_string)
if err then error(err) end

local function emit(query)
  local result, err = conn:execute(query)
  if not result then error(err) end
  return result
end

emit(config.drop_table)
emit(config.create_table)
emit "insert into people (name) values ('Joe Schmoe')"
emit "insert into people (name) values ('John Doe')"
emit "insert into people (name) values ('Jim Beam')"

local conn = postgres.connection(config.connection_string)
local result, err = conn:execute("SELECT * FROM people WHERE id < $1", {1000})
local fields = result:fields()
for i, v in ipairs(fields) do
  print(i, v, v:name(), v:number(), v:table_oid(), v:table_column_number(), v:is_binary(), v:type_oid())
end
