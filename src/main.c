#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>

// Set the base revision to 4, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(4);

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0
};

// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .c file, as seen fit.

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

// GCC and Clang reserve the right to generate calls to the following
// 4 functions even if they are not directly called.
// Implement them as the C specification mandates.
// DO NOT remove or rename these functions, or stuff will eventually break!
// They CAN be moved to a different .c file.

void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
    uint8_t *restrict pdest = (uint8_t *restrict)dest;
    const uint8_t *restrict psrc = (const uint8_t *restrict)src;

    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    if (src > dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

// Halt and catch fire function.
static void hcf(void) {
    for (;;) {
        asm ("hlt");
    }
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

#define KBD_DATA 0x60
#define KBD_STATUS 0x64

uint8_t kbd_read_scancode() {
    // čakáme, kým bude dostupný dátový bajt
    while (!(inb(KBD_STATUS) & 0x01));
    return inb(KBD_DATA);
}

char scancode_to_ascii(uint8_t scancode, int shift) {
    switch (scancode) {
        case 0x02: return shift ? '!' : '1';
        case 0x03: return shift ? '@' : '2';
        case 0x04: return shift ? '#' : '3';
        case 0x05: return shift ? '$' : '4';
        case 0x06: return shift ? '%' : '5';
        case 0x07: return shift ? '^' : '6';
        case 0x08: return shift ? '&' : '7';
        case 0x09: return shift ? '*' : '8';
        case 0x0A: return shift ? '(' : '9';
        case 0x0B: return shift ? ')' : '0';

        case 0x1E: return shift ? 'A' : 'a';
        case 0x30: return shift ? 'B' : 'b';
        case 0x2E: return shift ? 'C' : 'c';
        case 0x20: return shift ? 'D' : 'd';
        case 0x12: return shift ? 'E' : 'e';
        case 0x21: return shift ? 'F' : 'f';
        case 0x22: return shift ? 'G' : 'g';
        case 0x23: return shift ? 'H' : 'h';
        case 0x17: return shift ? 'I' : 'i';
        case 0x24: return shift ? 'J' : 'j';
        case 0x25: return shift ? 'K' : 'k';
        case 0x26: return shift ? 'L' : 'l';
        case 0x32: return shift ? 'M' : 'm';
        case 0x31: return shift ? 'N' : 'n';
        case 0x18: return shift ? 'O' : 'o';
        case 0x19: return shift ? 'P' : 'p';
        case 0x10: return shift ? 'Q' : 'q';
        case 0x13: return shift ? 'R' : 'r';
        case 0x1F: return shift ? 'S' : 's';
        case 0x14: return shift ? 'T' : 't';
        case 0x16: return shift ? 'U' : 'u';
        case 0x2F: return shift ? 'V' : 'v';
        case 0x11: return shift ? 'W' : 'w';
        case 0x2D: return shift ? 'X' : 'x';
        case 0x15: return shift ? 'Y' : 'y';
        case 0x2C: return shift ? 'Z' : 'z';

        case 0x39: return ' ';   // Space
        case 0x0E: return '\b';  // Backspace
        case 0x1C: return '\n';  // Enter
        default: return 0;       // Unsupported
    }
}

int shift_pressed = 0;

void update_shift(uint8_t scancode) {
    // Left Shift: 0x2A, Right Shift: 0x36
    if (scancode == 0x2A || scancode == 0x36) shift_pressed = 1;
    else if (scancode == 0xAA || scancode == 0xB6) shift_pressed = 0;
}

char kbd_get_char() {
    while (1) {
        uint8_t sc = kbd_read_scancode();

        // Update shift stav
        update_shift(sc);

        // Ignorujeme uvolnenie kľúčov, okrem shift
        if (sc & 0x80) continue;

        char c = scancode_to_ascii(sc, shift_pressed);
        if (c) return c;
    }
}


// The following will be our kernel's entry point.
// If renaming kmain() to something else, make sure to change the
// linker script accordingly.
void kmain(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
        hcf();
    }

    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL
     || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];


    char c;

    while (1) {
        c = kbd_get_char();
        if (c == 'a') {
            // Note: we assume the framebuffer model is RGB with 32-bit pixels.
            for (size_t i = 0; i < 300; i++) {
                volatile uint32_t *fb_ptr = framebuffer->address;
                fb_ptr[i * (framebuffer->pitch / 4) + i] = 0xffffff;
            }  
        }
            if (c == 'b') {
            // Note: we assume the framebuffer model is RGB with 32-bit pixels.
            for (size_t i = 0; i < 300; i++) {
                volatile uint32_t *fb_ptr = framebuffer->address;
                fb_ptr[i * (framebuffer->pitch / 4) + i] = 0x0;
            }  
        }


    }

    // We're done, just hang...
    hcf();
}
