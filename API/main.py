from flask import Flask, render_template, jsonify
import requests
import threading
import time
from collections import defaultdict
from datetime import datetime

app = Flask(__name__)

devices = [
    {"name": "ESP32_1", "ip": "ip", "port": 8080},
    {"name": "ESP32_2", "ip": "ip", "port": 8080}
]

data = defaultdict(list)

def fetch_data(device):
    url = f"http://{device['ip']}:{device['port']}/status"
    while True:
        try:
            response = requests.get(url)
            if response.status_code == 200:
                json_data = response.json()
                json_data['timestamp'] = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                data[device['name']].append(json_data)
                print(f"Fetched data from {device['name']}: {json_data}")
            else:
                print(f"Failed to fetch data from {device['name']}: {response.status_code}")
        except requests.RequestException as e:
            print(f"Error fetching data from {device['name']}: {e}")
        time.sleep(60)

@app.route('/')
def index():
    return jsonify(data)

@app.route('/api/data')
def api_data():
    return jsonify(data)

if __name__ == '__main__':
    threads = []
    for device in devices:
        t = threading.Thread(target=fetch_data, args=(device,))
        t.start()
        threads.append(t)
    app.run(host='0.0.0.0')
