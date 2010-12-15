LUA_DIR= /usr/local
LUA_LIBDIR= $(LUA_DIR)/lib/lua/5.1
LIBFLAG= -g -Wall -shared -fpic

PG_LIBDIR=`pg_config --libdir`
PG_INCDIR=`pg_config --includedir`

# CC=llvm-gcc

postgres.so: src/*.c
	$(CC) -o postgres.so $(LIBFLAG) $(CFLAGS) src/*.c -L$(LUA_LIBDIR) -llua -I$(PG_INCDIR) -L$(PG_LIBDIR) -lpq

clean:
	$(RM) -rf postgres.so*

test: postgres.so
	tsc test/test.lua

install: postgres.so
	cp postgres.so $(LUA_LIBDIR)/

uninstall:
	$(RM) $(LUA_LIBDIR)/postgres.so

rock:
	luarocks make rockspecs/postgres-scm-1.rockspec

memtest: test/memtest.lua
	valgrind --leak-check=full lua test/memtest.lua

doc:
	luadoc -r README.md --nofiles -d docs luadocs/*.luadoc
