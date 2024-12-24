import serial
from datetime import datetime

# Set up serial communication (adjust port and baudrate as per your setup)
ser = serial.Serial('/dev/ttyACM1', 115200, timeout=1)  # Replace '/dev/ttyUSB0' with your port

# Get current date and timestamp for file naming
current_time = datetime.now()
date_str = current_time.strftime("%y%m%d")  # Format the date as yymmdd
timestamp_str = current_time.strftime("%H%M%S")  # Format the timestamp as HHMMSS

# Create the filename with the format nmea_<yymmdd>_timestamp.txt
filename = f"nmea_{date_str}_{timestamp_str}.txt"

# Open file to write NMEA sentences
with open(filename, "w") as file:
    try:
        while True:
            # Read a line from the serial port
            line = ser.readline().decode('ascii', errors='replace').strip()

            # If the line starts with '$', it's a valid NMEA sentence
            if line.startswith('$'):
                print(line)  # Print the sentence to the terminal
                file.write(line + '\n')  # Write the sentence to the file

    except KeyboardInterrupt:
        # Stop the script with Ctrl+C
        print("Stopping NMEA capture.")
    finally:
        ser.close()  # Close the serial connection when done
