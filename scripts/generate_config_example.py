"""
Generate the example config file from the source config file.
"""
import re

SOURCE_PATH = "src/config.h"
EXAMPLE_PATH = "src/config.example.h"

def generate() -> None:
    """Generate the example config file from the source config file."""
    with open(SOURCE_PATH, "r", encoding="utf-8") as f:
        lines = f.readlines()

    for i, line in enumerate(lines):
        if '// REPLACE:' in line:
            replacement = line.split(':')[1].strip()
            result = re.match(r'.* "(.*)"', line)
            lines[i] = lines[i].replace(result[1], replacement)

    with open(EXAMPLE_PATH, "w", encoding="utf-8") as f:
        f.writelines(lines)
    print(f"Config template generated at {EXAMPLE_PATH}")

generate()
