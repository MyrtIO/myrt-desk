import sys


def format_c_file(source: bytes, name: str) -> str:
    content = "#include <stdint.h>\n\n"
    content += f"const uint8_t {name}[] "
    content += "= {\n"

    for b in source:
        content += f"0x{b:02x}, "
    content += "\n};\n"
    content += f"const uint32_t {name}_len = {len(source)};\n"

    return content


def format_h_file(name: str) -> str:
    content = "#include <stdint.h>\n\n"
    content += f"extern const uint8_t {name}[];\n"
    content += f"extern const size_t {name}_len;\n"
    return content


def binary_to_source(source: str, target: str, name: str):
    c_file = target + ".c"
    h_file = target + ".h"

    with open(source, "rb") as f:
        data = f.read()
    c_content = format_c_file(data, name)
    h_content = format_h_file(name)

    with open(c_file, "w") as f:
        _ = f.write(c_content)

    with open(h_file, "w") as f:
        _ = f.write(h_content)


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: bin2source.py <source> <target> <const_name>")
        sys.exit(1)
    binary_to_source(sys.argv[1], sys.argv[2], sys.argv[3])
