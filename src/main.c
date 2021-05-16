#include <gba_base.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include <gba_interrupt.h>
#include <gba_sprites.h>
#include <stdlib.h>
#include <string.h>

#ifdef LINK_XBOO
#include "xcomms.h"
#endif

#include "gbfs.h"
#include "console.h"
#include "lua4gba.h"

extern const u8 gba_font[];

int main()
{
	// Set up the interrupt handlers
	irqInit();
	// Enable Vblank Interrupt to allow VblankIntrWait
	irqEnable(IRQ_VBLANK);

	// Initialize the console
	con_init(RGB5(0, 0, 0), RGB5(18, 5, 5), (u16*) gba_font);

	// Screen Mode & Background to display & Sprites
	SetMode(MODE_0 | BG0_ENABLE | OBJ_ENABLE | OBJ_1D_MAP);

	// Set arguments for lua's pmain()
	Smain s;
	s.length = 0;

	// Find the embedded source, if any
	const GBFS_FILE* gbfs = find_first_gbfs_file(find_first_gbfs_file);
	if (gbfs != NULL) {
		GBFS_ENTRY* entry = (GBFS_ENTRY*) ((char*) gbfs + gbfs->dir_off);
		s.name = entry->name;
		s.source = (char*) gbfs_get_obj(gbfs, s.name, (u32*) &s.length);
	}

	// Initialize and run lua 
	lua_State* l = luaL_newstate();
	if (l == NULL) {
		l_message(LUA_PROGNAME, "cannot create state: not enough memory");
		return 1;
	}
	lua_pushcfunction(l, &pmain);
	lua_pushlightuserdata(l, &s);
	report(l, lua_pcall(l, 1, 0, 0));
	lua_close(l);

	// Loop forever
	for (;;) VBlankIntrWait();

	// Never reached
	return 0;
}
