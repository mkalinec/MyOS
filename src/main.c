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
#include "liballoc.h"
#include "cpu/interrupts/idt.h"
#include "cpu/pic/irq.h"
#include "cpu/pic/pic.h"
#include "cpu/gdt/gdt.h"
#include "limine_attribute.h"

// Halt and catch fire function.
static void hcf(void) {
    for (;;) {
        asm ("hlt");
    }
}


static void handle_command(console_t *con, char *line);

uint64_t hhdm_offset;



void kmain(void) {
    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
        hcf();
    }

    if (check_for_framebuffer(framebuffer_request) == false){
        hcf();
    }

    if (!hhdm_request.response) {
        hcf();
    }

    hhdm_offset = hhdm_request.response->offset;



    init_gdt();
    idt_init();


    pic_remap(0x20, 0x28);
    idt_set_descriptor(0x21, keyboard_interrupt_handler_asm, 0x8E);
    pic_clear_mask(1);                         // unmask keyboard
    asm volatile ("sti");                      // enable IRQs LAST



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


static void handle_command(console_t *con, char *line) 
{
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

    if (streq(cl.cmd, "circle")) {
        if (argc != 5) {
            console_write(con, "Usage: circle x y radius COLOR\n");
            return;
        }

        int x, y, rad;
        uint32_t col;

        if (!cmd_parse_i32(cl.argv[1], &x) ||
            !cmd_parse_i32(cl.argv[2], &y) ||
            !cmd_parse_i32(cl.argv[3], &rad) ||
            !cmd_parse_color(cl.argv[4], &col)) {
            console_write(con, "Invalid args.\n");
            return;
        }

        console_cursor_render(con, 0);
        draw_circle(con->fb, x, y, rad, (int)col);
        return;
    }

    if (streq(cl.cmd, "testalloc")) {
        void* a = malloc(100);
        void* b = malloc(4096);
        void* c = malloc(8000);
//
        free(a);
        free(b);
        free(c);

        console_write(con, "liballoc OK\n");

        return;
    }

    if (streq(cl.cmd, "testzero")) {

        int a = 30;
        int b = 0;
        int c = a/b; 
        c++;

      
       for(int i = 0; i < c; i++)
            console_write(con, "dividing by zero successful\n");    // this shoud never see this text

        


        return;
    }

    console_write(con, "Unknown command: ");
    console_write(con, cl.cmd);
    console_putc(con, '\n');
}