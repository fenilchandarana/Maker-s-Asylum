import requests
import cv2
import easyocr
import os

# === Step 1: Download image from ESP32-CAM ===
esp_ip = "192.168.137.235"  # Change this if your ESP32 has a different IP
url = f"http://{esp_ip}/photo"
photo_path = "photo.jpg"

try:
    print(f"Downloading photo from {url}...")
    response = requests.get(url, timeout=5)
    if response.status_code == 200:
        with open(photo_path, "wb") as f:
            f.write(response.content)
        print(f"Photo downloaded and saved as {photo_path}")
    else:
        print(f"Failed to download. HTTP status code: {response.status_code}")
        exit(1)
except requests.exceptions.RequestException as e:
    print(f"Request failed: {e}")
    exit(1)

# === Step 2: OCR – Convert image to text using EasyOCR ===
print("Running OCR on the downloaded image...")

# Read image using OpenCV
img = cv2.imread(photo_path)

# Initialize EasyOCR
reader = easyocr.Reader(['en'], gpu=False)

# Perform OCR
text_results = reader.readtext(img)

# Print results
if not text_results:
    print("No text detected.")
else:
    print("Detected text:")
    for bbox, detected_text, confidence in text_results:
        print(f"'{detected_text}' – confidence: {confidence * 100:.2f}%")
