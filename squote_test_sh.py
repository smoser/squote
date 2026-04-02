#!/usr/bin/env python3
import json
import os
import subprocess
import sys

fname = sys.argv[1] if len(sys.argv) > 1 else "tests.json"
script = os.path.join(os.path.dirname(os.path.abspath(fname)), "squote.sh")

with open(fname) as f:
    cases = json.load(f)

passed = failed = 0
for case in cases:
    args = case["input"]
    want = case["want"]
    result = subprocess.run(
        ["sh", "-c", '. "$0"; squote "$@"', script] + args,
        capture_output=True, text=True
    )
    got = result.stdout.rstrip("\n")
    if got == want:
        passed += 1
    else:
        print(f"FAIL: input={args!r}")
        print(f"  got:  {got!r}")
        print(f"  want: {want!r}")
        failed += 1

print(f"{passed} passed, {failed} failed")
sys.exit(1 if failed else 0)
