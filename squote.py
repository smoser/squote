#!/usr/bin/env python3
import sys
import unittest

_SAFE_BARE = frozenset(
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789"
    "@%+=:,./-_"
)

_DOUBLE_QUOTE_SPECIAL = frozenset('"$`\\!')


def quote_arg(s):
    if not s:
        return "''"
    if all(c in _SAFE_BARE for c in s):
        return s
    if "'" not in s:
        return "'" + s + "'"
    # has single quote: double quotes with backslash escaping
    escaped = "".join(("\\" + c if c in _DOUBLE_QUOTE_SPECIAL else c) for c in s)
    return '"' + escaped + '"'


def quote_args(args):
    return " ".join(quote_arg(a) for a in args)


class TestQuoteArg(unittest.TestCase):

    # bare
    def test_bare_simple(self):        self.assertEqual(quote_arg("echo"), "echo")
    def test_bare_path(self):          self.assertEqual(quote_arg("/usr/bin/env"), "/usr/bin/env")
    def test_bare_key_value(self):     self.assertEqual(quote_arg("key=value"), "key=value")
    def test_bare_email(self):         self.assertEqual(quote_arg("foo@bar.com"), "foo@bar.com")
    def test_bare_flag(self):          self.assertEqual(quote_arg("some-flag_name"), "some-flag_name")

    # single-quote wrapping
    def test_sq_space(self):           self.assertEqual(quote_arg("hello world"), "'hello world'")
    def test_sq_double_quote(self):    self.assertEqual(quote_arg('say "hi"'), '\'say "hi"\'')
    def test_sq_dollar(self):          self.assertEqual(quote_arg("$HOME"), "'$HOME'")
    def test_sq_backtick(self):        self.assertEqual(quote_arg("`date`"), "'`date`'")
    def test_sq_backslash(self):       self.assertEqual(quote_arg("C:\\path"), "'C:\\path'")
    def test_sq_exclamation(self):     self.assertEqual(quote_arg("hello!"), "'hello!'")
    def test_sq_mixed_special(self):   self.assertEqual(quote_arg("$var and spaces"), "'$var and spaces'")

    # double-quote wrapping: has ' but no shell-special
    def test_dq_single_quote(self):    self.assertEqual(quote_arg("howdy' partner"), '"howdy\' partner"')
    def test_dq_both_quotes(self):     self.assertEqual(quote_arg("it's a \"test\""), '"it\'s a \\"test\\""')
    def test_dq_apostrophe(self):      self.assertEqual(quote_arg("it's"), '"it\'s"')
    def test_dq_multi_apostrophe(self):self.assertEqual(quote_arg("it's rockin'"), '"it\'s rockin\'"')

    # double-quote wrapping: has ' and shell-special
    def test_dq_sq_dollar(self):       self.assertEqual(quote_arg("it's $HOME"), '"it\'s \\$HOME"')
    def test_dq_sq_backtick(self):     self.assertEqual(quote_arg("it's `date`"), '"it\'s \\`date\\`"')
    def test_dq_sq_backslash(self):    self.assertEqual(quote_arg("it's C:\\path"), '"it\'s C:\\\\path"')
    def test_dq_sq_exclamation(self):  self.assertEqual(quote_arg("it's great!"), '"it\'s great\\!"')

    # empty
    def test_empty(self):              self.assertEqual(quote_arg(""), "''")

    # multi-arg
    def test_multi_bare(self):
        self.assertEqual(quote_args(["echo", "hello", "world"]), "echo hello world")
    def test_multi_space(self):
        self.assertEqual(quote_args(["echo", "hello world"]), "echo 'hello world'")
    def test_multi_apostrophe(self):
        self.assertEqual(quote_args(["echo", "howdy' partner"]), "echo \"howdy' partner\"")


if __name__ == "__main__":
    args = sys.argv[1:]
    if args == ["--test"]:
        sys.argv = sys.argv[:1]
        unittest.main()
    elif args:
        print(quote_args(args))
