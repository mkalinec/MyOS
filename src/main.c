#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "include/limine.h"
#include "include/keyboard.h"
#include "include/memory.h"
#include "include/display.h"
#include "include/colors.h"


__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(4);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0
};

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;


// Halt and catch fire function.
static void hcf(void) {
    for (;;) {
        asm ("hlt");
    }
}


void kmain(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
        hcf();
    }

    if (check_for_framebuffer(framebuffer_request) == false){
        hcf();
    }



    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];


    char c;

    draw_text(framebuffer, "Hello, World!\nFrom OS that doest exist", 10, 10, COLOR_WHITE);

    while (1) {
        c = kbd_get_char();
        if (c == 'a') {
            draw_line(framebuffer, 0, 0, 300, 300, COLOR_WHITE);
        }
        if (c == 'b') {
            draw_line(framebuffer, 0, 0, 300, 300, COLOR_BLACK);
        }
        if (c == 'c'){
            draw_rectangle(framebuffer, 10, 10, 200, 500, COLOR_MAGENTA);
        }
        if (c == 'd'){
            draw_line(framebuffer, 20, 30, 800, 100, COLOR_ORANGE);
        }
        if (c == 'e'){
            draw_triangle(framebuffer, 100, 100, 300, 50, 600, 400, COLOR_LAWN_GREEN);
        }
        if (c == 'f'){
            draw_circle(framebuffer, 100, 100, 20, COLOR_PINK);
        }
        if (c == 'g'){
            draw_filled_circle(framebuffer, 500, 500, 100, COLOR_YELLOW);
        }
        if (c == 'h'){
            draw_filled_rectangle(framebuffer, 352, 123, 100, 235, COLOR_LIGHT_SKY_BLUE);
        }
        if (c == 'i'){
            draw_filled_triangle(framebuffer, 200, 50, 600, 600, 800, 120, COLOR_BROWN);
        }
        if (c == ' '){
            clear_screen(framebuffer, COLOR_BLACK);
        }


    }

    // We're done, just hang...
    hcf();
}
