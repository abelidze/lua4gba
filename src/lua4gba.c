/*
	Lua bindings. Mostly contains code from original lua.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "console.h"
#include "lua4gba.h"

static const char progname[] = LUA_PROGNAME;
const char welcome[] = "print('-= " LUA_PROGNAME " " LUA_PROGVER " =-')";

char buf[500];

static int traceback(lua_State* L)
{
	const char* msg = lua_tostring(L, 1);
	if (msg) {
		luaL_traceback(L, L, msg, 1);
	} else if (!lua_isnoneornil(L, 1)) {  /* is there an error object? */
		if (!luaL_callmeta(L, 1, "__tostring"))  /* try its 'tostring' metamethod */
			lua_pushliteral(L, "(no error message)");
	}
	return 1;
}

static int docall(lua_State* L, int narg, int nres)
{
	int base = lua_gettop(L) - narg;  /* function index */
	lua_pushcfunction(L, traceback);  /* push traceback function */
	lua_insert(L, base);  /* put it under chunk and args */
	int status = lua_pcall(L, narg, nres, base);
	lua_remove(L, base);  /* remove traceback function */
	return status;
}

int pmain(lua_State* L)
{
	int status = LUA_OK;
	Smain* s = (Smain*) lua_touserdata(L, 1);
	luaL_openlibs(L);
	if (s->length > 0) {
		char script[32] = "=";
		strcat(script, s->name);
		status = dostring(L, s->source, s->length, script);
	} else {
		status = dostring(L, welcome, strlen(welcome), "=<script>");
	}
	s->status = status;
	return 0;
}

void l_message(const char* pname, const char* msg)
{
	*buf = '\0';
	if (pname) sprintf(buf, "%s: ", pname);
	sprintf(buf + strlen(buf), "%s\n", msg);
	con_write(buf);
}

int report(lua_State* L, int status)
{
	if (status != LUA_OK && !lua_isnil(L, -1)) {
		const char* msg = lua_tostring(L, -1);
		if (msg == NULL) msg = "(error object is not a string)";
		l_message(progname, msg);
		lua_pop(L, 1);
	}
	return status;
}

int dostring(lua_State* L, const char* s, int len, const char* name)
{
	int status = luaL_loadbuffer(L, s, len, name);
	if (status == LUA_OK) status = docall(L, 0, 0);
	return report(L, status);
}
