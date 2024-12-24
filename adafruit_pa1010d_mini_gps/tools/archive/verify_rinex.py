import re

def verify_rinex(file_path):
    """
    Verify the structure and contents of a RINEX file.

    Args:
        file_path (str): Path to the RINEX file.

    Returns:
        bool: True if the RINEX file is valid, False otherwise.
    """
    try:
        with open(file_path, 'r') as f:
            lines = f.readlines()

        # Check for mandatory fields in the header
        required_fields = {
            "RINEX VERSION / TYPE": False,
            "END OF HEADER": False
        }

        header_end_index = None
        for i, line in enumerate(lines):
            for field in required_fields:
                if field in line:
                    required_fields[field] = True
                    if field == "END OF HEADER":
                        header_end_index = i
            if all(required_fields.values()):
                break

        # If header is incomplete
        if not all(required_fields.values()):
            print("Error: Missing required header fields.")
            return False

        # Validate the header's RINEX version
        version_line = lines[0]
        version_match = re.match(r"^\s*([\d.]+)\s+OBSERVATION DATA", version_line)
        if not version_match:
            print("Error: Invalid RINEX version or missing 'OBSERVATION DATA'.")
            return False

        # Validate observation data after the header
        if header_end_index is None:
            print("Error: 'END OF HEADER' not found.")
            return False

        obs_lines = lines[header_end_index + 1:]
        if not obs_lines:
            print("Error: Observation data missing.")
            return False

        # Validate observation lines (basic format check)
        obs_format = re.compile(r"^\s*\d{4}\s\d{2}\s\d{2}\s\d{2}\s\d{2}\s\d{2}\s+\d+")
        for i, line in enumerate(obs_lines):
            if not obs_format.match(line[:30]):
                print(f"Error: Invalid observation line at {header_end_index + 1 + i}: {line.strip()}")
                return False

        print("RINEX file is valid.")
        return True

    except FileNotFoundError:
        print(f"Error: File not found - {file_path}")
        return False
    except Exception as e:
        print(f"Error: An unexpected error occurred - {e}")
        return False


if __name__ == "__main__":
    # Replace with your RINEX file path
    rinex_file = "output_rinex.obs"
    is_valid = verify_rinex(rinex_file)
    if is_valid:
        print("The RINEX file passed verification.")
    else:
        print("The RINEX file failed verification.")
