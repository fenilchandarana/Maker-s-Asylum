import cv2
import easyocr
import os

# Automatically use the image in the same folder as this script
script_dir = os.path.dirname(os.path.abspath(__file__))
image_path = os.path.join(script_dir, "photo.jpg")

# Load image
img = cv2.imread(image_path)

# Initialize EasyOCR
reader = easyocr.Reader(['en'], gpu=False)

# Run OCR
text = reader.readtext(img)

# Print results
for bbox, detected_text, confidence in text:
    print(f"'{detected_text}' – confidence: {confidence * 100:.2f}%")
