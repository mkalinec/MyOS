#include "../include/display.h"

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

bool check_for_framebuffer(struct limine_framebuffer_request *framebuffer_request){
    // Ensure we got a framebuffer.
    if (framebuffer_request->response == NULL
     || framebuffer_request->response->framebuffer_count < 1) {
        return true;
    }
    return false;
}

void draw_rectangle(struct limine_framebuffer *fb,
                    int start_x, int start_y,
                    int end_x,   int end_y,
                    int color)
{
    volatile uint32_t *pixels = fb->address;
    int pixels_per_row = fb->pitch / 4;

    // top and bottom
    for (int x = start_x; x <= end_x; x++) {
        pixels[start_y * pixels_per_row + x] = color;
        pixels[end_y   * pixels_per_row + x] = color;
    }

    // left and right
    for (int y = start_y; y <= end_y; y++) {
        pixels[y * pixels_per_row + start_x] = color;
        pixels[y * pixels_per_row + end_x]   = color;
    }
}


void draw_line(struct limine_framebuffer *fb,
               int start_x, int start_y,
               int end_x,   int end_y,
               int color)
{
    volatile uint32_t *pixels = fb->address;
    int pixels_per_row = fb->pitch / 4;

    int delta_x = end_x - start_x;
    int delta_y = end_y - start_y;

    int step_x = (delta_x >= 0) ? 1 : -1;
    int step_y = (delta_y >= 0) ? 1 : -1;

    int distance_x = (delta_x >= 0) ? delta_x : -delta_x;
    int distance_y = (delta_y >= 0) ? delta_y : -delta_y;

    int current_x = start_x;
    int current_y = start_y;

    if (distance_x >= distance_y) {
        int error_accumulator = distance_x / 2;

        while (current_x != end_x) {
            pixels[current_y * pixels_per_row + current_x] = color;

            error_accumulator -= distance_y;
            if (error_accumulator < 0) {
                current_y += step_y;
                error_accumulator += distance_x;
            }

            current_x += step_x;
        }
    }

    else {
        int error_accumulator = distance_y / 2;

        while (current_y != end_y) {
            pixels[current_y * pixels_per_row + current_x] = color;

            error_accumulator -= distance_x;
            if (error_accumulator < 0) {
                current_x += step_x;
                error_accumulator += distance_y;
            }

            current_y += step_y;
        }
    }

    pixels[current_y * pixels_per_row + current_x] = color;
}

void draw_triangle(struct limine_framebuffer *fb,
                   int x1, int y1,
                   int x2, int y2,
                   int x3, int y3,
                   int color)
{
    draw_line(fb, x1, y1 ,x2, y2, color);
    draw_line(fb, x1, y1 ,x3, y3, color);
    draw_line(fb, x2, y2 ,x3, y3, color);
}