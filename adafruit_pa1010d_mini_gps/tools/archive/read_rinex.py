import georinex as gr

def read_rinex_file(file_path):
    try:
        # Load RINEX data
        data = gr.load(file_path)
        
        # Check if data is loaded successfully
        if data is None or len(data.data_vars) == 0:
            print(f"Failed to load RINEX file: {file_path} or No observation data found.")
            return

        # Print the raw header information
        print("RINEX Metadata:")
        print(data.attrs)  # Display header attributes

        # Check if data contains observations
        print("\nAvailable data variables:")
        print(data.data_vars)  # Check available observation types (e.g., C1C, L1C)

        # Extract and print the satellite PRNs (SVs) and observation times
        if 'sv' in data.coords:
            satellites = data.coords['sv'].values  # Satellite identifiers
            print("\nSatellites being tracked:")
            print(satellites)
        else:
            print("\nNo satellite data found in the file.")

        # Extract the observation times
        if 'time' in data.coords:
            times = data.coords['time'].values  # Time values
            print("\nObservation times:")
            print(times)
        else:
            print("\nNo observation times found in the file.")
        
        # Display observation data (e.g., C1C, L1C, D1C, S1C)
        if len(data.data_vars) > 0:
            for obs_type, obs_data in data.data_vars.items():
                print(f"\nObservation Type: {obs_type}")
                print(obs_data)

    except Exception as e:
        print(f"Error reading RINEX file: {e}")

if __name__ == "__main__":
    rinex_file_path = "test_rinex.obs"  # Replace with your actual RINEX file path
    read_rinex_file(rinex_file_path)
