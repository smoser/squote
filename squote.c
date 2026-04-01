#include <stdio.h>
#include <string.h>

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
           c == '@' || c == '%' || c == '+' || c == '=' || c == ':' ||
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

static int quote_argv(int argc, char **argv, char *out, size_t outsz) {
    Buf b = { out, 0, outsz };
    out[0] = '\0';
    for (int i = 0; i < argc; i++) {
        if (i > 0 && bputc(&b, ' ') < 0) return -1;
        if (quote_arg(argv[i], &b) < 0) return -1;
    }
    return 0;
}

#ifndef TEST
int main(int argc, char **argv) {
    char buf[BUFMAX];
    if (argc < 2) return 0;
    if (quote_argv(argc - 1, argv + 1, buf, sizeof(buf)) < 0) {
        fprintf(stderr, "squote: arguments too long\n");
        return 1;
    }
    puts(buf);
    return 0;
}
#endif

#ifdef TEST
#include <stdarg.h>

static int passed, failed;

static void check(const char *label, const char *got, const char *want) {
    if (strcmp(got, want) == 0) {
        passed++;
    } else {
        printf("FAIL: %s\n  got:  %s\n  want: %s\n", label, got, want);
        failed++;
    }
}

static void test_arg(const char *input, const char *want) {
    char buf[BUFMAX];
    Buf b = { buf, 0, sizeof(buf) };
    buf[0] = '\0';
    quote_arg(input, &b);
    check(input, buf, want);
}

static void test_argv(const char *want, int argc, ...) {
    char *argv[64];
    va_list ap;
    va_start(ap, argc);
    for (int i = 0; i < argc; i++) argv[i] = va_arg(ap, char *);
    va_end(ap);
    char buf[BUFMAX];
    quote_argv(argc, argv, buf, sizeof(buf));
    check(want, buf, want);
}

int main(void) {
    /* bare */
    test_arg("echo",           "echo");
    test_arg("foo@bar.com",    "foo@bar.com");
    test_arg("/usr/bin/env",   "/usr/bin/env");
    test_arg("key=value",      "key=value");
    test_arg("some-flag_name", "some-flag_name");

    /* single-quote wrapping */
    test_arg("hello world",    "'hello world'");
    test_arg("say \"hi\"",     "'say \"hi\"'");
    test_arg("$HOME",          "'$HOME'");
    test_arg("`date`",         "'`date`'");
    test_arg("C:\\path",       "'C:\\path'");
    test_arg("hello!",         "'hello!'");
    test_arg("$var and spaces","'$var and spaces'");

    /* double-quote wrapping: has ' but no shell-special */
    test_arg("howdy' partner",     "\"howdy' partner\"");
    test_arg("it's a \"test\"",    "\"it's a \\\"test\\\"\"");
    test_arg("it's",               "\"it's\"");
    test_arg("it's rockin'",       "\"it's rockin'\"");

    /* double-quote wrapping: has ' and shell-special */
    test_arg("it's $HOME",         "\"it's \\$HOME\"");
    test_arg("it's `date`",        "\"it's \\`date\\`\"");
    test_arg("it's C:\\path",      "\"it's C:\\\\path\"");
    test_arg("it's great!",        "\"it's great\\!\"");

    /* empty */
    test_arg("", "''");

    /* multi-arg */
    test_argv("echo hello world",      3, "echo", "hello", "world");
    test_argv("echo 'hello world'",    2, "echo", "hello world");
    test_argv("echo \"howdy' partner\"", 2, "echo", "howdy' partner");

    printf("%d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
#endif
