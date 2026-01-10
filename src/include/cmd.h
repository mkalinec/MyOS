#pragma once
#include <stddef.h>
#include <stdint.h>

typedef struct {
    const char *cmd;
    int argc;
    const char *argv[16];
} cmdline_t;

int cmd_tokenize(char *line, cmdline_t *out);
int cmd_parse_i32(const char *s, int *out);
int cmd_parse_color(const char *s, uint32_t *out);
int streq(const char *a, const char *b);