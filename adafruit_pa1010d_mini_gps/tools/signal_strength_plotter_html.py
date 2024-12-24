import sys
import re
import pandas as pd
import plotly.express as px
from PyQt5.QtWidgets import QApplication, QMainWindow, QFileDialog, QVBoxLayout, QPushButton, QWidget

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

    def load_file(self):
        # Open file dialog
        options = QFileDialog.Options()
        file_path, _ = QFileDialog.getOpenFileName(self, "Select NMEA Log File", "", "Text Files (*.txt);;All Files (*)", options=options)
        
        if file_path:
            # Parse the file
            satellite_data = self.parse_nmea_file(file_path)
            if not satellite_data.empty:
                self.create_interactive_html_plot(satellite_data, file_path)
            else:
                print("No valid GSV data found in the file.")


    def parse_nmea_file(self, file_path):
        """
        Parses the NMEA file to extract signal strength for each satellite.
        
        Returns:
            pd.DataFrame: A DataFrame containing satellite PRN, constellation, signal strength, and observation index.
        """
        constellation_map = {
            "GPGSV": "GPS",
            "GLGSV": "GLONASS",
            "GAGSV": "Galileo",
            "BDGSV": "BeiDou",
            "QZGSV": "QZSS",
            "IRGSV": "IRNSS"
        }

        data = {
            "Satellite": [],
            "Constellation": [],
            "SNR": [],
            "Observation": []
        }

        observation_index = 0

        with open(file_path, 'r') as file:
            for line in file:
                match = re.match(r"^\$(..GSV)", line)
                if match:
                    message_type = match.group(1)
                    constellation = constellation_map.get(message_type, "Unknown")
                    fields = line.split(',')

                    # Each satellite's data spans 4 fields: PRN, Elevation, Azimuth, SNR
                    for i in range(4, len(fields) - 4, 4):
                        try:
                            prn = fields[i]
                            snr = fields[i + 3]

                            if prn and snr and snr.isdigit():
                                prn = int(prn)
                                snr = int(snr)

                                data["Satellite"].append(f"{prn}")
                                data["Constellation"].append(constellation)
                                data["SNR"].append(snr)
                                data["Observation"].append(observation_index)
                        except (ValueError, IndexError):
                            # Skip invalid or incomplete data
                            continue

                    observation_index += 1

        return pd.DataFrame(data)

    def create_interactive_html_plot(self, satellite_data, file_path):
        """
        Creates an interactive HTML plot for satellite signal strengths.
        
        :param satellite_data: DataFrame containing parsed satellite data.
        :param file_path: Path to the input NMEA file (used for naming the output HTML file).
        """
        # Create the plotly figure
        fig = px.line(
            satellite_data,
            x="Observation",
            y="SNR",
            color="Satellite",
            line_group="Satellite",
            hover_data=["Constellation"],
            labels={"SNR": "Signal Strength (SNR, dB-Hz)", "Observation": "Observation Index"},
            title="Interactive Satellite Signal Strength"
        )

        fig.update_layout(
            legend_title="Satellite PRN",
            xaxis_title="Observation Index",
            yaxis_title="Signal Strength (SNR, dB-Hz)",
            template="plotly_white"
        )

        # Save to HTML
        output_file = file_path.replace(".txt", "_signal_strength.html")
        fig.write_html(output_file)
        print(f"Interactive HTML plot saved to: {output_file}")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    main_window = NMEAParserPlotter()
    main_window.show()
    sys.exit(app.exec_())
