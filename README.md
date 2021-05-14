# lua4gba

About
-------------------------------------------------------------------------------

This is a fork of `Lua4gba` which was originally created in far 2005 by
Torlus (https://gbadev.org/demos.php?showinfo=1212). Fork updates build tools,
scripts and removes binary from source distribution.

`lua4gba` is a quick port of Lua (http://www.lua.org/) for the Gameboy Advance.
Lua is a powerful scripting language with many high-level features, used in 
many projects.

This project is based on Lua 5.0.2 so is more up to date than GBALua 
(based on 4.0 version of Lua, https://www.gatesboy.com/Lua/Documentation/Download.html)
and was made using devkitARM.

This project uses the Lua source tree as-is, with only one small (necessary) modification
for console output.

It uses Damian Yerrick's GBFS for script embedding.

Contents
-------------------------------------------------------------------------------

Project structure :
```
src/                           GBA-specific source, and Lua startup code.
src/lua/                       Lua 5.0.2 source.
src/misc/                      Some stuff handy for porting purposes.
examples/                      Sample Lua demo with script for building.
include/                       Header files for lua and GBFS.
tools/                         Source code for GBFS tools.
                               Nowadays most of them are distributed with devkitARM.
```

Instructions
-------------------------------------------------------------------------------

There are several options to build the Lua4gba demo:

* simply run `"examples/demo.bat"` script (`"examples/demo.sh"` for Linux);

* run `make demo` from the command line.

It will output a `"examples/demo.gba"` ROM, that you can run on your console or favorite emulator.

Its source is rather self-explanatory:

* It uses GBFS tool `"padbin"` to adjust the Lua4gba.gba ROM size to a multiple
of 256 (this alignment is needed by GBFS to find its root entry).

* It builds a temporary GBFS file containing the `"examples/demo.lua"` Lua source.

* The GBFS file is appened to the `lua4gba.gba` into a new file `"demo.gba"`.

To build the `lua4gba.gba` file, you **need** a proper installation of devkitPro,
containing `devkitARM` and `libgba`. Using the "devkitPro updater" with default
settings works perfectly.

You might use another toolchain, provided that it features some minimal libc 
requirements:

* floating-point support (in software of course);

* working memory allocation routines (like `malloc()`, `realloc()`, `free()`);

* a working `sprintf()`. It should be the case if the two previous requirements
are met.

However, it is possible to get rid of these requirements, provided that you
change some stuff in the source. See the "Comments" section below.

Comments
-------------------------------------------------------------------------------

Porting Lua is really easy, mostly for two reasons:

* Lua is designed to be embedded, so its requirements are minimal, and the 
source code has been organized to enable easy porting.

* The devkitARM toolchain does most of the work by providing working C library
functions.

However, if you want to use another toolchain, or port Lua to another platform 
without all these features, here are some tricks:

* Lua uses two memory functions, `l_realloc()` and `l_free()`. If these functions 
are not defined, they default to `realloc()` and `free()`. You can simply define
them (example source code can be found in the `src/misc/` folder, use it **only**
for testing purposes).

* Lua uses `sprintf()`. Usually, implementations of `printf()` functions use 
`malloc()`, so check your C-library's implementation. You can still have a look at 
the source provided in the `src/misc/` folder.

* Many lightweight implementations of `printf()` functions (including the one
located in the `src/misc/` folder) do not implement floating point numbers formatting.
Once again, the Lua team has done it right: float formatting is not used, apart
for one case that can be avoided. By default, Lua will use `sprintf()` for floating
point numbers formatting, but if you define the `lua_number2str()` function, then
you can avoid this issue.

* Lua uses some `libm` (math) functions. All of these are used in `lmathlib.c`.

* Lua uses some other `libc` functions, but all of them are easy to replace, if 
you want to get rid of it in order to have a very small system.

Credits
-------------------------------------------------------------------------------

- Torlus, original author of this port.
    - https://torlus.github.io/

- Lua team.
    - https://www.lua.org/

- Damian Yerrick, for GBFS.
    - https://pineight.com/gba/#gbfs
