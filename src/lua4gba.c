/*
	Lua bindings. Mostly contains code from original lua.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "console.h"
#include "lua4gba.h"

static lua_State* L = NULL;
static const char* progname = PROGNAME;
const char welcome[] = "print(\"-= " PROGNAME " " PROGVER " =-\")";

char buf[500];

static const luaL_reg lualibs[] = {
	{"base", luaopen_base},
	{"table", luaopen_table},
	/*{"io", luaopen_io},*/
	{"string", luaopen_string},
	{"math", luaopen_math},
	{"debug", luaopen_debug},
	/*{"loadlib", luaopen_loadlib},*/
	/* add your libraries here */
	LUA_EXTRALIBS
	{NULL, NULL}
};

static int lcall(int narg, int clear)
{
	int status;
	int base = lua_gettop(L) - narg;  /* function index */
	lua_pushliteral(L, "_TRACEBACK");
	lua_rawget(L, LUA_GLOBALSINDEX);  /* get traceback function */
	lua_insert(L, base);  /* put it under chunk and args */
	status = lua_pcall(L, narg, (clear ? 0 : LUA_MULTRET), base);
	lua_remove(L, base);  /* remove traceback function */
	return status;
}

static int docall(int status)
{
	if (status == 0) status = lcall(0, 1);
	return report(status);
}

static void openstdlibs(lua_State* l)
{
	const luaL_reg* lib = lualibs;
	for (; lib->func; ++lib) {
		lib->func(l);  /* open library */
		lua_settop(l, 0);  /* discard any results */
	}
}

int pmain(lua_State* l)
{
	L = l;
	int status;
	Smain* s = (Smain*) lua_touserdata(l, 1);
	lua_userinit(l);  /* open libraries */
	if (s->length > 0) {
		status = dostring(s->source, "=<script>");
	} else {
		status = dostring(welcome, "=<script>");
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

int report(int status)
{
	const char *msg;
	if (status) {
		msg = lua_tostring(L, -1);
		if (msg == NULL) msg = "(error with no message)";
		l_message(progname, msg);
		lua_pop(L, 1);
	}
	return status;
}

int dostring(const char* s, const char* name)
{
	return docall(luaL_loadbuffer(L, s, strlen(s), name));
}
