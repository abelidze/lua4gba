#include <gba_base.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include <gba_interrupt.h>
#include <gba_sprites.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef LINK_MBV2
#include "mbv2.h"
#endif

#ifdef LINK_XBOO
#include "xcomms.h"
#endif

#include "gbfs.h"

//// >>> lua.c
#include "lauxlib.h"
#include "lualib.h"
#ifndef PROGNAME
#define PROGNAME	"Lua4gba"
#endif

#ifndef lua_userinit
#define lua_userinit(L)		openstdlibs(L)
#endif

#ifndef LUA_EXTRALIBS
#define LUA_EXTRALIBS	/* empty */
#endif

static lua_State *L = NULL;
static const char *progname = PROGNAME;

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
//// <<< lua.c
char *argv[1] = { PROGNAME };
int argc = 1;
const char *welcome = "print(\"-= Lua4gba =-\")";


// Useful definitions for handling background data, from Damian Yerrick
typedef u16 NAMETABLE[32][32];
#define MAP ((NAMETABLE *)0x06000000)

// 8x8 Font
extern const u8 gba_font[];

// Console Data
u16 console[20][30];
// Cursor position
int row,col;

// Source Files
int filesCount;
GBFS_FILE *gbfs;
GBFS_ENTRY *gbfs_entry;
u8 *sourcePos;
u32 sourceLen;

// Utilities
void write_char(int ch);
void write_string(const char *s);

char buf[500];

//// >>> lua.c
//Torlus - slightly modified
static void l_message (const char *pname, const char *msg)
{
	*buf='\0';
	if (pname) sprintf(buf, "%s: ", pname);
	sprintf(buf+strlen(buf), "%s\n", msg);
	write_string(buf);
}

static int report (int status)
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

static int lcall (int narg, int clear)
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

static int docall (int status)
{
	if (status == 0) status = lcall(0, 1);
	return report(status);
}

static int dostring (const char *s, const char *name)
{
	return docall(luaL_loadbuffer(L, s, strlen(s), name));
}

static void openstdlibs (lua_State *l)
{
	const luaL_reg *lib = lualibs;
	for (; lib->func; lib++) {
		lib->func(l);  /* open library */
		lua_settop(l, 0);  /* discard any results */
	}
}

struct Smain
{
	int argc;
	char **argv;
	int status;
};

static int pmain (lua_State *l)
{
	struct Smain *s = (struct Smain *)lua_touserdata(l, 1);
	int status;

	L = l;
	lua_userinit(l);  /* open libraries */
	if (sourceLen > 0) {
		status = dostring((char *)sourcePos,"=<script>");
	} else {
		status = dostring(welcome,"=<script>");
	}
	s->status = status;
	return 0;
}

//// <<< lua.c

int main()
{
	int i;
	u16 *src, *dst;

	// Set up the interrupt handlers
	irqInit();
	// Enable Vblank Interrupt to allow VblankIntrWait
	irqEnable(IRQ_VBLANK);
	// Allow Interrupts
	REG_IME = 1;

	// Init Background Palette
	BG_COLORS[0] = RGB5(0, 0, 0);
	BG_COLORS[1] = RGB5(15, 15, 31);

	// Copy Font Data
	//memcpy((u16 *)CHAR_BASE_ADR(0),gba_font,256*16*2);
	src = (u16 *)gba_font;
	dst = (u16 *)CHAR_BASE_ADR(0);
	for(i=0; i<256*16*2; i++) *dst++ = *src++;

	// Init Background 0
	REG_BG0HOFS = 0;
	REG_BG0VOFS = 0;
	BGCTRL[0] = CHAR_BASE(0) | BG_256_COLOR | SCREEN_BASE(8) | BG_SIZE_0;

	// Clear the background
	for(i=0; i<32*32; i++) MAP[8][0][i] = ' ';

	// Initialize sprites (outside of screen)
	OBJATTR obj_attr = {160, 240, 0, 0};
	for(i=0; i<128; i++) OAM[i] = obj_attr;

	// Screen Mode & Background to display & Sprites
	SetMode(MODE_0 | BG0_ENABLE | OBJ_ENABLE | OBJ_1D_MAP);

	// Initialize Console Data
	row = col = 0;
	for(i=0; i<30*20; i++) console[0][i] = ' ';

	// Find the embedded source, if any
	gbfs_entry = NULL;
	gbfs = (GBFS_FILE *)find_first_gbfs_file(find_first_gbfs_file);
	sourceLen = 0;

	if (gbfs != NULL) {
		filesCount = gbfs->dir_nmemb-1;
		gbfs_entry = (GBFS_ENTRY *)((char *)gbfs + gbfs->dir_off);        
		sourcePos = (u8 *)gbfs_get_obj(gbfs,gbfs_entry->name,&sourceLen);
	}

//// >>> lua.c
	int status;
	struct Smain s;
	lua_State *l = lua_open();  /* create state */
	if (l == NULL) {
		l_message(argv[0], "cannot create state: not enough memory");
		return EXIT_FAILURE;
	}
	s.argc = argc;
	s.argv = argv;
	status = lua_cpcall(l, &pmain, &s);
	report(status);
	lua_close(l);
//// <<< lua.c

	// Loop forever
	while(1) VBlankIntrWait();

	// Never reached
	return 0;
}

void write_char(int ch)
{
	int lastrow = row;
	int x,y,i;
	if (ch >= 32) { 
		console[row][col++] = ch;		
		if (col == 30) {
			col=0;
			row++;
			if (row == 20) row = 0;
			// Clean the next colon
			for(x=0; x<30; x++) console[row][x] = ' ';
		}
	} else if (ch == '\n') {
		col=0;
		row++;
		if (row == 20) row = 0;		
		// Clean the next colon
		for(x=0; x<30; x++) console[row][x] = ' ';
	} else if (ch == 0x08) {
		console[row][col--] = ' ';
		if (col < 0) {
			col = 29;
			row--;
			if (row < 0) row = 19;
		}
	}
	if (lastrow != row) {
		// Update the whole screen
		y = row+1;
		if (y > 19) y=0;
		for(i=0; i<20; i++) {
			for(x=0; x<30; x++) {
				MAP[8][i][x] = console[y][x];
			}
			y++;
			if (y > 19) y=0;
		}
	} else {
		// Update only the current row
		for(x=0; x<30; x++) {
			MAP[8][19][x] = console[row][x];
		}
	}
}

void write_string(const char *s)
{
	if (s == NULL) return;
	while(*s) write_char(*s++);	
}
