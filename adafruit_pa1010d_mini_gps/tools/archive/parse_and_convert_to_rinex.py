import pynmea2
from datetime import datetime

def parse_nmea_log(nmea_log_file, rinex_output_file):
    """
    Parse NMEA log file and convert to RINEX format.
    Args:
    - nmea_log_file: Path to the input NMEA log file
    - rinex_output_file: Path to the output RINEX file
    """
    with open(nmea_log_file, 'r') as nmea_file, open(rinex_output_file, 'w') as rinex_file:
        # Write RINEX Header
        write_rinex_header(rinex_file)
        
        satellite_data = {}  # To store satellite information from GSV sentences
        
        # Parse each NMEA sentence and convert it to RINEX format
        for line in nmea_file:
            try:
                msg = pynmea2.parse(line.strip())
                
                if isinstance(msg, pynmea2.GGA):  # GPS Fix Data
                    write_rinex_observation(rinex_file, msg)
                
                elif isinstance(msg, pynmea2.GSV):  # Satellites in view
                    satellite_data = update_satellite_info(satellite_data, msg)
                
                elif isinstance(msg, pynmea2.RMC):  # Minimum navigation info
                    pass  # Can be used to extract position, velocity, etc., if needed

                # Add more NMEA sentence types if needed (e.g., GSA)
                
            except pynmea2.nmea.ParseError:
                continue  # Skip lines that cannot be parsed

def write_rinex_header(file):
    """
    Write the RINEX file header.
    Args:
    - file: File object to write the RINEX header
    """
    # Version and file type
    file.write(f"{'2.11':<20}{'OBSERVATION DATA':<20}{'M (MIXED)':<20}{'RINEX VERSION / TYPE':<20}\n")
    
    # Marker information (this should be customized to your setup)
    file.write(f"{'Test GNSS Receiver':<60}{'MARKER NAME':<20}\n")
    file.write(f"{'GNSS Receiver Test':<60}{'OBSERVER / AGENCY':<20}\n")

    # Number of observation types (add observation types based on system)
    file.write(f"{'SYS / # / OBS TYPES G':<20}{7:<4}{'C1 L1 D1 S1 C2 L2 D2':<40}{'SYS / # / OBS TYPES':<20}\n")

    # Timestamp of the file creation
    now = datetime.utcnow().strftime("%Y%m%d %H%M%S UTC")
    file.write(f"{now:<60}{'TIME OF FIRST OBS':<20}\n")
    
    # End of header
    file.write(f"{'END OF HEADER':<60}\n")

def write_rinex_observation(file, data):
    """
    Write RINEX observation data from NMEA parsed sentence.
    Args:
    - file: File object to write the RINEX observation
    - data: Parsed NMEA sentence (e.g., GGA for position data)
    """
    # Convert the NMEA GGA sentence into RINEX observation data
    timestamp = datetime.combine(datetime.utcnow().date(), data.timestamp)  # Use current date with NMEA time
    file.write(f"{timestamp.strftime('%Y %m %d %H %M %S')}  0\n")  # Format for time observation
    
    try:
        # Get number of satellites from GGA sentence
        num_satellites = int(data.num_sats)  # Number of satellites (from GGA sentence)
    except ValueError:
        num_satellites = 0  # If num_sats is not an integer, default to 0

    # Example: Write satellite observations (ensure proper format for each satellite)
    for sat_num in range(1, num_satellites + 1):
        # Using placeholder values for pseudo-range, phase, etc., for each satellite.
        # In a real scenario, replace these with actual observation values.
        prn = f"G{sat_num:02}"  # Satellite PRN, G = GPS system
        obs_values = [12345.678, 23456.789, 34567.890]  # Placeholder observation values
        formatted_obs = "  ".join(f"{val:14.3f}" for val in obs_values)
        file.write(f"{prn}   {formatted_obs}\n")



def update_satellite_info(satellite_data, msg):
    """
    Update satellite information from NMEA GSV sentences.
    Args:
    - satellite_data: Dictionary to store satellite info
    - msg: Parsed NMEA GSV sentence
    """
    # Check the number of satellites reported in this message (up to 4 per GSV message)
    for i in range(1, 5):
        try:
            sv_prn = getattr(msg, f'sv_prn_{i}')
            elevation = getattr(msg, f'elevation_deg_{i}')
            azimuth = getattr(msg, f'azimuth_{i}')
            snr = getattr(msg, f'snr_{i}')
            
            if sv_prn:
                satellite_data[sv_prn] = (elevation, azimuth, snr)
        except AttributeError:
            # Skip if the attribute is not found in the message
            continue
    
    return satellite_data


if __name__ == "__main__":
    # Specify paths to the NMEA log and the output RINEX file
    nmea_log_file = 'nmea_log.txt'
    rinex_output_file = 'output_rinex.obs'
    
    # Convert the NMEA log to RINEX format
    parse_nmea_log(nmea_log_file, rinex_output_file)
