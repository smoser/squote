# squote - shell quote
Often times in a shell script or some other program when I execute a subprocess
I want to print the command that was executed to a log or debug message.
The reader of such a mesage could make sense of a json encoded array
but it isn't as useful if they want to use that command.

This repo provides "shell quote" in several different languages, a function
to represent an array in a way that can be given to shell.

    $ squote "Isn't this great?" "Yes"
    "Isn't this great?" Yes

    $ squote 'it\''s $HOME'

It prefers double quotes to single quotes, and won't use either unless it
is required.  The goal is to provide the command the way a human would type it.


## Notes
1. '%' is not in 'safe-bare' set here because a bare '%1' can trigger job control.
2. '=' in not in 'safe-bare' set because in zsh, bare =word triggers path expansion (equivalent to which word), so squote =ls would
