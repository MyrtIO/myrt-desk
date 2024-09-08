import os

CONFIG_DIR = 'src/controllers/mqtt/topics'
OUT_PATH = 'src/controllers/mqtt/topics/config_data'
CC_OUT = OUT_PATH + '.cc'
H_OUT = OUT_PATH + '.h'

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
    paths = collect_json_files(CONFIG_DIR)
    contents = map_contents(paths)
    out_base = os.path.basename(OUT_PATH)
    cpp_content = f"#include \"{out_base}.h\"\n\n"
    h_content = '#pragma once\n\n'
    for file in paths:
        name = extract_name(file)
        h_content += "extern " + format_heading(name) + ";\n"
        cpp_content += format_heading(name)
        cpp_content += " = R\"(\n" + "".join(contents[file]) + ")\";\n\n"

    with open(H_OUT, "w", encoding="utf-8") as f:
        f.write(h_content)

    with open(CC_OUT, "w", encoding="utf-8") as f:
        f.write(cpp_content)

if __name__ == "__main__":
    generate()
