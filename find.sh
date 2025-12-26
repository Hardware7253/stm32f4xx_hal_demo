#!/bin/bash
# A script to help find the file where something is defined

find=$1
exclude="${2:-default_value_123456}"

grep -rH --include="*.h" $find . | grep -v $exclude
grep -rH --include="*.c" $find . | grep -v $exclude