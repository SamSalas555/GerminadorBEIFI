from flask import Flask, request, jsonify, render_template, send_file
import json
import matplotlib.pyplot as plt
from io import BytesIO
from datetime import datetime

app = Flask(__name__)

# Almacenar los datos en un diccionario con timestamp
json_file = {}

# Endpoint para recibir datos y almacenarlos en un diccionario con timestamp
@app.route('/recivedata', methods=['POST'])
def receive_data():
    try:
        data = request.get_json()

        # Agregar timestamp a los datos
        data['timestamp'] = datetime.now().strftime('%Y-%m-%d %H:%M:%S')

        # Cargar datos existentes desde el archivo JSON
        try:
            with open('data.json', 'r') as json_file:
                data_dict = json.load(json_file)
        except FileNotFoundError:
            data_dict = {}

        # Agregar datos al diccionario
        for key, value in data.items():
            if key not in data_dict:
                data_dict[key] = []
            data_dict[key].append({'timestamp': data['timestamp'], 'value': value})

        # Guardar los datos en el archivo JSON
        with open('data.json', 'w') as json_file:
            json.dump(data_dict, json_file, indent=4)  # indent=4 para formato legible

        return jsonify({'message': 'Data received and saved successfully'}), 201

    except Exception as e:
        return jsonify({'error': str(e)}), 500


# Endpoint para trazar los datos de una variable específica en función del tiempo
@app.route('/plot/<variable>', methods=['GET'])
def plot_variable(variable):
    try:
        with open('data.json', 'r') as json_file:
            # Cargar los datos del archivo JSON en un diccionario
            data_dict = json.load(json_file)

            # Obtener los datos de la variable específica del diccionario
            if variable not in data_dict:
                return jsonify({'error': f'Data for {variable} not found'}), 404

            data = data_dict[variable]

            # Extraer timestamps y valores para trazar
            timestamps = [entry['timestamp'] for entry in data]
            values = [entry['value'] for entry in data]

            # Convertir timestamps a objetos de datetime para trazar
            timestamps = [datetime.strptime(ts, '%Y-%m-%d %H:%M:%S') for ts in timestamps]

            # Ordenar los datos por timestamp
            sorted_indices = sorted(range(len(timestamps)), key=lambda k: timestamps[k])
            timestamps = [timestamps[i] for i in sorted_indices]
            values = [values[i] for i in sorted_indices]

            # Plotear los datos
            plt.plot(timestamps, values, marker='o')
            plt.xlabel('Timestamp')
            plt.ylabel(variable.capitalize())
            plt.title(f'{variable.capitalize()} Over Time')
            plt.xticks(rotation=45)
            plt.tight_layout()

            # Guardar el gráfico en un objeto BytesIO en formato PNG
            img_stream = BytesIO()
            plt.savefig(img_stream, format='png')
            img_stream.seek(0)
            plt.close()

            # Devolver la imagen como respuesta
            return send_file(img_stream, mimetype='image/png')

    except Exception as e:
        return jsonify({'error': str(e)}), 500

if __name__ == '__main__':
    app.run(debug=True)