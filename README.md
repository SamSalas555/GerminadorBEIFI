# GerminadorBEIFI
Sistema de germinacion basado el ESP32 con el IDE de arduino, ademas api correspondiente para poder comunicar multiples modulos del sistema
## Uso del proyecto

Clona mi proyecto a tu entorno con el siguiente comando

```bash
  git clone git@github.com:SamSalas555/GerminadorBEIFI.git
```
Dento de la carpeta de germinador encontraras el codigo para el IDE de arduno para poder usarlo deberas instalar las librerias ´Fuzzy.h´ y la libreia ´SHT1x-ESP.h´ en tu ide de arduno de lo contrario marcara errores al momento de realizar la programacion.
## Uso de la API
Para poder usar la API es necesario tener instalado flask en tu entorno virtual de python, despues de eso en la carpeta de API se puede crear un servidor de debugging del mismo con el comando 
```bash
  python3 main.py
```
## End Points
Los endpoints de esta API marcaran su funcionamiento asi que a continuacion se describen cada uno de estos para su correcto funcionamiento.
### /recievedata
Dentro de este endpoint se recibira mediante el metodo POST datos provenientes del germinador y mediante la API seran guardados en un archivo .json para posteriormente ser graficados.
### /plot/<variable>
dentro de este endpoint podremos graficar los datos recibidos por el germinador y mostrarlos como una imagen 
