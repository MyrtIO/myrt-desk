import yaml
import sys
import os

USAGE = "Usage: python gen_config.py <yaml_config_file> <output_h_file>"

def deep_flatten(input_dict: dict, delimiter: str = "_") -> dict:
    """
    Flatten a dictionary. Adds a delimiter between levels.
    Examples:

    ```
    input = {
        "a": {
            "b": {
                "c": 1
            },
        }
    }

    out = {
        "a_b_c": 1
    }
    ```
    """

    output = {}
    for key, value in input_dict.items():
        if isinstance(value, dict):
            for subkey, subvalue in deep_flatten(value, delimiter).items():
                output[key + delimiter + subkey] = subvalue
        else:
            output[key] = value
    return output


def generate() -> None:
    """Generate the config file."""
    if len(sys.argv) < 3:
        print(USAGE)
        sys.exit(1)

    yaml_path = sys.argv[1]
    h_path = sys.argv[2]

    with open(yaml_path, "r", encoding="utf-8") as f:
        yaml_content = yaml.safe_load(f)

    yaml_content = deep_flatten(yaml_content)
    h_content = ""
    for key, value in yaml_content.items():
        config_value = value
        if isinstance(config_value, str):
            if config_value.startswith('$raw'):
                config_value = config_value[5:]
            else:
                config_value = f'"{config_value}"'
        h_content += f"#define CONFIG_{key.upper()} {config_value}\n"

    with open(h_path, "w", encoding="utf-8") as f:
        f.write(h_content)

    print(f"Config generated at {h_path}")


if __name__ == "__main__":
    generate()
