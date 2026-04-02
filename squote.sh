squote() {
    local buf="" c="" first="" out="" rest="" val=""
    _sq_quote_one() {
        case "$1" in
            '')
                buf="${buf}''"
                return
                ;;
        esac
        case "$1" in
            *[!a-zA-Z0-9@+:,./_-]*)
                ;;
            *)
                buf="${buf}$1"
                return
                ;;
        esac
        case "$1" in
            *\'*)
                # Has single quote: use double quotes, escape " $ ` \ !
                out=''
                rest="$1"
                while [ -n "$rest" ]; do
                    c="${rest%"${rest#?}"}"  # first char
                    rest="${rest#?}"              # remainder
                    case "$c" in
                        [\"'$'"\`\\!"]) out="${out}\\${c}" ;;
                        *)              out="${out}${c}" ;;
                    esac
                done
                buf="${buf}\"${out}\""
                ;;
            *)
                buf="${buf}'$1'"
                ;;
        esac
    }

    [ "$1" = "--" ] && shift

    buf=''
    first=1
    for val in "$@"; do
        [ "$first" = 1 ] || buf="${buf} "
        first=0
        _sq_quote_one "$val"
    done
    [ "$#" -gt 0 ] && printf '%s\n' "$buf"
}
