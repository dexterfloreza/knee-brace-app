import serial
import csv
from datetime import datetime
import os

# Safe path
save_dir = "C:\\Temp"
os.makedirs(save_dir, exist_ok=True)  # Create folder if missing

# File name
filename = os.path.join(save_dir, f"session_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv")

# Serial config
port = "COM5"  # 🔧 Change this if your ESP32 is on another port
baud = 115200

# Try opening serial
try:
    ser = serial.Serial(port, baud)
except Exception as e:
    print(f"❌ Could not open serial port {port}: {e}")
    exit()

# Try opening CSV
try:
    csvfile = open(filename, "w", newline='')
    writer = csv.writer(csvfile)
    writer.writerow(["Yaw", "Pitch", "Roll", "Flex", "FSR"])
except Exception as e:
    print(f"❌ Could not open file for writing: {e}")
    ser.close()
    exit()

print(f"✅ Logging to {filename}... Press Ctrl+C to stop.\n")

try:
    while True:
        line = ser.readline().decode("utf-8").strip()
        print(line)
        if "Yaw:" in line:
            try:
                # Parse ESP32 output string
                parts = line.replace("Yaw: ", "") \
                            .replace(" | Pitch: ", ",") \
                            .replace(" | Roll: ", ",") \
                            .replace(" | Flex: ", ",") \
                            .replace(" | FSR: ", ",") \
                            .split(",")
                if len(parts) == 5:
                    writer.writerow(parts)
            except Exception as e:
                print(f"⚠️ Parse error: {e}")
except KeyboardInterrupt:
    print("\n✅ Stopped logging.")
    csvfile.close()
    ser.close()
