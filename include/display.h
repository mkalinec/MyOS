#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "limine.h"
#include "font.h"


bool check_for_framebuffer();

void draw_pixel(struct limine_framebuffer *framebuffer,
                              uint32_t x, uint32_t y,
                              uint32_t color);


void draw_rectangle(struct limine_framebuffer *framebuffer,
                    int start_x, int start_y,
                    int end_x, int end_y,
                    int color);


void draw_line(struct limine_framebuffer *framebuffer,
               int start_x, int start_y,
               int end_x, int end_y,
               int color);


void draw_triangle(struct limine_framebuffer *framebuffer,
                   int x1, int y1,
                   int x2, int y2,
                   int x3, int y3,
                   int color);


void draw_circle(struct limine_framebuffer *framebuffer,
                 int center_x, int center_y,
                 int radius,
                 uint32_t color);


void draw_filled_circle(struct limine_framebuffer *framebuffer,
                        int center_x, int center_y,
                        int radius,
                        uint32_t color);

void draw_filled_rectangle(struct limine_framebuffer *fb,
                           int start_x, int start_y,
                           int end_x,   int end_y,
                           int color);

void draw_filled_triangle(struct limine_framebuffer *fb,
                          int x1, int y1,
                          int x2, int y2,
                          int x3, int y3,
                          int color);


void draw_char(struct limine_framebuffer *fb,
               char c, int x, int y,
               uint32_t color);

               
void draw_text(struct limine_framebuffer *fb,
               const char *text,
               int x, int y,
               uint32_t color);

void clear_screen(struct limine_framebuffer* fb,
                  uint32_t color);

void draw_char_scaled(struct limine_framebuffer *fb,
                      char c, int x, int y,
                      uint32_t color, int scale);

void draw_text_scaled(struct limine_framebuffer *fb,
                      const char *text,
                      int x, int y,
                      uint32_t color, int scale);

void draw_char_8x16(struct limine_framebuffer *fb,
                    unsigned char c, int x, int y,
                    uint32_t fg, int draw_bg, uint32_t bg);

void draw_text_8x16(struct limine_framebuffer *fb,
                    const char *text, int x, int y,
                    uint32_t fg, int draw_bg, uint32_t bg);

void draw_char_16x32(struct limine_framebuffer *fb,
                     unsigned char c, int x, int y,
                     uint32_t fg, int draw_bg, uint32_t bg);

void draw_char_24x32(struct limine_framebuffer *fb,
                     unsigned char c, int x, int y,
                     uint32_t fg, int draw_bg, uint32_t bg);

void draw_char_custom(struct limine_framebuffer *fb,
                      unsigned char c, int x, int y,
                      uint32_t fg, int draw_bg, uint32_t bg,
                      uint32_t x_size, uint32_t y_size,
                      const uint32_t (*font)[y_size]);