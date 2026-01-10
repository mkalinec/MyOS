#include "../include/cmd.h"
#include "../lib/string.h"

static int is_space(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

int cmd_tokenize(char *line, cmdline_t *out) {
    if (!line || !out) return -1;

    out->cmd = NULL;
    out->argc = 0;

    char *p = line;

    while (*p && is_space(*p)) p++;

    while (*p) {
        if (out->argc >= (int)(sizeof(out->argv) / sizeof(out->argv[0]))) break;

        out->argv[out->argc++] = p;

        while (*p && !is_space(*p)) p++;

        if (*p) {
            *p = '\0';
            p++;
            while (*p && is_space(*p)) p++;
        }
    }

    if (out->argc > 0) out->cmd = out->argv[0];
    return out->argc;
}

int cmd_parse_i32(const char *s, int *out) {
    if (!s || !*s || !out) return 0;

    int sign = 1;
    if (*s == '-') { sign = -1; s++; }
    if (*s == '\0') return 0;

    int val = 0;
    while (*s) {
        char c = *s++;
        if (c < '0' || c > '9') return 0;
        val = val * 10 + (c - '0');
    }
    *out = val * sign;
    return 1;
}


int cmd_parse_color(const char *s, uint32_t *out) {
    if (!s || !*s || !out) return 0;

    if (streq(s, "BLACK"))   { *out = 0x000000; return 1; }
    if (streq(s, "WHITE"))   { *out = 0xFFFFFF; return 1; }
    if (streq(s, "RED"))     { *out = 0xFF0000; return 1; }
    if (streq(s, "GREEN"))   { *out = 0x00FF00; return 1; }
    if (streq(s, "BLUE"))    { *out = 0x0000FF; return 1; }
    if (streq(s, "YELLOW"))  { *out = 0xFFFF00; return 1; }
    if (streq(s, "CYAN"))    { *out = 0x00FFFF; return 1; }
    if (streq(s, "MAGENTA")) { *out = 0xFF00FF; return 1; }
    if (streq(s, "GRAY"))    { *out = 0x808080; return 1; }

    const char *p = s;
    if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) p += 2;

    uint32_t val = 0;
    int digits = 0;
    while (*p) {
        char c = *p++;
        uint32_t d;
        if (c >= '0' && c <= '9') d = (uint32_t)(c - '0');
        else if (c >= 'a' && c <= 'f') d = (uint32_t)(c - 'a' + 10);
        else if (c >= 'A' && c <= 'F') d = (uint32_t)(c - 'A' + 10);
        else return 0;

        val = (val << 4) | d;
        digits++;
        if (digits > 6) return 0;
    }
    if (digits != 6) return 0;

    *out = val;
    return 1;
}