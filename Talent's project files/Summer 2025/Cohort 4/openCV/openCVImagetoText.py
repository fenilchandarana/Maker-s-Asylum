import cv2
import easyocr
import matplotlib.pyplot as plt

image_path = 'C:/Users/fenil/Desktop/test.webp'

img = cv2.imread(image_path)

reader = easyocr.Reader(['en'], gpu=False)

text = reader.readtext(img)

for bbox, detected_text, confidence in text:
    print(f"'{detected_text}' – confidence: {confidence * 100:.2f}%")
