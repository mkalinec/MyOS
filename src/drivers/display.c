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

void draw_rectangle(struct limine_framebuffer *framebuffer, int start_x, int start_y, int end_x, int end_y){
    volatile uint32_t *fb_ptr = framebuffer->address;
    
    for (uint32_t x = start_x; x < end_x; x++){
        uint32_t index_top = start_y * (framebuffer->pitch / 4) + x;
        uint32_t index_bottom = end_y * (framebuffer->pitch / 4) + x;
        fb_ptr[index_top] = 0xffffff;
        fb_ptr[index_bottom] = 0xffffff;
    }

    for (uint32_t y = start_y; y < end_y; y++){
        uint32_t index_left =  y * (framebuffer->pitch / 4) + start_x;
        uint32_t index_right = y * (framebuffer->pitch / 4) + end_x;
        fb_ptr[index_left] = 0xffffff;
        fb_ptr[index_right] = 0xffffff;
    }
}

//just a test functions
void draw_line(struct limine_framebuffer *framebuffer){
    for (size_t i = 0; i < 300; i++) {
        volatile uint32_t *fb_ptr = framebuffer->address;
        fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
    }
}
// Note: we assume the framebuffer model is RGB with 32-bit pixels.
void undraw_line(struct limine_framebuffer *framebuffer){
    for (size_t i = 0; i < 300; i++) {
        volatile uint32_t *fb_ptr = framebuffer->address;
        fb_ptr[i * (framebuffer->pitch / 4) + i] = 0x000000;
    }
}