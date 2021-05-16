About lua4gba
===============================================================================

This is a fork of `Lua4gba` which was originally created in far 2005 by
[Torlus](https://gbadev.org/demos.php?showinfo=1212). Fork updates build tools,
scripts and removes binary from source distribution.

`lua4gba` is a quick port of [Lua](http://www.lua.org/) for the Gameboy Advance.
Lua is a powerful scripting language with many high-level features, used in 
many projects.

This project is based on Lua 5.2.4 so is more up to date than [GBALua](https://www.gatesboy.com/Lua/Documentation/Download.html) 
(based on 4.0 version of Lua)
and was made using devkitARM.

This project uses the Lua source tree as-is, with only one small (necessary) modification
for console output.

It uses Damian Yerrick's GBFS for script embedding.

Contents
-------------------------------------------------------------------------------

Project structure :
```
src/                           GBA-specific source, and Lua startup code.
src/lua/                       Submodule for Lua 5.2.4 sources.
src/misc/                      Some stuff handy for porting purposes.
examples/                      Sample Lua demo with script for building.
include/                       Header files for lua and GBFS.
tools/                         Source code for GBFS tools.
                               Nowadays most of them are distributed with devkitARM.
```

Installation
-------------------------------------------------------------------------------

1. Go to [releases](https://github.com/abelidze/lua4gba/releases) page.
2. Download one of packages in the latest available release:
    - `lua4gba.gba` - write your lua code and combine it using GBFS tools.
    - `demo.gba` - test how it works on emulator or real GBA.
    - `tools-win.zip` - GBFS tools binaries for Windows.
    - `tools-unix.zip` - GBFS tools binaries for Linux.
    - `Source code.zip` - sources for modifications in C/C++.

3. Considering you have been downloaded `lua4gba.gba` also download and unzip `tools-*.zip`.
They are required to create your game's resources bundles (`.gbfs` archives).
Alternatively you can use tools distributed with `devkitPro`.

4. If you have compiled `lua4gba.gba` by yourself, dont forget to use `padbin`.
Otherwise GBFS wouldn't work properly:
```sh
$ padbin 0x100 lua4gba.gba
```

5. Create GBFS archive for your files using `gbfs` tool:
```sh
$ gbfs bundle.gbfs file1.lua file2.txt file3.bin
```

6. Append your GBFS data to `lua4gba.gba`:

> **Windows**

```sh
$ copy /b lua4gba.gba + bundle.gbfs rom.gba
```

> **Linux**

```sh
$ cat lua4gba.gba bundle.gbfs > rom.gba
```

7. Now you are ready to start development!

Alternatively you can download project as '.zip' archive and extract it to whatever you like directory or use `git`:
```sh
$ git clone https://github.com/abelidze/lua4gba.git
```

It would be similar to `Source code.zip` distribution but also including all recent changes made to the project.

Building
-------------------------------------------------------------------------------

> **lua4gba**

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

Once you have `devkitPro` set up simply use `make`:

```sh
$ git clone https://github.com/abelidze/lua4gba.git
$ cd lua4gba && make
$ ls -la lua4gba.gba
```

> **Demo**

There are several options to build the `lua4gba` demo:

* simply run `"examples/demo.bat"` script (`"examples/demo.sh"` for Linux);

* run `make demo` from the command line.

It will output a `"examples/demo.gba"` ROM, that you can run on your console or favorite emulator.

Its source is rather self-explanatory:

* It uses GBFS tool `"padbin"` to adjust the Lua4gba.gba ROM size to a multiple
of 256 (this alignment is needed by GBFS to find its root entry).

* It builds a temporary GBFS file containing the `"examples/demo.lua"` Lua source.

* The GBFS file is appened to the `lua4gba.gba` into a new file `"demo.gba"`.

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

License
-------------------------------------------------------------------------------

`lua4gba` is an open-sourced software licensed under the [MIT License](https://opensource.org/licenses/MIT).

This project also uses some parts of third-party libraries listed below:

- Lua4gba by Torlus, original author of this port.
    - https://torlus.github.io/

- Lua 5.2.4, Lua team.
    - https://www.lua.org/

- GBFS, Damian Yerrick.
    - https://pineight.com/gba/#gbfs
