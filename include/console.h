#ifndef GBA_CONSOLE
#define GBA_CONSOLE
#ifdef __cplusplus
extern "C" {
#endif

#include <gba_video.h>

#define CONSOLE_WIDTH 30
#define CONSOLE_HEIGHT 20

void con_init(u16 background, u16 color, const u16* font);
void con_clear();
void con_clear_next_row();
void con_update_screen();
void con_update_row();
void con_write_char(int ch);
void con_write_string(const char *s);

#define con_write(X) _Generic((X),\
	char: con_write_char,\
	default: con_write_string\
)(X)

#ifdef __cplusplus
}    // extern "C"
#endif
#endif // GBA_CONSOLE
