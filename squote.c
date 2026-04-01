#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "squote.h"

#define BUFMAX 65536

typedef struct { char *buf; size_t pos; size_t max; } Buf;

static int bputc(Buf *b, char c) {
    if (b->pos + 1 >= b->max) return -1;
    b->buf[b->pos++] = c;
    b->buf[b->pos] = '\0';
    return 0;
}

static int bputs(Buf *b, const char *s) {
    while (*s)
        if (bputc(b, *s++) < 0) return -1;
    return 0;
}

static int is_safe_bare(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') ||
           c == '@' || c == '+' || c == ':' ||
           c == ',' || c == '.' || c == '/' || c == '-' || c == '_';
}

static int quote_arg(const char *s, Buf *b) {
    const char *p;

    if (*s == '\0')
        return bputs(b, "''");

    for (p = s; *p; p++)
        if (!is_safe_bare(*p)) break;
    if (*p == '\0')
        return bputs(b, s);

    if (!strchr(s, '\'')) {
        if (bputc(b, '\'') < 0) return -1;
        if (bputs(b, s) < 0) return -1;
        return bputc(b, '\'');
    }

    /* has single quote: double quotes, backslash-escape specials */
    if (bputc(b, '"') < 0) return -1;
    for (p = s; *p; p++) {
        if (*p == '"' || *p == '$' || *p == '`' || *p == '\\' || *p == '!')
            if (bputc(b, '\\') < 0) return -1;
        if (bputc(b, *p) < 0) return -1;
    }
    return bputc(b, '"');
}

int quote_argv(int argc, char **argv, char *out, size_t outsz) {
    Buf b = { out, 0, outsz };
    out[0] = '\0';
    for (int i = 0; i < argc; i++) {
        if (i > 0 && bputc(&b, ' ') < 0) return -1;
        if (quote_arg(argv[i], &b) < 0) return -1;
    }
    return 0;
}

#define VERSION "0.1.0"

#ifndef NO_MAIN
int main(int argc, char **argv) {
    char buf[BUFMAX];
    const char *passthrough = getenv("SQUOTE_PASSTHROUGH");
    if (passthrough && strcmp(passthrough, "true") != 0 && strcmp(passthrough, "false") != 0) {
        fprintf(stderr, "squote: SQUOTE_PASSTHROUGH must be 'true' or 'false', got '%s'\n", passthrough);
        return 2;
    }
    if (passthrough && strcmp(passthrough, "true") == 0) {
        if (argc < 2) return 0;
        if (quote_argv(argc - 1, argv + 1, buf, sizeof(buf)) < 0) {
            fprintf(stderr, "squote: arguments too long\n");
            return 1;
        }
        puts(buf);
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "--version") == 0) {
        puts("squote " VERSION);
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "--help") == 0) {
        puts("Usage: squote [--help] [--version] [--] [ARG...]\n"
             "Quote ARGs for safe copy/paste into a shell.");
        return 0;
    }
    if (argc >= 2 && strcmp(argv[1], "--") == 0) {
        argv++; argc--;
    }
    if (argc < 2) return 0;
    if (quote_argv(argc - 1, argv + 1, buf, sizeof(buf)) < 0) {
        fprintf(stderr, "squote: arguments too long\n");
        return 1;
    }
    puts(buf);
    return 0;
}
#endif
