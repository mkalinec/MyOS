#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../include/limine.h"


bool check_for_framebuffer();

static inline void draw_pixel(struct limine_framebuffer *framebuffer,
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
