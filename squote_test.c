#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "squote.h"

#define MAXSTR  4096
#define MAXARGS 64
#define BUFMAX  65536

/* --- minimal JSON parser --- */

typedef struct { const char *s; size_t pos; } P;

static void ws(P *p) {
    while (p->s[p->pos] && isspace((unsigned char)p->s[p->pos]))
        p->pos++;
}

static int eat(P *p, char c) {
    ws(p);
    if (p->s[p->pos] != c) return -1;
    p->pos++;
    return 0;
}

static int jstr(P *p, char *out, size_t outsz) {
    ws(p);
    if (p->s[p->pos] != '"') return -1;
    p->pos++;
    size_t i = 0;
    while (p->s[p->pos] && p->s[p->pos] != '"') {
        char c = p->s[p->pos++];
        if (c == '\\' && p->s[p->pos]) {
            c = p->s[p->pos++];
            if      (c == 'n') c = '\n';
            else if (c == 't') c = '\t';
            else if (c == 'r') c = '\r';
            /* \", \\, and any other \X: use X as-is */
        }
        if (i + 1 >= outsz) return -1;
        out[i++] = c;
    }
    if (p->s[p->pos] != '"') return -1;
    p->pos++;
    out[i] = '\0';
    return 0;
}

static int jstr_array(P *p, char out[][MAXSTR], int *n) {
    if (eat(p, '[') < 0) return -1;
    *n = 0;
    ws(p);
    if (p->s[p->pos] == ']') { p->pos++; return 0; }
    for (;;) {
        if (*n >= MAXARGS) return -1;
        if (jstr(p, out[(*n)++], MAXSTR) < 0) return -1;
        ws(p);
        if (p->s[p->pos] == ',') { p->pos++; continue; }
        break;
    }
    return eat(p, ']');
}

typedef struct {
    char args[MAXARGS][MAXSTR];
    int  argc;
    char want[MAXSTR];
} Case;

static int parse_case(P *p, Case *c) {
    if (eat(p, '{') < 0) return -1;
    for (int i = 0; i < 2; i++) {
        char key[32];
        if (jstr(p, key, sizeof(key)) < 0) return -1;
        if (eat(p, ':') < 0) return -1;
        if (strcmp(key, "input") == 0) {
            if (jstr_array(p, c->args, &c->argc) < 0) return -1;
        } else if (strcmp(key, "want") == 0) {
            if (jstr(p, c->want, sizeof(c->want)) < 0) return -1;
        } else {
            return -1;
        }
        ws(p);
        if (p->s[p->pos] == ',') p->pos++;
    }
    return eat(p, '}');
}

/* --- test runner --- */

int main(int argc, char **argv) {
    const char *fname = argc > 1 ? argv[1] : "tests.json";

    FILE *f = fopen(fname, "r");
    if (!f) { fprintf(stderr, "cannot open %s\n", fname); return 1; }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    rewind(f);
    char *json = malloc(sz + 1);
    if (fread(json, 1, sz, f) != (size_t)sz) {
        fprintf(stderr, "error reading %s\n", fname);
        free(json);
        return 1;
    }
    json[sz] = '\0';
    fclose(f);

    P p = { json, 0 };
    int passed = 0, failed = 0;

    if (eat(&p, '[') < 0) { fprintf(stderr, "expected '['\n"); return 1; }
    ws(&p);
    while (p.s[p.pos] != ']' && p.s[p.pos]) {
        Case c = {{{""}}, 0, ""};
        if (parse_case(&p, &c) < 0) {
            fprintf(stderr, "parse error near: %.20s\n", p.s + p.pos);
            free(json);
            return 1;
        }

        char *args[MAXARGS];
        for (int i = 0; i < c.argc; i++) args[i] = c.args[i];

        char got[BUFMAX];
        quote_argv(c.argc, args, got, sizeof(got));

        if (strcmp(got, c.want) == 0) {
            passed++;
        } else {
            printf("FAIL: input[0]=\"%s\"\n  got:  %s\n  want: %s\n",
                   c.args[0], got, c.want);
            failed++;
        }

        ws(&p);
        if (p.s[p.pos] == ',') p.pos++;
        ws(&p);
    }

    free(json);
    printf("%d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
