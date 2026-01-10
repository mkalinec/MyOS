#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "../include/limine.h"


bool check_for_framebuffer();
void draw_rectangle(struct limine_framebuffer *framebuffer, int start_x, int start_y, int end_x, int end_y, int color);
void draw_line(struct limine_framebuffer *framebuffer, int start_x, int start_y, int end_x, int end_y, int color);
void draw_triangle(struct limine_framebuffer *framebuffer, int x1, int y1, int x2, int y2, int x3, int y3, int color);
