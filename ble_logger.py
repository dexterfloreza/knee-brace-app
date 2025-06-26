import asyncio
from bleak import BleakClient, BleakScanner
import csv
from datetime import datetime

DEVICE_NAME = "ESP32-SensorHub"

csv_filename = f"session_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"
csv_file = open(csv_filename, mode='w', newline='')
csv_writer = csv.writer(csv_file)
csv_writer.writerow(["Yaw", "Pitch", "Roll", "Flex", "FSR"])

def handle_notification(handle, data):
    try:
        decoded = data.decode("utf-8").strip()
        print(f"🔹 {decoded}")
        parts = decoded.split(",")
        if len(parts) == 5:
            csv_writer.writerow(parts)
    except Exception as e:
        print(f"⚠️ Error decoding: {e}")

async def main():
    print("🔍 Scanning for ESP32...")
    devices = await BleakScanner.discover()
    
    target = None
    for d in devices:
        if d.name and DEVICE_NAME in d.name:
            target = d
            break
    
    if not target:
        print("❌ ESP32 device not found.")
        return
    
    print(f"✅ Found {DEVICE_NAME} at {target.address}")

    async with BleakClient(target.address) as client:
        print("🔗 Connected. Discovering services...")
        await client.get_services()

        # Find first notify-capable characteristic
        notify_char = None
        print("\n🧬 Available Notify Characteristics:")
        for service in client.services:
            for char in service.characteristics:
                if "notify" in char.properties:
                    print(f"👉 {char.uuid} in service {service.uuid}")
                    if not notify_char:
                        notify_char = char

        if not notify_char:
            print("❌ No notify-capable characteristic found.")
            return

        print(f"\n📡 Subscribing to: {notify_char.uuid}\n")
        await client.start_notify(notify_char.uuid, handle_notification)

        try:
            while True:
                await asyncio.sleep(1)
        except KeyboardInterrupt:
            print("🛑 Stopping...")
            await client.stop_notify(notify_char.uuid)
            csv_file.close()
            print(f"💾 Data saved to {csv_filename}")

asyncio.run(main())
