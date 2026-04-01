#!/usr/bin/env python3
import json
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from squote import quote_args


def main():
    fname = sys.argv[1] if len(sys.argv) > 1 else "tests.json"
    with open(fname) as f:
        cases = json.load(f)

    passed = failed = 0
    for case in cases:
        got  = quote_args(case["input"])
        want = case["want"]
        if got == want:
            passed += 1
        else:
            print(f"FAIL: input={case['input']!r}")
            print(f"  got:  {got}")
            print(f"  want: {want}")
            failed += 1

    print(f"{passed} passed, {failed} failed")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
