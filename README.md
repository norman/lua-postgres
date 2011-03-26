# Lua Postgres

A Postgres library for Lua. Other than the basic connection and query execution
functionality you would expect in any database driver, it also supports
automatic query parameter escaping via libpq, meta information about tables and
result sets, and asynchronous query execution.

It has only been tested with Postgres 9 and Lua 5.1.4. I don't intend to test
it with older versions of Postgres or Lua since I don't use them any more.

I'm doing this in part because I need it, and in part to learn more about C
programming. My total experience with C is reading K&R, and about 3 weeks of
practice. Use this library at your own risk! That said, my need for this
library grew out of frustrations with memory leaks, bugs and lack of
maintainence with other existing Lua Postgres libraries, and so I've been
fairly diligent about testing and keeping the code small and simple to read.
Any bugs will likely be easy for an experienced C programmer to spot.


## Why?

LuaSQL is nice, but the fact that it's an abstraction library means that by
design, it will not support some of Postgres's useful vendor-specific features,
like asynchronous queries, array and hash types, etc.

Other Postgres drivers I found for Lua were woefully buggy, incomplete, and
abandoned, so I've decided to work on this one.

My goal is to create a solid, general-purpose Postgres library that exposes all
of the most useful features of Postgres in a clean, Lua-idiomatic way.

## Acknowledgements

Some ideas and bits of code in this library are taken from
[Lua-Pgsql](https://github.com/alacner/lua-pgsql) and
[Luapgsql](https://github.com/STPeters/luapgsql), as well as
[LuaSQL](http://github.com/keplerproject/luasql).

## Where to get it

[http://github.com/norman/lua-postgres](http://github.com/norman/lua-postgres)

## License

MIT/X11