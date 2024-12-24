import struct

# EPO file format constants
RECORD_SIZE = 72  # Each record is 72 bytes
EPOCH_SIZE = 4    # 4-hour windows for satellite data
SECONDS_PER_HOUR = 3600

# Read the binary EPO file
def read_epo_file(epo_filename):
    try:
        with open(epo_filename, 'rb') as epo_file:
            epo_data = epo_file.read()
            return epo_data
    except FileNotFoundError:
        print(f"Error: File {epo_filename} not found.")
        return None

# Parse each 72-byte record in the EPO file
def parse_epo_data(epo_data):
    num_records = len(epo_data) // RECORD_SIZE
    print(f"Total records: {num_records}")
    
    for record_idx in range(num_records):
        record = epo_data[record_idx * RECORD_SIZE:(record_idx + 1) * RECORD_SIZE]
        
        # Extract the PRN (1 byte)
        prn = record[0]
        
        # Extract time of start of prediction (4 bytes)
        t_oe, = struct.unpack('<I', record[4:8])
        
        # Extract satellite position (6 floats for x, y, z and their velocities)
        position_velocity = struct.unpack('<6f', record[8:32])
        x, y, z, vx, vy, vz = position_velocity
        
        # Extract additional parameters (not all of them are meaningful to decode directly)
        params = struct.unpack('<6f', record[32:56])
        af0, af1, af2, crc, crs, cuc = params
        
        # Example additional float values (relevant for satellite modeling)
        additional_data = struct.unpack('<4f', record[56:72])
        i0, omg, idot, sqrtA = additional_data
        
        # Print satellite data
        print(f"Satellite PRN: {prn}")
        print(f"Time of ephemeris (t_oe): {t_oe} seconds since epoch")
        print(f"Position (x, y, z): ({x:.3f}, {y:.3f}, {z:.3f})")
        print(f"Velocity (vx, vy, vz): ({vx:.3f}, {vy:.3f}, {vz:.3f})")
        print(f"Additional parameters: af0={af0:.6f}, af1={af1:.6f}, af2={af2:.6f}")
        print(f"Orbital params: i0={i0:.6f}, omg={omg:.6f}, idot={idot:.6f}, sqrtA={sqrtA:.6f}")
        print('-' * 50)

# Main function
if __name__ == "__main__":
    epo_filename = "EPO.DAT"  # Path to your EPO file
    epo_data = read_epo_file(epo_filename)
    
    if epo_data:
        parse_epo_data(epo_data)
