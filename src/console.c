#include <stdlib.h>
#include "console.h"

// Console Data
u16 console[CONSOLE_HEIGHT][CONSOLE_WIDTH];

// Cursor position
int row = 0;
int col = 0;

void con_init(u16 background, u16 color, const u16* font)
{
	// Init Background Palette
	BG_COLORS[0] = background;
	BG_COLORS[1] = color;

	// Init Background 0
	REG_BG0HOFS = 0;
	REG_BG0VOFS = 0;
	BGCTRL[0] = CHAR_BASE(0) | BG_256_COLOR | SCREEN_BASE(8) | BG_SIZE_0;

	// Copy Font Data
	// memcpy((u16*) CHAR_BASE_ADR(0), font, 256 * 16 * 2);
	u16* src = (u16*) font;
	u16* dst = (u16*) CHAR_BASE_ADR(0);
	for (int i = 0; i < 256 * 16 * 2; ++i) {
		*dst++ = *src++;
	}

	// Clear and update screen
	con_clear();
	con_update_screen();
}

void con_clear()
{
	row = col = 0;
	for (int i = 0; i < CONSOLE_WIDTH * CONSOLE_HEIGHT; ++i) {
		*(*console + i) = ' ';
	}
}

void con_clear_next_row()
{
	col = 0;
	if (++row == CONSOLE_HEIGHT) {
		row = 0;
	}
	for (int x = 0; x < CONSOLE_WIDTH; ++x) {
		console[row][x] = ' ';
	}
}

void con_update_screen()
{
	int y = row + 1;
	if (y >= CONSOLE_HEIGHT) {
		y = 0;
	}
	for (int i = 0; i < CONSOLE_HEIGHT; ++i) {
		for (int x = 0; x < CONSOLE_WIDTH; ++x) {
			MAP[8][i][x] = console[y][x];
		}
		if (++y >= CONSOLE_HEIGHT) {
			y = 0;
		}
	}
}

void con_update_row()
{
	for (int x = 0; x < CONSOLE_WIDTH; ++x) {
		MAP[8][CONSOLE_HEIGHT - 1][x] = console[row][x];
	}
}

void con_write_char(int ch)
{
	int lastrow = row;
	switch (ch) {
		case '\n':
			con_clear_next_row();
			break;

		case 0x08:
			console[row][col--] = ' ';
			if (col < 0) {
				col = CONSOLE_WIDTH - 1;
				if (--row < 0) {
					row = CONSOLE_HEIGHT - 1;
				}
			}
			break;

		default:
			if (ch < 32) break;
			console[row][col++] = ch;
			if (col == CONSOLE_WIDTH) {
				con_clear_next_row();
			}
			break;
	}
	if (lastrow != row) {
		// Update the whole screen
		con_update_screen();
	} else {
		// Update only the current row
		con_update_row();
	}
}

void con_write_string(const char* s)
{
	if (s == NULL) return;
	while (*s) con_write(*s++);
}