import asyncio
from bleak import BleakClient, BleakScanner
import csv
from datetime import datetime

DEVICE_NAME = "ESP32-SensorHub"
CHARACTERISTIC_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

# Prepare CSV file
timestamp_str = datetime.now().strftime("%Y%m%d_%H%M%S")
csv_filename = f"esp32_data_{timestamp_str}.csv"
csv_file = open(csv_filename, mode='w', newline='')
csv_writer = csv.writer(csv_file)
csv_writer.writerow(["WallTime", "Yaw", "Pitch", "Roll", "Flex", "FSR"])

# Handle BLE notification
def handle_notification(_, data):
    try:
        decoded = data.decode("utf-8").strip()
        print(f"🔹 {decoded}")
        parts = decoded.split(",")
        if len(parts) == 5:
            now = datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")
            csv_writer.writerow([now] + parts)
    except Exception as e:
        print(f"⚠️ Error: {e}")

async def main():
    print("🔍 Scanning for ESP32...")
    devices = await BleakScanner.discover()

    target = next((d for d in devices if d.name == DEVICE_NAME), None)

    if not target:
        print("❌ ESP32 not found.")
        return

    print(f"✅ Found {DEVICE_NAME} at {target.address}")

    async with BleakClient(target.address) as client:
        print("🔗 Connected. Subscribing to notifications...")

        await client.start_notify(CHARACTERISTIC_UUID, handle_notification)

        try:
            while True:
                await asyncio.sleep(1)
        except KeyboardInterrupt:
            print("\n🛑 Stopping...")
            await client.stop_notify(CHARACTERISTIC_UUID)
            csv_file.close()
            print(f"💾 Data saved to {csv_filename}")

if __name__ == "__main__":
    asyncio.run(main())
