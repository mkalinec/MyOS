#pragma once
#include <stdint.h>
#include <stddef.h>
#include "limine.h"

typedef struct console {
    struct limine_framebuffer *fb;
    uint32_t fg;
    uint32_t bg;

    int cursor_col;
    int cursor_row;

    int cols;
    int rows;

    int cursor_visible;
    int cursor_blink_on;
    uint64_t blink_period;
    uint64_t blink_counter;
} console_t;

void console_init(console_t *c, struct limine_framebuffer *fb, uint32_t fg, uint32_t bg);
void console_clear(console_t *c);
void console_putc(console_t *c, char ch);
void console_write(console_t *c, const char *s);
void console_write_n(console_t *c, const char *s, size_t n);

void console_cursor_enable(console_t *c, int enable);
void console_cursor_set_blink(console_t *c, uint64_t period_ticks);
void console_cursor_render(console_t *c, int on);
void console_tick(console_t *c);

size_t console_readline(console_t *c, const char *prompt, char *out, size_t out_cap);
