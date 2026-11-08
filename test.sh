#!/bin/sh
# Builds and runs the reference test with a Grail compiler: GRAIL=/path/to/grail ./test.sh
set -e
GRAIL=${GRAIL:-grail}
dir=$(mktemp -d)
cp ./*.grl "$dir"/
cp tests/cases.grl "$dir"/main.grl
(cd "$dir" && "$GRAIL" build main.grl && ./target/main) | tee "$dir/out.txt"
if grep -q -v -E "ok|: 0 of" "$dir/out.txt"; then echo "FAIL"; exit 1; fi
echo PASS
