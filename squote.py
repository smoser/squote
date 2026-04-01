#!/usr/bin/env python3
import os
import sys

_SAFE_BARE = frozenset(
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789"
    "@+:,./-_"
)

_DOUBLE_QUOTE_SPECIAL = frozenset('"$`\\!')


def quote_arg(s):
    if not s:
        return "''"
    if all(c in _SAFE_BARE for c in s):
        return s
    if "'" not in s:
        return "'" + s + "'"
    escaped = "".join(("\\" + c if c in _DOUBLE_QUOTE_SPECIAL else c) for c in s)
    return '"' + escaped + '"'


def quote_args(args):
    return " ".join(quote_arg(a) for a in args)


VERSION = "0.1.0"

if __name__ == "__main__":
    args = sys.argv[1:]
    if os.environ.get("SQUOTE_PASSTHROUGH") == "true":
        if args:
            print(quote_args(args))
    elif args and args[0] == "--version":
        print(f"squote {VERSION}")
    elif args and args[0] == "--help":
        print("Usage: squote [--help] [--version] [--] [ARG...]\n"
              "Quote ARGs for safe copy/paste into a shell.")
    else:
        if args and args[0] == "--":
            args = args[1:]
        if args:
            print(quote_args(args))
