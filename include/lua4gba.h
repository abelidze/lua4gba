#ifndef LUA4GBA
#define LUA4GBA
#ifdef __cplusplus
extern "C" {
#endif

#include "lauxlib.h"
#include "lualib.h"

#ifndef LUA_PROGNAME
#define LUA_PROGNAME  "lua4gba"
#endif
#ifndef LUA_PROGVER
#define LUA_PROGVER   "1.1.0"
#endif

typedef struct Smain {
  char* name;
  char* source;
  int length;
  int status;
} Smain;

int pmain(lua_State* L);
int report(lua_State* L, int status);
void l_message(const char* pname, const char* msg);
int dostring(lua_State* L, const char* s, int len, const char* name);

#ifdef __cplusplus
}    // extern "C"
#endif
#endif // LUA4GBA
