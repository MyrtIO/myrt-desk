#!/usr/bin/env python3
"""Script to generate C files with the text content of the provided files."""
import os
import sys

USAGE = "Usage: python gen_content_char.py <output_basename> <files...>"

def collect_json_files(path: str) -> list:
    """Collect the json files."""
    files = []
    for root, _, filenames in os.walk(path):
        for filename in filenames:
            if filename.endswith('.json'):
                files.append(os.path.join(root, filename))
    return files

def format_heading(name: str) -> str:
    """Format the heading."""
    return f"const char* k{name.capitalize()}Config"

def extract_name(file: str) -> str:
    """Extract the name from the file."""
    return os.path.basename(file).split('.')[0].split('_')[1]

def map_contents(paths: list[str]) -> dict:
    """Collect the files."""
    file_contents = {}
    for path in paths:
        with open(path, "r", encoding="utf-8") as f:
            file_contents[path] = f.readlines()
    return file_contents

def generate() -> None:
    """Generate the file strings."""
    if len(sys.argv) < 3:
        print(USAGE)
        sys.exit(1)

    out_basename = sys.argv[1]
    out_cc = out_basename + ".cc"
    out_h = out_basename + ".h"

    paths = sys.argv[2:]
    contents = map_contents(paths)
    out_base = os.path.basename(out_basename)
    cpp_content = f"#include \"{out_base}.h\"\n\n"
    h_content = '#pragma once\n\n'
    for file in paths:
        name = extract_name(file)
        h_content += "extern " + format_heading(name) + ";\n"
        cpp_content += format_heading(name)
        cpp_content += " = R\"(\n" + "".join(contents[file]) + ")\";\n\n"

    with open(out_h, "w", encoding="utf-8") as f:
        f.write(h_content)

    with open(out_cc, "w", encoding="utf-8") as f:
        f.write(cpp_content)

    print(f"Chars available at {out_h}")

if __name__ == "__main__":
    generate()
