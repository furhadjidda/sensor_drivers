import csv
from datetime import datetime, timezone, timedelta
import matplotlib.pyplot as plt
import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QWidget, QFileDialog
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure


# Function to parse NMEA log
# Function to parse NMEA log
def parse_nmea_log(file_path):
    data = []
    with open(file_path, 'r') as file:
        lines = file.readlines()
    
    current_epoch = {
        'time': None,
        'hdop': None,
        'vdop': None,
        'satellites': None,  # Satellite Count (used in fix)
        'total_satellites': 0,  # Total satellites in view
        'satellite_counts': {'GPS': 0, 'GLONASS': 0, 'Galileo': 0, 'Beidou': 0},
        'signal_strength': [],  # List of signal strengths for each satellite
        'pseudorange': []  # Simulated pseudorange values
    }

    for line in lines:
        line = line.strip()
        if line.startswith('$GNGGA') or line.startswith('$GPGGA'):
            fields = line.split(',')
            if len(fields) > 7 and fields[6] != '':
                current_epoch['time'] = convert_to_local_time(fields[1])
                current_epoch['satellites'] = int(fields[7])
                current_epoch['hdop'] = float(fields[8]) if fields[8] else None
                current_epoch['vdop'] = float(fields[9]) if fields[9] else None

        elif line.startswith('$GPGSV') or line.startswith('$GLGSV') or line.startswith('$GBGSV') or line.startswith('$GAGSV'):
            fields = line.split(',')
            if len(fields) > 3:
                total_satellites_str = fields[3]  # Extract total satellites field
                total_satellites_str = total_satellites_str.split('*')[0]  # Remove checksum part
                try:
                    total_satellites = int(total_satellites_str)
                    current_epoch['total_satellites'] = max(current_epoch['total_satellites'], total_satellites)
                except ValueError:
                    print(f"Skipping invalid total satellites value: {total_satellites_str}")

                # Signal strength extraction (columns after satellite PRN)
                signal_strengths = []
                for i in range(4, len(fields), 4):  # Start from the 4th column and step by 4 (for each satellite)
                    try:
                        signal_strength = fields[i]
                        if signal_strength:  # Only convert if the field is not empty
                            signal_strengths.append(int(signal_strength))
                        else:
                            signal_strengths.append(None)  # Use None for empty fields
                    except ValueError:
                        signal_strengths.append(None)  # In case of invalid values, append None

                current_epoch['signal_strength'].extend(signal_strengths)  # Append signal strength of each satellite

                # Determine GNSS system and count satellites
                if line.startswith('$GPGSV'):
                    current_epoch['satellite_counts']['GPS'] += (len(fields) - 4) // 4
                elif line.startswith('$GLGSV'):
                    current_epoch['satellite_counts']['GLONASS'] += (len(fields) - 4) // 4
                elif line.startswith('$GBGSV'):
                    current_epoch['satellite_counts']['Beidou'] += (len(fields) - 4) // 4
                elif line.startswith('$GAGSV'):
                    current_epoch['satellite_counts']['Galileo'] += (len(fields) - 4) // 4

        elif line.startswith('$GNGSA') or line.startswith('$GPGSA'):
            fields = line.split(',')
            if len(fields) > 17:
                if fields[17]:
                    vdop_value = fields[17].split('*')[0]  # Remove checksum part
                    current_epoch['vdop'] = float(vdop_value) if vdop_value else None

        # Append and reset current epoch if time changes or new data begins
        if current_epoch['time'] and line.startswith('$GN'):
            data.append(current_epoch)
            current_epoch = {
                'time': None,
                'hdop': None,
                'vdop': None,
                'satellites': None,
                'total_satellites': 0,
                'satellite_counts': {'GPS': 0, 'GLONASS': 0, 'Galileo': 0, 'Beidou': 0},
                'signal_strength': [],
                'pseudorange': []
            }
    
    return data

# Convert UTC time to local CST time
def convert_to_local_time(utc_time):
    if not utc_time or len(utc_time) < 6:
        return None
    hour, minute, second = int(utc_time[:2]), int(utc_time[2:4]), float(utc_time[4:])
    utc_dt = datetime.now(timezone.utc).replace(hour=hour, minute=minute, second=int(second), microsecond=int((second % 1) * 1e6))
    cst_dt = utc_dt.astimezone(timezone(timedelta(hours=-6)))  # CST (UTC-6)
    return cst_dt.strftime('%Y-%m-%d %H:%M:%S')

