#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "include/limine.h"
#include "include/keyboard.h"
#include "include/memory.h"
#include "include/display.h"
#include "include/colors.h"
#include "include/console.h"
#include "lib/string.h"
#include "include/cmd.h"


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

static void handle_command(console_t *con, char *line);


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

    console_t con;
    console_init(&con, framebuffer, 0xFFFFFF, 0x000000);
    console_cursor_set_blink(&con, 20000000000000000); // doladíš podľa rýchlosti slučky

    char line[128];


    while (1) {
        console_readline(&con, "> ", line, sizeof(line));
        handle_command(&con, line);
    }
}


static void handle_command(console_t *con, char *line) {
    cmdline_t cl;
    int argc = cmd_tokenize(line, &cl);
    if (argc <= 0) return;

    if (streq(cl.cmd, "cls")) {
        console_clear(con);
        return;
    }

    if (streq(cl.cmd, "rectangle")) {
        // square x1 y1 x2 y2 COLOR
        if (argc != 6) {
            console_write(con, "Usage: square x1 y1 x2 y2 COLOR\n");
            return;
        }

        int x1, y1, x2, y2;
        uint32_t col;

        if (!cmd_parse_i32(cl.argv[1], &x1) ||
            !cmd_parse_i32(cl.argv[2], &y1) ||
            !cmd_parse_i32(cl.argv[3], &x2) ||
            !cmd_parse_i32(cl.argv[4], &y2) ||
            !cmd_parse_color(cl.argv[5], &col)) 
        {
            console_write(con, "Invalid args. Example: square 20 20 40 40 RED\n");
            return;
        }

        console_cursor_render(con, 0);

        draw_rectangle(con->fb, x1, y1, x2, y2, (int)col);
        return;
    }

    if (streq(cl.cmd, "fillrect")) {
        // fillrect x1 y1 x2 y2 COLOR
        if (argc != 6) {
            console_write(con, "Usage: fillrect x1 y1 x2 y2 COLOR\n");
            return;
        }

        int x1, y1, x2, y2;
        uint32_t col;

        if (!cmd_parse_i32(cl.argv[1], &x1) ||
            !cmd_parse_i32(cl.argv[2], &y1) ||
            !cmd_parse_i32(cl.argv[3], &x2) ||
            !cmd_parse_i32(cl.argv[4], &y2) ||
            !cmd_parse_color(cl.argv[5], &col)) {
            console_write(con, "Invalid args.\n");
            return;
        }

        console_cursor_render(con, 0);
        draw_filled_rectangle(con->fb, x1, y1, x2, y2, (int)col);
        return;
    }

    console_write(con, "Unknown command: ");
    console_write(con, cl.cmd);
    console_putc(con, '\n');
}