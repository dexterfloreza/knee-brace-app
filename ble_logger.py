from bleak import BleakClient, BleakScanner
import asyncio

ADDRESS = None  # We'll scan for this
DEVICE_NAME = "ESP32-SensorHub"
SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
CHARACTERISTIC_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

def notification_handler(_, data):
    print("🔹", data.decode().strip())

async def run():
    global ADDRESS
    print("🔍 Scanning for device...")
    devices = await BleakScanner.discover()
    for d in devices:
        if d.name == DEVICE_NAME:
            ADDRESS = d.address
            break

    if not ADDRESS:
        print("❌ Device not found")
        return

    async with BleakClient(ADDRESS) as client:
        print(f"✅ Connected to {DEVICE_NAME}")
        await client.start_notify(CHARACTERISTIC_UUID, notification_handler)
        try:
            while True:
                await asyncio.sleep(1)
        except KeyboardInterrupt:
            print("🛑 Disconnecting...")
            await client.stop_notify(CHARACTERISTIC_UUID)

asyncio.run(run())
