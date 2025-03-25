#!/usr/bin/env bash

set -e

# Change to the location of the script
(
    cd "$(dirname "$0")"/../build
    make
    for i in {1..100}; do
        echo "Run #$i"
        ctest
    done
)
