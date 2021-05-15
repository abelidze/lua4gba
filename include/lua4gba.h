#ifndef LUA4GBA
#define LUA4GBA
#ifdef __cplusplus
extern "C" {
#endif

#include "lauxlib.h"
#include "lualib.h"

#ifndef PROGNAME
#define PROGNAME  "lua4gba"
#endif
#ifndef PROGVER
#define PROGVER   "1.0.1"
#endif

#ifndef lua_userinit
#define lua_userinit(L)   openstdlibs(L)
#endif

#ifndef LUA_EXTRALIBS
#define LUA_EXTRALIBS /* empty */
#endif

typedef struct Smain {
  int argc;
  char** argv;
  char* source;
  int length;
  int status;
} Smain;

int pmain(lua_State* l);
int report(int status);
void l_message(const char* pname, const char* msg);
int dostring(const char* s, const char* name);

#ifdef __cplusplus
}    // extern "C"
#endif
#endif // LUA4GBA
