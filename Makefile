.PHONY: test

LUA_DIR    = /usr/local
LUA_LIBDIR = $(LUA_DIR)/lib/lua/5.1
LIBFLAG    = -g -Wall -shared -fpic
PG_LIBDIR  = `pg_config --libdir`
PG_INCDIR  = `pg_config --includedir`

CC=llvm-gcc

postgres/core.so: src/*.c
	@-mkdir -p postgres
	$(CC) -o postgres/core.so $(ARCHFLAGS) $(LIBFLAG) $(CFLAGS) src/*.c -L$(LUA_LIBDIR) -llua -I$(PG_INCDIR) -L$(PG_LIBDIR) -lpq

clean:
	$(RM) -r postgres

test: postgres/core.so
	@-tsc test/test.lua

install: postgres/core.so
	mkdir -p $(LUA_LIBDIR)/postgres
	cp -r postgres/core.so $(LUA_LIBDIR)/postgres/core.so

uninstall:
	-rm  $(LUA_LIBDIR)/postgres/core.so
	-rmdir $(LUA_LIBDIR)/postgres

rock:
	luarocks make rockspecs/postgres-scm-1.rockspec

memtest: test/memtest.lua
	@valgrind --leak-check=full lua test/memtest.lua

doc:
	ldoc.lua -f markdown src/
