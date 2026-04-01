fn main() {
    let args: Vec<String> = std::env::args().skip(1).collect();
    let args_ref: Vec<&str> = args.iter().map(String::as_str).collect();
    if !args_ref.is_empty() {
        println!("{}", squote::quote_args(&args_ref));
    }
}
