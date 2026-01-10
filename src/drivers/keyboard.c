#include "../include/limine.h"
#include "../include/keyboard.h"

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

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}