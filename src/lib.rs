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

pub fn quote_arg(s: &str) -> String {
    if s.is_empty() {
        return "''".to_string();
    }

    if s.chars().all(is_safe_bare) {
        return s.to_string();
    }

    if !s.contains('\'') {
        format!("'{s}'")
    } else {
        format!("\"{}\"", double_quote_escape(s))
    }
}

pub fn quote_args(args: &[&str]) -> String {
    args.iter().map(|s| quote_arg(s)).collect::<Vec<_>>().join(" ")
}