# Write parsed data to a CSV
def write_to_csv(data, output_file):
    fieldnames = ['time', 'hdop', 'vdop', 'satellites', 'total_satellites', 'GPS', 'GLONASS', 'Galileo', 'Beidou', 'signal_strength', 'pseudorange']
    with open(output_file, 'w', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        for item in data:
            row = {key: item.get(key, None) for key in fieldnames}
            if item.get('satellite_counts'):
                row.update(item['satellite_counts'])
            writer.writerow(row)

# PyQt5 Window Class to Display Matplotlib Plots
class PlotWindow(QMainWindow):
    def __init__(self, data):
        super().__init__()
        self.data = data
        self.initUI()

    def initUI(self):
        self.setWindowTitle("NMEA Data Visualization")
        self.setGeometry(100, 100, 1200, 800)
        
        # Main widget and layout
        main_widget = QWidget(self)
        self.setCentralWidget(main_widget)
        layout = QVBoxLayout(main_widget)

        # Add Satellite Count Plot
        satellite_canvas = FigureCanvas(self.create_satellite_plot())
        layout.addWidget(satellite_canvas)

        # Add GNSS System Plot
        gnss_canvas = FigureCanvas(self.create_gnss_plot())
        layout.addWidget(gnss_canvas)

        # Add Signal Strength Plot
        signal_strength_canvas = FigureCanvas(self.create_signal_strength_plot())
        layout.addWidget(signal_strength_canvas)

        # Add HDOP and VDOP Plot
        hdop_vdop_canvas = FigureCanvas(self.create_hdop_vdop_plot())
        layout.addWidget(hdop_vdop_canvas)

    def create_satellite_plot(self):
        # Create the figure for satellite counts
        fig = Figure(figsize=(12, 6))
        ax = fig.add_subplot(111)

        times = [item['time'] for item in self.data if item['time']]
        satellites = [item['satellites'] for item in self.data]
        total_satellites = [item['total_satellites'] for item in self.data]

        ax.plot(times, satellites, label='Satellites Used (Fix)', marker='o')
        ax.plot(times, total_satellites, label='Total Satellites in View', marker='x')

        ax.set_xlabel('Time (CST)')
        ax.set_ylabel('Satellite Count')
        ax.set_title('Satellite Counts Over Time')
        ax.legend()
        ax.grid()

        return fig

    def create_gnss_plot(self):
        # Create the figure for GNSS system counts
        fig = Figure(figsize=(12, 6))
        ax = fig.add_subplot(111)

        times = [item['time'] for item in self.data if item['time']]
        gps_counts = [item['satellite_counts']['GPS'] for item in self.data]
        glonass_counts = [item['satellite_counts']['GLONASS'] for item in self.data]
        galileo_counts = [item['satellite_counts']['Galileo'] for item in self.data]
        beidou_counts = [item['satellite_counts']['Beidou'] for item in self.data]

        ax.plot(times, gps_counts, label='GPS', marker='o')
        ax.plot(times, glonass_counts, label='GLONASS', marker='x')
        ax.plot(times, galileo_counts, label='Galileo', marker='s')
        ax.plot(times, beidou_counts, label='Beidou', marker='^')

        ax.set_xlabel('Time (CST)')
        ax.set_ylabel('Satellite Count')
        ax.set_title('GNSS System Satellite Counts')
        ax.legend()
        ax.grid()

        return fig

    def create_signal_strength_plot(self):
        # Create the figure for signal strength
        fig = Figure(figsize=(12, 6))
        ax = fig.add_subplot(111)

        times = [item['time'] for item in self.data if item['time']]
        signal_strengths = [sum(item['signal_strength']) for item in self.data]  # Sum the signal strengths
        ax.plot(times, signal_strengths, label='Signal Strength', marker='o')

        ax.set_xlabel('Time (CST)')
        ax.set_ylabel('Signal Strength')
        ax.set_title('Signal Strength Over Time')
        ax.legend()
        ax.grid()

        return fig

    def create_signal_strength_plot2(self):
        # Create the figure for signal strength
        fig = Figure(figsize=(12, 6))
        ax = fig.add_subplot(111)

        # Filter out data where signal_strength is missing or empty
        times = [item['time'] for item in self.data if item['time'] and item['signal_strength']]  # Ensure time and signal_strength exist
        signal_strengths = [sum(item['signal_strength']) for item in self.data if item['signal_strength']]  # Sum the signal strengths
        signal_strengths_dbm = [self.convert_to_dbm(ss) for ss in signal_strengths if ss is not None]  # Convert to dBm if needed

        # Check if times and signal_strengths_dbm have the same length
        if len(times) != len(signal_strengths_dbm):
            print(f"Warning: times and signal_strengths_dbm have different lengths! times: {len(times)}, signal_strengths_dbm: {len(signal_strengths_dbm)}")

        ax.plot(times, signal_strengths_dbm, label='Signal Strength (dBm)', marker='o')

        ax.set_xlabel('Time (CST)')
        ax.set_ylabel('Signal Strength (dBm)')
        ax.set_title('Signal Strength Over Time')
        ax.legend()
        ax.grid()

        return fig


    def convert_to_dbm(self, signal_strength):
        # If signal_strength is in some raw value, convert it to dBm using the formula:
        # Assuming the raw value is in the range of received signal (like a negative integer representing dBm)
        # If the values are already in dBm, no conversion is needed.
        if signal_strength is None:
            return None
        return signal_strength  # Replace this if conversion is required, e.g., signal_strength / 100


    def create_hdop_vdop_plot(self):
        # Create the figure for HDOP and VDOP
        fig = Figure(figsize=(12, 6))
        ax = fig.add_subplot(111)

        times = [item['time'] for item in self.data if item['time']]
        hdop = [item['hdop'] for item in self.data]
        vdop = [item['vdop'] for item in self.data]

        ax.plot(times, hdop, label='HDOP', marker='o')
        ax.plot(times, vdop, label='VDOP', marker='x')

        ax.set_xlabel('Time (CST)')
        ax.set_ylabel('DOP Values')
        ax.set_title('HDOP and VDOP Over Time')
        ax.legend()
        ax.grid()

        return fig

# Main script
def get_input_file():
    file_path, _ = QFileDialog.getOpenFileName(None, "Select NMEA Log File", "", "Text Files (*.txt);;All Files (*)")
    return file_path

# Main program
if __name__ == '__main__':    
    # Ensure QApplication is created first
    app = QApplication(sys.argv)
    input_file = get_input_file()
    
    data = parse_nmea_log(input_file)

    #app = QApplication(sys.argv)
    window = PlotWindow(data)
    window.show()
    sys.exit(app.exec_())
