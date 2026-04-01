fn skip_ws(s: &[u8], pos: &mut usize) {
    while *pos < s.len() && s[*pos].is_ascii_whitespace() {
        *pos += 1;
    }
}

fn eat(s: &[u8], pos: &mut usize, c: u8) -> bool {
    skip_ws(s, pos);
    if s.get(*pos) == Some(&c) {
        *pos += 1;
        true
    } else {
        false
    }
}

fn parse_str(s: &[u8], pos: &mut usize) -> Option<String> {
    if !eat(s, pos, b'"') {
        return None;
    }
    let mut result = String::new();
    loop {
        match s.get(*pos)? {
            b'"' => {
                *pos += 1;
                return Some(result);
            }
            b'\\' => {
                *pos += 1;
                let esc = *s.get(*pos)?;
                *pos += 1;
                result.push(match esc {
                    b'n' => '\n',
                    b't' => '\t',
                    b'r' => '\r',
                    e => e as char,
                });
            }
            &c => {
                *pos += 1;
                result.push(c as char);
            }
        }
    }
}

fn parse_str_array(s: &[u8], pos: &mut usize) -> Option<Vec<String>> {
    if !eat(s, pos, b'[') {
        return None;
    }
    let mut result = Vec::new();
    skip_ws(s, pos);
    if s.get(*pos) == Some(&b']') {
        *pos += 1;
        return Some(result);
    }
    loop {
        result.push(parse_str(s, pos)?);
        skip_ws(s, pos);
        if s.get(*pos) == Some(&b',') {
            *pos += 1;
        } else {
            break;
        }
    }
    eat(s, pos, b']').then_some(result)
}

struct Case {
    input: Vec<String>,
    want: String,
}

fn parse_case(s: &[u8], pos: &mut usize) -> Option<Case> {
    if !eat(s, pos, b'{') {
        return None;
    }
    let (mut input, mut want) = (None, None);
    for _ in 0..2 {
        let key = parse_str(s, pos)?;
        if !eat(s, pos, b':') {
            return None;
        }
        match key.as_str() {
            "input" => input = Some(parse_str_array(s, pos)?),
            "want" => want = Some(parse_str(s, pos)?),
            _ => return None,
        }
        skip_ws(s, pos);
        if s.get(*pos) == Some(&b',') {
            *pos += 1;
        }
    }
    eat(s, pos, b'}').then_some(Case { input: input?, want: want? })
}

fn load_cases(s: &[u8]) -> Vec<Case> {
    let mut pos = 0;
    let mut cases = Vec::new();
    assert!(eat(s, &mut pos, b'['));
    skip_ws(s, &mut pos);
    while s.get(pos) != Some(&b']') && pos < s.len() {
        cases.push(parse_case(s, &mut pos).expect("parse error in tests.json"));
        skip_ws(s, &mut pos);
        if s.get(pos) == Some(&b',') {
            pos += 1;
        }
        skip_ws(s, &mut pos);
    }
    cases
}

#[test]
fn json_test_cases() {
    let path = std::path::Path::new(env!("CARGO_MANIFEST_DIR")).join("tests.json");
    let data = std::fs::read(&path).expect("could not read tests.json");
    let cases = load_cases(&data);

    let mut failed = 0;
    for case in &cases {
        let args: Vec<&str> = case.input.iter().map(String::as_str).collect();
        let got = squote::quote_args(&args);
        if got != case.want {
            eprintln!("FAIL: input={:?}\n  got:  {}\n  want: {}", case.input, got, case.want);
            failed += 1;
        }
    }
    assert_eq!(failed, 0, "{failed} of {} test cases failed", cases.len());
}
