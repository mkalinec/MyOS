#include "display.h"

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

void draw_pixel(struct limine_framebuffer *framebuffer,
                              uint32_t x, uint32_t y,
                              uint32_t color) {
    if (!framebuffer) return;
    if (x >= framebuffer->width) return;
    if (y >= framebuffer->height) return;

    volatile uint32_t *pixels = framebuffer->address;
    uint32_t pitch_in_pixels = framebuffer->pitch / sizeof(uint32_t);

    pixels[y * pitch_in_pixels + x] = color;
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


void draw_circle(struct limine_framebuffer *framebuffer,
                 int center_x, int center_y,
                 int radius,
                 uint32_t color) {

    int x = 0;
    int y = radius;
    int decision = 3 - 2 * radius;

    while (y >= x) {
        draw_pixel(framebuffer, center_x + x, center_y + y, color);
        draw_pixel(framebuffer, center_x - x, center_y + y, color);
        draw_pixel(framebuffer, center_x + x, center_y - y, color);
        draw_pixel(framebuffer, center_x - x, center_y - y, color);
        draw_pixel(framebuffer, center_x + y, center_y + x, color);
        draw_pixel(framebuffer, center_x - y, center_y + x, color);
        draw_pixel(framebuffer, center_x + y, center_y - x, color);
        draw_pixel(framebuffer, center_x - y, center_y - x, color);

        if (decision < 0) {
            decision += 4 * x + 6;
        } else {
            decision += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}


void draw_filled_circle(struct limine_framebuffer *framebuffer,
                        int center_x, int center_y,
                        int radius,
                        uint32_t color) {

    int x = 0;
    int y = radius;
    int decision = 3 - 2 * radius;

    while (y >= x) {
        for (int i = center_x - x; i <= center_x + x; i++) {
            draw_pixel(framebuffer, i, center_y + y, color);
            draw_pixel(framebuffer, i, center_y - y, color);
        }
        for (int i = center_x - y; i <= center_x + y; i++) {
            draw_pixel(framebuffer, i, center_y + x, color);
            draw_pixel(framebuffer, i, center_y - x, color);
        }

        if (decision < 0) {
            decision += 4 * x + 6;
        } else {
            decision += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void draw_filled_rectangle(struct limine_framebuffer *fb,
                           int start_x, int start_y,
                           int end_x,   int end_y,
                           int color)
{
    volatile uint32_t *pixels = fb->address;
    int pixels_per_row = fb->pitch / 4;

    // clipping
    if (start_x > end_x) { int tmp = start_x; start_x = end_x; end_x = tmp; }
    if (start_y > end_y) { int tmp = start_y; start_y = end_y; end_y = tmp; }

    for (int y = start_y; y <= end_y; y++) {
        for (int x = start_x; x <= end_x; x++) {
            pixels[y * pixels_per_row + x] = color;
        }
    }
}

void draw_filled_triangle(struct limine_framebuffer *fb,
                          int x1, int y1,
                          int x2, int y2,
                          int x3, int y3,
                          int color)
{
    if (y1 > y2) { int tmp; tmp=y1;y1=y2;y2=tmp; tmp=x1;x1=x2;x2=tmp; }
    if (y1 > y3) { int tmp; tmp=y1;y1=y3;y3=tmp; tmp=x1;x1=x3;x3=tmp; }
    if (y2 > y3) { int tmp; tmp=y2;y2=y3;y3=tmp; tmp=x2;x2=x3;x3=tmp; }

    int total_height = y3 - y1;

    for (int i = 0; i <= total_height; i++) {
        int second_half = i > (y2 - y1) || (y2 - y1 == 0);
        int segment_height = second_half ? y3 - y2 : y2 - y1;
        if (segment_height == 0) continue;

        int ax = x1 + (x3 - x1) * i / total_height;
        int bx;
        if (second_half) {
            bx = x2 + (x3 - x2) * (i - (y2 - y1)) / segment_height;
        } else {
            bx = x1 + (x2 - x1) * i / segment_height;
        }

        if (ax > bx) { int tmp = ax; ax = bx; bx = tmp; }

        draw_line(fb, ax, y1 + i, bx, y1 + i, color);
    }
}

void draw_char(struct limine_framebuffer *fb,
               char c, int x, int y,
               uint32_t color)
{
    if (c < 0 || c > 127) return;

    for (int row = 0; row < 8; row++) {
        uint8_t bitmap = font8x8_basic[(int)c][row];
        for (int col = 0; col < 8; col++) {
            if (bitmap & (1 << (7 - col))) {
                draw_pixel(fb, x + col, y + row, color);
            }
        }
    }
}

void draw_text(struct limine_framebuffer *fb,
               const char *text,
               int x, int y,
               uint32_t color)
{
    int cursor_x = x;
    int cursor_y = y;

    while (*text) {
        if (*text == '\n') {
            cursor_x = x;
            cursor_y += 8;
        } else {
            draw_char(fb, *text, cursor_x, cursor_y, color);
            cursor_x += 8;
        }
        text++;
    }
}

void clear_screen(struct limine_framebuffer* fb,
                  uint32_t color) 
{
    uint32_t* pixels = (uint32_t*)fb->address;
    size_t pitch_pixels = fb->pitch / 4;

    for (size_t y = 0; y < fb->height; y++) {
        uint32_t* row = pixels + y * pitch_pixels;
        for (size_t x = 0; x < fb->width; x++) {
            row[x] = color;
        }
    }
}


void draw_char_8x16(struct limine_framebuffer *fb,
                    unsigned char c, int x, int y,
                    uint32_t fg, int draw_bg, uint32_t bg)
{
    if (!fb) return;

    for (int row = 0; row < 16; row++) {
        uint8_t bits = font8x16[c][row];

        for (int col = 0; col < 8; col++) {
            int on = bits & (1u << (7 - col));

            if (on) {
                draw_pixel(fb, (uint32_t)(x + col), (uint32_t)(y + row), fg);
            } else if (draw_bg) {
                draw_pixel(fb, (uint32_t)(x + col), (uint32_t)(y + row), bg);
            }
        }
    }
}

void draw_char_16x32(struct limine_framebuffer *fb,
                     unsigned char c, int x, int y,
                     uint32_t fg, int draw_bg, uint32_t bg)
{
    if (!fb) return;

    for (int row = 0; row < 32; row++) {
        uint8_t hi = font16x32[c][row][0];
        uint8_t lo = font16x32[c][row][1];

        for (int col = 0; col < 16; col++) {
            int on;
            if (col < 8) on = hi & (1u << (7 - col));
            else         on = lo & (1u << (15 - col));

            if (on) draw_pixel(fb, (uint32_t)(x + col), (uint32_t)(y + row), fg);
            else if (draw_bg) draw_pixel(fb, (uint32_t)(x + col), (uint32_t)(y + row), bg);
        }
    }
}

void draw_char_24x32(struct limine_framebuffer *fb,
                     unsigned char c, int x, int y,
                     uint32_t fg, int draw_bg, uint32_t bg)
{
    for (int row = 0; row < 32; row++) {
        uint32_t bits = font24x32[c][row];
        for (int col = 0; col < 24; col++) {
            int on = bits & (1u << (23 - col));
            if (on) draw_pixel(fb, (uint32_t)(x + col), (uint32_t)(y + row), fg);
            else if (draw_bg) draw_pixel(fb, (uint32_t)(x + col), (uint32_t)(y + row), bg);
        }
    }
}


void draw_char_custom(struct limine_framebuffer *fb,
                      unsigned char c, int x, int y,
                      uint32_t fg, int draw_bg, uint32_t bg,
                      uint32_t x_size, uint32_t y_size,
                      const uint32_t (*font)[y_size])
{
    for (int row = 0; row < y_size; row++) {
        uint32_t bits = font[c][row];
        for (int col = 0; col < x_size; col++) {
            int on = bits & (1u << (x_size - 1 - col));
            if (on) draw_pixel(fb, (uint32_t)(x + col), (uint32_t)(y + row), fg);
            else if (draw_bg) draw_pixel(fb, (uint32_t)(x + col), (uint32_t)(y + row), bg);
        }
    }
}