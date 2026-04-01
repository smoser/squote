fn main() {
    let args: Vec<String> = std::env::args().skip(1).collect();
    if std::env::var("SQUOTE_PASSTHROUGH").as_deref() == Ok("true") {
        let args_ref: Vec<&str> = args.iter().map(String::as_str).collect();
        if !args_ref.is_empty() {
            println!("{}", squote::quote_args(&args_ref));
        }
        return;
    }
    match args.first().map(String::as_str) {
        Some("--version") => println!("squote {}", env!("CARGO_PKG_VERSION")),
        Some("--help") => println!(
            "Usage: squote [--help] [--version] [--] [ARG...]\n\
             Quote ARGs for safe copy/paste into a shell."
        ),
        _ => {
            let args = match args.first().map(String::as_str) {
                Some("--") => &args[1..],
                _ => &args,
            };
            let args_ref: Vec<&str> = args.iter().map(String::as_str).collect();
            if !args_ref.is_empty() {
                println!("{}", squote::quote_args(&args_ref));
            }
        }
    }
}
