import requests
import cv2
import easyocr
import serial
import time

# === CONFIGURATION ===
esp_cam_ip = "192.168.0.245"  # ESP32-CAM IP
photo_url = f"http://{esp_cam_ip}/photo"
photo_path = "photo.jpg"

serial_port = "COM3"  # Change to your second ESP32 COM port
baud_rate = 115200

# === Step 1: Download image from ESP32-CAM ===
try:
    print(f"Downloading photo from {photo_url}...")
    response = requests.get(photo_url, timeout=5)
    if response.status_code == 200:
        with open(photo_path, "wb") as f:
            f.write(response.content)
        print(f"Photo downloaded and saved as {photo_path}")
    else:
        print(f"Failed to download image. Status code: {response.status_code}")
        exit(1)
except requests.exceptions.RequestException as e:
    print(f"Request error: {e}")
    exit(1)

# === Step 2: OCR using EasyOCR ===
print("Performing OCR...")
img = cv2.imread(photo_path)

reader = easyocr.Reader(['en'], gpu=False)
results = reader.readtext(img)

detected_text = ""
if not results:
    print("No text detected.")
else:
    print("Detected text:")
    for _, text, conf in results:
        print(f"'{text}' – confidence: {conf * 100:.2f}%")
        detected_text += text + " "

detected_text = detected_text.strip()

# === Step 3: Send text to second ESP32 and receive echo ===
if detected_text:
    try:
        print(f"\nOpening serial port: {serial_port}...")
        ser = serial.Serial(serial_port, baud_rate, timeout=2)
        time.sleep(2)  # Wait for ESP32 to reboot if needed

        print(f"Sending to ESP32: {detected_text}")
        ser.write((detected_text + "\n").encode())
        ser.flush()

        # Read echoed response
        echoed_response = ser.readline().decode(errors='ignore').strip()
        print(f"Received from ESP32: {echoed_response}")

        ser.close()
    except Exception as e:
        print(f"Serial communication error: {e}")
else:
    print("No text to send.")
