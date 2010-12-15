# Lua Postgres

A very basic Postgres library for Lua. It currently only supports:

* Connecting to a Postgres database
* Executing queries
* Escaping query parameters automatically
* Fetching rows indexed numerically or associatively
* Getting row count on result

It has only been tested with Postgres 9 and Lua 5.1.4. I don't intend to test it
with older versions of Postgres or Lua since I don't use them any more.

I'm doing this in part because I need it, and in part to learn more about C
programming. My total experience with C is reading K&R, and about 3 weeks of
practice. Use this library at your own risk! That said, my need for this library
grew out of frustrations with memory leaks, bugs and lack of maintainence with
other existing Lua Postgres libraries, and so I've been fairly diligent about
testing and keeping the code small and simple to read. Any bugs will likely be
easy for an experienced C programmer to spot.

With this library I'm hoping to provide a minimally usable framework to build
upon. I would like this to eventually become a very complete Postgres driver,
but for that I'll probably need the assistance of more experienced C
programmers. Want to help me? [Send me email](mailto:norman@njclarke.com).

## Acknowledgements

Some of the ideas in this library are taken from
[Lua-Pgsql](https://github.com/alacner/lua-pgsql) and
[Luapgsql](https://github.com/STPeters/luapgsql), as well as
[LuaSQL](http://github.com/keplerproject/luasql).

## Where to get it

[http://github.com/norman/lua-postgres](http://github.com/norman/lua-postgres)

## License

MIT/X11