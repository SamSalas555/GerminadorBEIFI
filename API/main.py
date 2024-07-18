from flask import Flask, render_template, jsonify
import requests
import json
import threading
import time
from apscheduler.schedulers.background import BackgroundScheduler
from flask_socketio import SocketIO
import matplotlib.pyplot as plt
import io
import base64
from datetime import datetime

app = Flask(__name__)
socketio = SocketIO(app)
scheduler = BackgroundScheduler()

# Diccionario para almacenar datos de múltiples ESP32
data = {}

# Lista de direcciones IP de los ESP32
esp32_ips = ["http://esp32_1_ip/status", "http://esp32_2_ip/status"]

def fetch_data():
    for ip in esp32_ips:
        try:
            response = requests.get(ip)
            if response.status_code == 200:
                json_data = response.json()
                timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                if ip not in data:
                    data[ip] = []
                data[ip].append({
                    "timestamp": timestamp,
                    "temperature": json_data["temperature"],
                    "humidity": json_data["humidity"],
                    "velocity": json_data["velocity"],
                    "capacity": json_data["capacity"]
                })
                # Emitir datos a los clientes conectados
                socketio.emit('new_data', {"ip": ip, "data": data[ip][-1]})
        except Exception as e:
            print(f"Error fetching data from {ip}: {e}")

scheduler.add_job(fetch_data, 'interval', minutes=1)
scheduler.start()

@app.route('/')
def index():
    return render_template('index.html', data=data)

@app.route('/data/<ip>')
def get_data(ip):
    if ip in data:
        return jsonify(data[ip])
    else:
        return jsonify([])

@app.route('/plot/<ip>')
def plot_data(ip):
    if ip in data:
        timestamps = [entry['timestamp'] for entry in data[ip]]
        temperatures = [entry['temperature'] for entry in data[ip]]
        humidities = [entry['humidity'] for entry in data[ip]]
        velocities = [entry['velocity'] for entry in data[ip]]
        capacities = [entry['capacity'] for entry in data[ip]]

        plt.figure(figsize=(10, 5))
        
        plt.subplot(2, 2, 1)
        plt.plot(timestamps, temperatures, label='Temperature (C)')
        plt.xticks(rotation=45)
        plt.legend()

        plt.subplot(2, 2, 2)
        plt.plot(timestamps, humidities, label='Humidity (%)')
        plt.xticks(rotation=45)
        plt.legend()

        plt.subplot(2, 2, 3)
        plt.plot(timestamps, velocities, label='Velocity')
        plt.xticks(rotation=45)
        plt.legend()

        plt.subplot(2, 2, 4)
        plt.plot(timestamps, capacities, label='Capacity (%)')
        plt.xticks(rotation=45)
        plt.legend()

        plt.tight_layout()

        img = io.BytesIO()
        plt.savefig(img, format='png')
        img.seek(0)
        plot_url = base64.b64encode(img.getvalue()).decode()

        return f'<img src="data:image/png;base64,{plot_url}" />'
    else:
        return 'No data available for this IP.'

if __name__ == '__main__':
    socketio.run(app, debug=True)
