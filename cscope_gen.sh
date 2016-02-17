#!/usr/bin/env bash

find . \( -path './action_skel_code' -prune -o -path './.git' -prune -o -path './lib' -prune -o -path '*/gtest' -prune -o -name '*.cpp' -o -name '*.inl' -o -name '*.hpp' -o -name '*.h' -o -name '*.c' \) -a -type f > cscope.files

cscope -b -q
