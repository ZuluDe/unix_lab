#!/bin/sh

dir="/data"

id=$(cat /proc/sys/kernel/random/uuid)
count=0

while true; do
    count=$((count + 1))

    name=$(

        flock 9
        i=1

        while true; do

            f=$(printf "%03d" "$i")

            if [ ! -e "$dir/$f" ]; then

                echo "$id $count" > "$dir/$f"
                echo "$f"

                break
            fi

            i=$((i + 1))

	    done
    ) 9>>"$dir/.lock"

    sleep 1

    rm -f "$dir/$name"
done
