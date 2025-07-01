import requests

esp_ip = "192.168.137.235"
url = f"http://{esp_ip}/photo"

try:
    print(f"Downloading photo from {url}...")
    response = requests.get(url, timeout=5)
    if response.status_code == 200:
        with open("photo.jpg", "wb") as f:
            f.write(response.content)
        print("Photo downloaded and saved as photo.jpg")
    else:
        print(f"Failed to download. HTTP status code: {response.status_code}")
except requests.exceptions.RequestException as e:
    print(f"Request failed: {e}")
