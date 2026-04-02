squote() {
    _sq_quote_one() {
        case "$1" in
            '')
                printf "''"
                return
                ;;
        esac
        case "$1" in
            *[!a-zA-Z0-9@+:,./_-]*)
                ;;
            *)
                printf '%s' "$1"
                return
                ;;
        esac
        case "$1" in
            *\'*)
                # Has single quote: use double quotes, escape " $ ` \ !
                _sq_out=''
                _sq_rest="$1"
                while [ -n "$_sq_rest" ]; do
                    _sq_c="${_sq_rest%"${_sq_rest#?}"}"  # first char
                    _sq_rest="${_sq_rest#?}"              # remainder
                    case "$_sq_c" in
                        [\"'$'"\`\\!"]) _sq_out="${_sq_out}\\${_sq_c}" ;;
                        *)              _sq_out="${_sq_out}${_sq_c}" ;;
                    esac
                done
                printf '"%s"' "$_sq_out"
                ;;
            *)
                printf "'%s'" "$1"
                ;;
        esac
    }

    case "$1" in --) shift ;; esac

    _sq_first=1
    for _sq_val in "$@"; do
        [ "$_sq_first" = 1 ] || printf ' '
        _sq_first=0
        _sq_quote_one "$_sq_val"
    done
    [ "$#" -gt 0 ] && printf '\n'
}
