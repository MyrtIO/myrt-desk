#!/usr/bin/env python3
"""
Generate obfuscated file to hide sensitive information in it.
"""
import re
import sys

USAGE = "Usage: python gen_replace.py <source_path> <output_path>"

REPLACEMENT_RE = r'.*\"(.*)\"\s+(\/{2}|#).*replace:.*\"(.*)\"'

def generate() -> None:
    """Generate the example config file from the source config file."""
    if len(sys.argv) < 3:
        print(USAGE)
        sys.exit(1)

    source_path = sys.argv[1]
    output_path = sys.argv[2]

    with open(source_path, "r", encoding="utf-8") as f:
        lines = f.readlines()

    for i, _ in enumerate(lines):
        match = re.search(REPLACEMENT_RE, lines[i])
        if match:
            (original, _, replacement) = match.groups()
            lines[i] = lines[i].replace(original, replacement)

    with open(output_path, "w", encoding="utf-8") as f:
        f.writelines(lines)
    print(f"Obfuscated file generated at {output_path}")

if __name__ == "__main__":
    generate()
