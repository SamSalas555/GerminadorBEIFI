# GerminadorBEIFI

GerminadorBEIFI es un proyecto diseñado para gestionar y monitorizar las condiciones de crecimiento de las plantas. Este repositorio incluye los componentes de backend y frontend necesarios para controlar el entorno dentro de un germinador.

## Características

- Monitoreo de temperatura y niveles de luz
- Control de sistemas de calefacción e iluminación
- Interfaz web para control y monitoreo remoto
- Registro de datos para análisis histórico

## Dependencias

### Backend

- **Python 3.8+**
- **Flask**: Un micro framework web para Python.
- **Flask-CORS**: Extensión de Flask para permitir peticiones de origen cruzado.

### Frontend

- **HTML/CSS/JavaScript**: Para la creación de la interfaz de usuario.
- **Bootstrap**: Framework CSS para diseño responsivo.

### ESP32

- **Arduino IDE**: Para programar el ESP32.
- **ESPAsyncWebServer**: Librería para crear un servidor web asincrónico en el ESP32.
- **DHT**: Librería para el sensor de temperatura y humedad DHT.

## Instalación

### Backend

1. Clona el repositorio:
    ```bash
    git clone https://github.com/SamSalas555/GerminadorBEIFI.git
    cd GerminadorBEIFI/Germinador
    ```

2. Crea un entorno virtual y actívalo:
    ```bash
    python -m venv venv
    source venv/bin/activate   # En Windows: venv\Scripts\activate
    ```

3. Instala las dependencias:
    ```bash
    pip install -r requirements.txt
    ```

4. Inicia el servidor:
    ```bash
    flask run
    ```

### Frontend

1. Navega al directorio del frontend:
    ```bash
    cd frontend
    ```

2. Abre `index.html` en tu navegador preferido.

### ESP32

1. Abre el archivo `Germinador.ino` en el Arduino IDE.
2. Conecta tu ESP32 a tu computadora.
3. Selecciona la placa correcta y el puerto en el Arduino IDE.
4. Carga el programa en el ESP32.

## Uso

1. Abre tu navegador web y navega a `http://127.0.0.1:5000` para acceder a la interfaz de usuario.
2. Utiliza la interfaz para monitorizar y controlar las condiciones de temperatura y luz del germinador.
3. Revisa los registros históricos para analizar las condiciones de crecimiento.

## Contribuciones

Las contribuciones son bienvenidas. Por favor, abre un issue o crea un pull request para colaborar.

## Licencia

Este proyecto está bajo la Licencia MIT. Consulta el archivo `LICENSE` para más detalles.

---

Esperamos que este proyecto te sea útil para gestionar tus germinadores y mejorar las condiciones de crecimiento de tus plantas.
