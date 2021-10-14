#!/bin/sh

if [ -z $1 ]
then
    TIMEOUT="0"
else
    TIMEOUT="$1"
fi

# temp files :D
tmpfile="$(mktemp -q -t "$(basename "$0").XXXXXX" 2>/dev/null || mktemp -q)"
exec 3> "$tmpfile" 4< "$tmpfile"
rm -f -- "$tmpfile"

y=0
IFS="\n"
while read -r line
do
    x=0
    tr_line=$(printf "%s" "$line" | tr \  \\001 | hexdump -ve '1/1 "%.2x"')

    tmp="$tr_line"
    while [ -n "$tmp" ]; do
        prime="${tmp#?}"
        rest="${prime#?}"
        first="${tmp%"$rest"}"

        if test "$first" != "01"; then
            printf "%s %d %d\n" $first $y $x >&3
        fi

        x=$((x + 1))
        tmp="$rest"
    done
    y=$((y + 1))
done
exec 3>&-

tput clear
IFS=" "
cat <&4 | shuf - | while read -r sym ypos xpos; do
    sleep $TIMEOUT
    tput cup $ypos $xpos
    printf "\\$(printf "%o" "0x$sym")"
done
exec 4<&-

tput cup $(tput lines) $(tput cols)
while true; do
    :
done

