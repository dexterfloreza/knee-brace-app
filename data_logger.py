import serial
import csv
from datetime import datetime

# COM port and baud rate
port = "COM7"           # Replace with your port
baud = 115200

# File name based on timestamp
filename = f"session_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"

# Open serial and CSV file
ser = serial.Serial(port, baud)
csvfile = open(filename, "w", newline='')
writer = csv.writer(csvfile)
writer.writerow(["Yaw", "Pitch", "Roll", "Flex", "FSR"])  # header

print(f"📡 Logging serial data to {filename} ... Press Ctrl+C to stop.")

try:
    while True:
        line = ser.readline().decode("utf-8").strip()
        print(line)
        if "Yaw:" in line:
            # Parse values
            try:
                parts = line.replace("Yaw: ", "").replace(" | ", ",").replace("Flex: ", "").replace("FSR: ", "").split(",")
                writer.writerow(parts)
            except:
                continue
except KeyboardInterrupt:
    print("📁 Done logging.")
    csvfile.close()
    ser.close()
