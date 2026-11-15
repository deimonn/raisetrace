#!/bin/sh

exit_code=0

for file in *.h *.c; do
    if ! indent -st "$file" | sed 's/{ 0 }/{0}/' | diff --color=always "$file" -; then
        echo "make the above changes to '$file'"
        exit_code=1
    fi
done

exit $exit_code
