import matplotlib.pyplot as plt
from datetime import datetime
from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QWidget
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import sys

# Function to convert NMEA timestamp (hhmmss.sss) to datetime
def parse_nmea_timestamp(timestamp_str):
    try:
        # Parsing timestamp of the format hhmmss.sss
        return datetime.strptime(timestamp_str, "%H%M%S.%f")
    except ValueError:
        print(f"Error parsing timestamp: {timestamp_str}")
        return None

# Sample data parsing function
def parse_nmea_log(input_file):
    data = []
    first_fix_time = None  # To track the first fix time
    for line in input_file:
        fields = line.split(',')
        if line.startswith('$GPGGA') or line.startswith('$GNGGA'):
            if len(fields) > 9:
                fix_status = fields[6]
                timestamp = fields[1]
                parsed_time = parse_nmea_timestamp(timestamp)
                if parsed_time:
                    if fix_status in ['1', '2']:  # 1: 2D fix, 2: 3D fix
                        if first_fix_time is None:
                            first_fix_time = parsed_time  # Record time of first fix
                    data.append({
                        'time': parsed_time,
                        'fix_status': fix_status,
                        'first_fix_time': first_fix_time
                    })
    return data

# Time to First Fix calculation
def calculate_ttff(data):
    first_fix_time = None
    ttff = []
    for entry in data:
        if entry['fix_status'] in ['1', '2']:  # Only calculate TTFF for valid fixes
            if first_fix_time is None:
                first_fix_time = entry['time']  # Record the time of first fix
                ttff.append((entry['time'], 0))  # TTFF for first fix is 0 seconds
            else:
                ttff_seconds = (entry['time'] - first_fix_time).total_seconds()
                ttff.append((entry['time'], ttff_seconds))  # Calculate TTFF from first fix
    return ttff

# PyQt Application class
class PlotWindow(QMainWindow):
    def __init__(self, data):
        super().__init__()
        self.data = data
        self.initUI()

    def initUI(self):
        self.setWindowTitle("NMEA Data Visualization")
        self.setGeometry(100, 100, 1200, 800)

        main_widget = QWidget(self)
        self.setCentralWidget(main_widget)
        layout = QVBoxLayout(main_widget)

        # Plot for Time to First Fix
        ttff_canvas = FigureCanvas(self.create_ttff_plot())
        layout.addWidget(ttff_canvas)

    def create_ttff_plot(self):
        fig = Figure(figsize=(12, 6))
        ax = fig.add_subplot(111)

        # Extract times and calculate TTFF
        ttff = calculate_ttff(self.data)
        ttff_times = [entry[0] for entry in ttff]
        ttff_values = [entry[1] for entry in ttff]

        ax.plot(ttff_times, ttff_values, label='Time to First Fix (TTFF)', marker='o', color='red')
        ax.set_xlabel('Time (UTC)')
        ax.set_ylabel('TTFF (seconds)')
        ax.set_title('Time to First Fix (TTFF) Over Time')
        ax.legend()
        ax.grid()

        return fig

# Function to run the GUI
def generate_plots(data):
    app = QApplication(sys.argv)
    window = PlotWindow(data)
    window.show()
    sys.exit(app.exec_())

# Main function
if __name__ == "__main__":
    # Prompt the user for the NMEA log file
    input_file_path = input("Please enter the path to the NMEA log file: ")

    # Check if the file exists and handle potential errors
    try:
        with open(input_file_path, 'r') as input_file:
            data = parse_nmea_log(input_file)
            if not data:
                print(f"No valid data found in {input_file_path}. Please ensure the file contains valid NMEA sentences.")
            else:
                generate_plots(data)
    except FileNotFoundError:
        print(f"The file {input_file_path} was not found. Please check the file path and try again.")
    except Exception as e:
        print(f"An error occurred: {e}")
