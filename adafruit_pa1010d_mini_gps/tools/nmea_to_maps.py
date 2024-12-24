import sys, os
from PyQt5.QtWidgets import QApplication, QWidget, QFileDialog, QVBoxLayout, QPushButton, QLabel
from pykml.factory import KML_ElementMaker as KML
from lxml import etree

def nmea_to_decimal(value, direction):
    """
    Convert NMEA latitude or longitude to decimal format.
    """
    if not value or not direction:
        return None
    try:
        # Latitude: First 2 digits for degrees, the rest for minutes.
        if len(value) == 9:  # For 8 digits like 4807.038
            degrees = int(value[:2])
            minutes = float(value[2:])
        elif len(value) == 10:  # For 9 digits like 01234.567
            degrees = int(value[:3])
            minutes = float(value[3:])
        else:
            return None
        
        decimal = degrees + minutes / 60.0
        
        # Adjust for hemisphere (S and W are negative)
        if direction in ["S", "W"]:
            decimal = -decimal
        
        return decimal
    except ValueError:
        print(f"Invalid NMEA value: {value} {direction}")
        return None

def parse_nmea(file_path):
    """
    Parse the NMEA log file to extract GPS coordinates (latitude and longitude).
    """
    coordinates = []
    with open(file_path, "r") as file:
        for line in file:
            # Check for different types of GGA sentences for various constellations
            if line.startswith("$GPGGA") or line.startswith("$GLGGA") or line.startswith("$GAGGA") or line.startswith("$GNGGA"):
                parts = line.split(",")
                if len(parts) > 5:
                    # Latitude and longitude are at parts[2], parts[3] for latitude and parts[4], parts[5] for longitude
                    lat = nmea_to_decimal(parts[2], parts[3])  # Latitude and its direction (N/S)
                    lon = nmea_to_decimal(parts[4], parts[5])  # Longitude and its direction (E/W)
                    if lat is not None and lon is not None:
                        coordinates.append((lon, lat))  # Make sure lon is before lat (Longitude, Latitude)
    return coordinates

def create_kml(coordinates, output_file):
    """
    Create a KML file from the list of coordinates.
    """
    # Create placemarks for each coordinate
    placemarks = [
        KML.Placemark(
            KML.Point(
                KML.coordinates(f"{lon},{lat},0")  # Longitude, Latitude, Altitude (0 for simplicity)
            )
        ) for lon, lat in coordinates
    ]

    # Create the KML document with the placemarks
    kml_document = KML.kml(KML.Document(*placemarks))

    # Write the KML content to the specified file
    with open(output_file, "wb") as file:
        file.write(etree.tostring(kml_document, pretty_print=True))

class NMEAParserApp(QWidget):
    def __init__(self):
        super().__init__()

        self.initUI()

    def initUI(self):
        self.setWindowTitle('NMEA to KML Converter')
        self.setGeometry(100, 100, 400, 200)

        # Layout for the UI
        layout = QVBoxLayout()

        # Label to show the selected file
        self.label = QLabel('No file selected', self)
        layout.addWidget(self.label)

        # Button to open file dialog
        btnOpen = QPushButton('Open NMEA Log File', self)
        btnOpen.clicked.connect(self.openFileDialog)
        layout.addWidget(btnOpen)

        # Button to process the file and generate KML
        btnProcess = QPushButton('Generate KML', self)
        btnProcess.clicked.connect(self.generate_kml_from_file)
        layout.addWidget(btnProcess)

        # Set the layout for the window
        self.setLayout(layout)

    def openFileDialog(self):
        """
        Open a file dialog to select the NMEA log file.
        """
        file, _ = QFileDialog.getOpenFileName(self, "Open NMEA Log File", "", "Text Files (*.txt);;All Files (*)")
        
        if file:
            self.file_path = file
            self.label.setText(f"Selected File: {file}")

    def generate_kml_from_file(self):
        """
        Generate KML from the selected NMEA file.
        """
        if hasattr(self, 'file_path'):
            coordinates = parse_nmea(self.file_path)
            
            if not coordinates:
                self.label.setText("No valid coordinates found in the NMEA file.")
                return

            #output_kml = "output.kml"
            # Generate output file name based on input file name
            input_file_name = os.path.basename(self.file_path)
            base_name, _ = os.path.splitext(input_file_name)
            output_kml = f"{base_name}.kml"
            create_kml(coordinates, output_kml)
            self.label.setText(f"KML file generated: {output_kml}")
        else:
            self.label.setText("No file selected.")

        # Exit the app after generating KML
        QApplication.quit()

def main():
    app = QApplication(sys.argv)
    ex = NMEAParserApp()
    ex.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
