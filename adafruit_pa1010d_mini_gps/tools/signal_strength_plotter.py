import sys
import re
from PyQt5.QtWidgets import QApplication, QMainWindow, QFileDialog, QVBoxLayout, QPushButton, QWidget
from matplotlib.figure import Figure
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas

class NMEAParserPlotter(QMainWindow):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setWindowTitle("NMEA Signal Strength Plotter")
        self.setGeometry(200, 200, 800, 600)

        # Central widget
        self.central_widget = QWidget(self)
        self.setCentralWidget(self.central_widget)
        
        # Layout
        self.layout = QVBoxLayout(self.central_widget)

        # Button to load file
        self.load_button = QPushButton("Load NMEA Log", self)
        self.load_button.clicked.connect(self.load_file)
        self.layout.addWidget(self.load_button)

        # Matplotlib figure
        self.figure = Figure()
        self.canvas = FigureCanvas(self.figure)
        self.layout.addWidget(self.canvas)

    def load_file(self):
        # Open file dialog
        options = QFileDialog.Options()
        file_path, _ = QFileDialog.getOpenFileName(self, "Select NMEA Log File", "", "Text Files (*.txt);;All Files (*)", options=options)
        
        if file_path:
            # Parse the file
            satellite_data = self.parse_nmea_file(file_path)
            if satellite_data:
                self.plot_signal_strength(satellite_data)
            else:
                print("No valid GSV data found in the file.")

    def parse_nmea_file(self, file_path):
        """
        Parses the NMEA file to extract signal strength for each satellite.
        
        Returns:
            dict: A dictionary where keys are satellite PRNs and values are lists of SNR values over time.
        """
        satellite_data = {}

        with open(file_path, 'r') as file:
            for line in file:
                if line.startswith('$GPGSV') or line.startswith('$GLGSV') or line.startswith('$GAGSV'):
                    fields = line.split(',')

                    # Each satellite's data spans 4 fields: PRN, Elevation, Azimuth, SNR
                    for i in range(4, len(fields) - 4, 4):
                        try:
                            prn = fields[i]
                            snr = fields[i + 3]

                            if prn and snr and snr.isdigit():
                                prn = int(prn)
                                snr = int(snr)

                                if prn not in satellite_data:
                                    satellite_data[prn] = []
                                satellite_data[prn].append(snr)
                        except (ValueError, IndexError):
                            # Skip invalid or incomplete data
                            continue
        return satellite_data

    def plot_signal_strength(self, satellite_data):
        """
        Plots the signal strength for each satellite.
        """
        self.figure.clear()
        ax = self.figure.add_subplot(111)

        for prn, snr_values in satellite_data.items():
            ax.plot(snr_values, label=f"Satellite {prn}")

        ax.set_xlabel("Observation Index")
        ax.set_ylabel("Signal Strength (SNR, dB-Hz)")
        ax.set_title("Signal Strength of Satellites")
        ax.legend()
        ax.grid()

        self.canvas.draw()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    main_window = NMEAParserPlotter()
    main_window.show()
    sys.exit(app.exec_())
