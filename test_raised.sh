#!/bin/sh

for test; do
    if ./$test 2> ${test}.log; then
    	echo "test '$test' should fail; got success instead"
    	exit 1
    fi

    if ! grep -q "^raised " ${test}.log; then
    	echo "test '$test' should produce a 'raised' message, got:"
    	cat ${test}.log
    	exit 1
    fi
done
