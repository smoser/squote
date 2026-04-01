fn is_safe_bare(c: char) -> bool {
    c.is_ascii_alphanumeric()
        || matches!(c, '@' | '+' | ':' | ',' | '.' | '/' | '-' | '_')
}

fn double_quote_escape(s: &str) -> String {
    let mut out = String::with_capacity(s.len());
    for c in s.chars() {
        if matches!(c, '"' | '$' | '`' | '\\' | '!') {
            out.push('\\');
        }
        out.push(c);
    }
    out
}

fn quote_arg(s: &str) -> String {
    if s.is_empty() {
        return "''".to_string();
    }

    if s.chars().all(is_safe_bare) {
        return s.to_string();
    }

    if !s.contains('\'') {
        // Single quotes are safe for everything including $, `, \, !
        format!("'{s}'")
    } else {
        // Has a single quote: double quotes, backslash-escape special chars
        format!("\"{}\"", double_quote_escape(s))
    }
}

pub fn quote_args(args: &[&str]) -> String {
    args.iter().map(|s| quote_arg(s)).collect::<Vec<_>>().join(" ")
}

fn main() {
    let args: Vec<String> = std::env::args().skip(1).collect();
    let args_ref: Vec<&str> = args.iter().map(String::as_str).collect();
    if !args_ref.is_empty() {
        println!("{}", quote_args(&args_ref));
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    // --- bare (no quoting needed) ---

    #[test]
    fn bare_simple_word() {
        assert_eq!(quote_arg("echo"), "echo");
    }

    #[test]
    fn bare_multiple_args() {
        assert_eq!(quote_args(&["echo", "hello", "world"]), "echo hello world");
    }

    #[test]
    fn bare_with_safe_punctuation() {
        assert_eq!(quote_arg("foo@bar.com"), "foo@bar.com");
        assert_eq!(quote_arg("/usr/bin/env"), "/usr/bin/env");
        assert_eq!(quote_arg("some-flag_name"), "some-flag_name");
    }

    #[test]
    fn single_quote_for_equals() {
        assert_eq!(quote_arg("key=value"), "'key=value'");
    }

    // --- single-quote wrapping ---

    #[test]
    fn single_quote_for_space() {
        assert_eq!(quote_arg("hello world"), "'hello world'");
    }

    #[test]
    fn single_quote_for_double_quote() {
        assert_eq!(quote_arg(r#"say "hi""#), r#"'say "hi"'"#);
    }

    #[test]
    fn single_quote_for_dollar() {
        assert_eq!(quote_arg("$HOME"), "'$HOME'");
    }

    #[test]
    fn single_quote_for_backtick() {
        assert_eq!(quote_arg("`date`"), "'`date`'");
    }

    #[test]
    fn single_quote_for_backslash() {
        assert_eq!(quote_arg("C:\\path"), "'C:\\path'");
    }

    #[test]
    fn single_quote_for_exclamation() {
        assert_eq!(quote_arg("hello!"), "'hello!'");
    }

    #[test]
    fn single_quote_mixed_specials() {
        assert_eq!(quote_arg("$var and spaces"), "'$var and spaces'");
    }

    // --- double-quote wrapping (has ' but no shell-special) ---

    #[test]
    fn double_quote_for_single_quote() {
        assert_eq!(quote_arg("howdy' partner"), "\"howdy' partner\"");
    }

    #[test]
    fn double_quote_single_and_double_quote() {
        assert_eq!(quote_arg("it's a \"test\""), "\"it's a \\\"test\\\"\"");
    }

    #[test]
    fn double_quote_only_single_quote() {
        assert_eq!(quote_arg("it's"), "\"it's\"");
    }

    #[test]
    fn double_quote_multiple_single_quotes() {
        assert_eq!(quote_arg("it's rockin'"), "\"it's rockin'\"");
    }

    // --- double-quote with escaping (has ' and shell-special) ---

    #[test]
    fn double_quote_single_quote_and_dollar() {
        assert_eq!(quote_arg("it's $HOME"), "\"it's \\$HOME\"");
    }

    #[test]
    fn double_quote_single_quote_and_backtick() {
        assert_eq!(quote_arg("it's `date`"), "\"it's \\`date\\`\"");
    }

    #[test]
    fn double_quote_single_quote_and_backslash() {
        assert_eq!(quote_arg("it's C:\\path"), "\"it's C:\\\\path\"");
    }

    #[test]
    fn double_quote_single_quote_and_exclamation() {
        assert_eq!(quote_arg("it's great!"), "\"it's great\\!\"");
    }

    // --- empty string ---

    #[test]
    fn empty_string() {
        assert_eq!(quote_arg(""), "''");
    }

    // --- multi-arg joining ---

    #[test]
    fn multi_arg_mixed() {
        assert_eq!(
            quote_args(&["echo", "hello world"]),
            "echo 'hello world'"
        );
    }

    #[test]
    fn multi_arg_with_single_quote() {
        assert_eq!(
            quote_args(&["echo", "howdy' partner"]),
            "echo \"howdy' partner\""
        );
    }
}
