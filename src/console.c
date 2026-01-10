#include "include/console.h"
#include "include/display.h"
#include "include/keyboard.h"
#include <stdint.h>
#include <stddef.h>

#define GLYPH_W 24
#define GLYPH_H 32
static const uint32_t (*GLYPH_FONT)[GLYPH_H] = font24x32;

static void memmove_bytes(uint8_t *dst, const uint8_t *src, size_t n) {
    if (dst == src || n == 0) return;
    if (dst < src) {
        for (size_t i = 0; i < n; i++) dst[i] = src[i];
    } else {
        for (size_t i = n; i > 0; i--) dst[i - 1] = src[i - 1];
    }
}

static void console_scroll_up(console_t *c, int lines) {
    if (!c || !c->fb || lines <= 0) return;

    size_t row_bytes = c->fb->pitch;
    size_t scroll_px = (size_t)lines * GLYPH_H;
    size_t height_px = (size_t)c->fb->height;

    if (scroll_px >= height_px) {
        clear_screen(c->fb, c->bg);
        c->cursor_row = 0;
        c->cursor_col = 0;
        return;
    }

    uint8_t *base = (uint8_t*)c->fb->address;
    size_t bytes_to_move = (height_px - scroll_px) * row_bytes;
    memmove_bytes(base, base + scroll_px * row_bytes, bytes_to_move);

    uint32_t *pixels = (uint32_t*)c->fb->address;
    size_t pitch_px = c->fb->pitch / 4;

    for (size_t y = height_px - scroll_px; y < height_px; y++) {
        uint32_t *row = pixels + y * pitch_px;
        for (size_t x = 0; x < c->fb->width; x++) row[x] = c->bg;
    }
}

static void console_newline(console_t *c) {
    c->cursor_col = 0;
    c->cursor_row++;
    if (c->cursor_row >= c->rows) {
        console_scroll_up(c, 1);
        c->cursor_row = c->rows - 1;
    }
}

static inline void invert_cell(console_t *c, int col, int row) {
    if (!c || !c->fb) return;

    int x0 = col * GLYPH_W;
    int y0 = row * GLYPH_H;

    uint32_t *pixels = (uint32_t*)c->fb->address;
    size_t pitch_px = c->fb->pitch / 4;

    for (int y = 0; y < GLYPH_H; y++) {
        uint32_t *p = pixels + (size_t)(y0 + y) * pitch_px + x0;
        for (int x = 0; x < GLYPH_W; x++) {
            p[x] ^= 0x00FFFFFFu; // invert RGB
        }
    }
}


void console_cursor_enable(console_t *c, int enable) {
    if (!c) return;

    if (c->cursor_visible && !enable) {
        if (c->cursor_blink_on) {
            invert_cell(c, c->cursor_col, c->cursor_row);
            c->cursor_blink_on = 0;
        }
    }

    c->cursor_visible = enable ? 1 : 0;
}

void console_cursor_set_blink(console_t *c, uint64_t period_ticks) {
    if (!c) return;
    if (period_ticks == 0) period_ticks = 1;
    c->blink_period = period_ticks;
}

void console_cursor_render(console_t *c, int on) {
    if (!c || !c->cursor_visible) return;

    if (!!on == !!c->cursor_blink_on) return;

    invert_cell(c, c->cursor_col, c->cursor_row);
    c->cursor_blink_on = on ? 1 : 0;
}

void console_tick(console_t *c) {
    if (!c || !c->cursor_visible) return;

    c->blink_counter++;
    if (c->blink_counter >= c->blink_period) {
        c->blink_counter = 0;
        console_cursor_render(c, !c->cursor_blink_on);
    }
}


void console_init(console_t *c, struct limine_framebuffer *fb, uint32_t fg, uint32_t bg) {
    c->fb = fb;
    c->fg = fg;
    c->bg = bg;
    c->cursor_col = 0;
    c->cursor_row = 0;
    c->cols = (int)(fb->width / GLYPH_W);
    c->rows = (int)(fb->height / GLYPH_H);

    c->cursor_visible = 1;
    c->cursor_blink_on = 0;
    c->blink_period = 200000;
    c->blink_counter = 0;

    console_clear(c);
}

void console_clear(console_t *c) {
    if (!c || !c->fb) return;

    console_cursor_render(c, 0);

    clear_screen(c->fb, c->bg);
    c->cursor_col = 0;
    c->cursor_row = 0;
}

void console_putc(console_t *c, char ch) {
    if (!c || !c->fb) return;

    console_cursor_render(c, 0);

    switch (ch) {
        case '\r':
            c->cursor_col = 0;
            return;

        case '\n':
            console_newline(c);
            return;

        case '\b': {
            if (c->cursor_col > 0) {
                c->cursor_col--;
            } else if (c->cursor_row > 0) {
                c->cursor_row--;
                c->cursor_col = c->cols - 1;
            } else {
                return;
            }

            int px = c->cursor_col * GLYPH_W;
            int py = c->cursor_row * GLYPH_H;
            draw_filled_rectangle(c->fb, px, py, px + (GLYPH_W - 1), py + (GLYPH_H - 1), (int)c->bg);
            return;
        }

        default:
            if ((unsigned char)ch < 32) return;
            break;
    }

    if (c->cursor_col >= c->cols) console_newline(c);

    int x = c->cursor_col * GLYPH_W;
    int y = c->cursor_row * GLYPH_H;

    draw_filled_rectangle(c->fb, x, y, x + (GLYPH_W - 1), y + (GLYPH_H - 1), (int)c->bg);
    draw_char_custom(c->fb, (unsigned char)ch, x, y, c->fg, 1, c->bg, GLYPH_W, GLYPH_H, GLYPH_FONT);

    c->cursor_col++;
    if (c->cursor_col >= c->cols) console_newline(c);
}

void console_write(console_t *c, const char *s) {
    if (!s) return;
    while (*s) console_putc(c, *s++);
}

void console_write_n(console_t *c, const char *s, size_t n) {
    if (!s) return;
    for (size_t i = 0; i < n; i++) console_putc(c, s[i]);
}

size_t console_readline(console_t *c, const char *prompt, char *out, size_t out_cap) {
    if (!c || !out || out_cap == 0) return 0;

    if (prompt) console_write(c, prompt);

    size_t len = 0;
    out[0] = '\0';

    console_cursor_enable(c, 1);

    while (1) {
        console_cursor_render(c, 1);

        char ch = kbd_get_char();

        console_cursor_render(c, 0);

        if (ch == '\r') continue;

        if (ch == '\n') {
            console_putc(c, '\n');
            out[len] = '\0';
            return len;
        }

        if (ch == '\b') {
            if (len > 0) {
                len--;
                out[len] = '\0';
                console_putc(c, '\b');
            }
            continue;
        }

        if ((unsigned char)ch >= 32) {
            if (len + 1 < out_cap) {
                out[len++] = ch;
                out[len] = '\0';
                console_putc(c, ch);
            } else {
            }
        }
    }
}