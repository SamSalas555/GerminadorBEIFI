/*
Autor: Samuel Salas Jimenez
Version: 1.0
Institucion: IPN
Titulo: Sistema de Germinacion basado en SOC ESP32
*/

#include <Fuzzy.h>
#include <SHT1x-ESP.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

// Puertos de conexion de la ESP32
#define dataPin 21
#define clockPin 23

// Instancia del sensor SHT10
SHT1x sht10(dataPin, clockPin, SHT1x::Voltage::DC_3_3v);

// Configuración del WiFi
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Fuzzy
Fuzzy *fuzzy = new Fuzzy();
// Entradas del sistema
// Conjuntos difusos para la temperatura
FuzzySet *cold = new FuzzySet(10, 0, 10, 20);
FuzzySet *normal = new FuzzySet(25, 15, 25, 35);
FuzzySet *hot = new FuzzySet(30, 30, 40, 50);

// Conjuntos difusos para la humedad
FuzzySet *dry = new FuzzySet (20, 0, 20, 40);
FuzzySet *moderate = new FuzzySet (50, 30, 50, 70);
FuzzySet *wet = new FuzzySet (70, 60, 80, 100);

// Salidas del sistema
// Conjuntos difusos para velocidad del motor
FuzzySet *slow = new FuzzySet(20, 10, 10, 20);
FuzzySet *average = new FuzzySet(10, 20, 30, 40);
FuzzySet *fast = new FuzzySet(30, 40, 40, 50);
// Conjuntos difusos para la apertura de las electro válvulas
FuzzySet *low = new FuzzySet(10, 10, 10, 20);
FuzzySet *mid = new FuzzySet(20, 20, 30, 40);
FuzzySet *full = new FuzzySet(30, 40, 40, 50);

AsyncWebServer server(80);

void setup()
{
    Serial.begin(38400); // Se comienza la comunicación serial
    // Mensaje de inicialización en caso de no visualizarse en consola revisar configuración
    Serial.println("Starting up");

    // Conexión WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    // Configuración del servidor
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
        // Lectura de los valores del sensor
        float temperature = sht10.readTemperatureC();
        float humidity = sht10.readHumidity();
        fuzzy->setInput(1, temperature);
        fuzzy->setInput(2, humidity);
        fuzzy->fuzzify();

        float fanVel = fuzzy->defuzzify(1);
        float waterC = fuzzy->defuzzify(2);

        // Formateo de los datos en JSON
        String json;
        DynamicJsonDocument doc(256);
        doc["temperature"] = temperature;
        doc["humidity"] = humidity;
        doc["velocity"] = fanVel;
        doc["capacity"] = waterC;
        doc["timestamp"] = "2023-11-15 09:44:28"; // Puedes actualizar esto con un valor dinámico si tienes un RTC

        serializeJson(doc, json);
        request->send(200, "application/json", json);
    });

    server.begin();

    // Fuzzificación de nuestros conjuntos y creación de reglas
    FuzzyInput *temperature = new FuzzyInput(1);
    temperature->addFuzzySet(cold);
    temperature->addFuzzySet(normal);
    temperature->addFuzzySet(hot);

    FuzzyInput *humidity = new FuzzyInput(2);
    humidity->addFuzzySet(dry);
    humidity->addFuzzySet(moderate);
    humidity->addFuzzySet(wet);

    FuzzyOutput *velocity = new FuzzyOutput(1);
    velocity->addFuzzySet(slow);
    velocity->addFuzzySet(average);
    velocity->addFuzzySet(fast);

    FuzzyOutput *flow = new FuzzyOutput(2);
    flow->addFuzzySet(low);
    flow->addFuzzySet(mid);
    flow->addFuzzySet(full);

    FuzzyRuleAntecedent *temperatureCold = new FuzzyRuleAntecedent();
    temperatureCold->joinSingle(cold);

    FuzzyRuleAntecedent *temperatureNormal = new FuzzyRuleAntecedent();
    temperatureNormal->joinSingle(normal);

    FuzzyRuleAntecedent *temperatureHot = new FuzzyRuleAntecedent();
    temperatureHot->joinSingle(hot);

    FuzzyRuleAntecedent *humidityDry = new FuzzyRuleAntecedent();
    humidityDry->joinSingle(dry);

    FuzzyRuleAntecedent *humidityModerate = new FuzzyRuleAntecedent();
    humidityModerate->joinSingle(moderate);

    FuzzyRuleAntecedent *humidityWet = new FuzzyRuleAntecedent();
    humidityWet->joinSingle(wet);

    // Creando las reglas difusas del sistema
    FuzzyRuleAntecedent *temperatureColdAndHumidityDry = new FuzzyRuleAntecedent();
    temperatureColdAndHumidityDry->joinWithAND(temperatureCold, humidityDry);

    FuzzyRuleAntecedent *temperatureColdAndHumidityModerate = new FuzzyRuleAntecedent();
    temperatureColdAndHumidityModerate->joinWithAND(temperatureCold, humidityModerate);

    FuzzyRuleAntecedent *temperatureColdAndHumidityWet = new FuzzyRuleAntecedent();
    temperatureColdAndHumidityWet->joinWithAND(temperatureCold, humidityWet);

    FuzzyRuleAntecedent *temperatureNormalAndHumidityDry = new FuzzyRuleAntecedent();
    temperatureNormalAndHumidityDry->joinWithAND(temperatureNormal, humidityDry);

    FuzzyRuleAntecedent *temperatureNormalAndHumidityModerate = new FuzzyRuleAntecedent();
    temperatureNormalAndHumidityModerate->joinWithAND(temperatureNormal, humidityModerate);

    FuzzyRuleAntecedent *temperatureNormalAndHumidityWet = new FuzzyRuleAntecedent();
    temperatureNormalAndHumidityWet->joinWithAND(temperatureNormal, humidityWet);

    FuzzyRuleAntecedent *temperatureHotAndHumidityDry = new FuzzyRuleAntecedent();
    temperatureHotAndHumidityDry->joinWithAND(temperatureHot, humidityDry);

    FuzzyRuleAntecedent *temperatureHotAndHumidityModerate = new FuzzyRuleAntecedent();
    temperatureHotAndHumidityModerate->joinWithAND(temperatureHot, humidityModerate);

    FuzzyRuleAntecedent *temperatureHotAndHumidityWet = new FuzzyRuleAntecedent();
    temperatureHotAndHumidityWet->joinWithAND(temperatureHot, humidityWet);

    // Crear los consecuentes difusos para las reglas
    FuzzyRuleConsequent *thenVelocitySlow = new FuzzyRuleConsequent();
    thenVelocitySlow->addOutput(slow);

    FuzzyRuleConsequent *thenVelocityAverage = new FuzzyRuleConsequent();
    thenVelocityAverage->addOutput(average);

    FuzzyRuleConsequent *thenVelocityFast = new FuzzyRuleConsequent();
    thenVelocityFast->addOutput(fast);

    FuzzyRuleConsequent *thenFlowLow = new FuzzyRuleConsequent();
    thenFlowLow->addOutput(low);

    FuzzyRuleConsequent *thenFlowMid = new FuzzyRuleConsequent();
    thenFlowMid->addOutput(mid);

    FuzzyRuleConsequent *thenFlowFull = new FuzzyRuleConsequent();
    thenFlowFull->addOutput(full);

    // Create Fuzzy Rules
    FuzzyRule *rule1 = new FuzzyRule(1, temperatureColdAndHumidityDry, thenVelocitySlow);
    FuzzyRule *rule2 = new FuzzyRule(2, temperatureColdAndHumidityModerate, thenVelocityAverage);
    FuzzyRule *rule3 = new FuzzyRule(3, temperatureColdAndHumidityWet, thenVelocityFast);

    FuzzyRule *rule4 = new FuzzyRule(4, temperatureNormalAndHumidityDry, thenVelocityAverage);
    FuzzyRule *rule5 = new FuzzyRule(5, temperatureNormalAndHumidityModerate, thenVelocityFast);
    FuzzyRule *rule6 = new FuzzyRule(6, temperatureNormalAndHumidityWet, thenVelocityFast);

    FuzzyRule *rule7 = new FuzzyRule(7, temperatureHotAndHumidityDry, thenVelocityFast);
    FuzzyRule *rule8 = new FuzzyRule(8, temperatureHotAndHumidityModerate, thenVelocityFast);
    FuzzyRule *rule9 = new FuzzyRule(9, temperatureHotAndHumidityWet, thenVelocityFast);

    FuzzyRule *rule10 = new FuzzyRule(10, temperatureColdAndHumidityDry, thenFlowLow);
    FuzzyRule *rule11 = new FuzzyRule(11, temperatureColdAndHumidityModerate, thenFlowMid);
    FuzzyRule *rule12 = new FuzzyRule(12, temperatureColdAndHumidityWet, thenFlowFull);

    FuzzyRule *rule13 = new FuzzyRule(13, temperatureNormalAndHumidityDry, thenFlowMid);
    FuzzyRule *rule14 = new FuzzyRule(14, temperatureNormalAndHumidityModerate, thenFlowFull);
    FuzzyRule *rule15 = new FuzzyRule(15, temperatureNormalAndHumidityWet, thenFlowFull);

    FuzzyRule *rule16 = new FuzzyRule(16, temperatureHotAndHumidityDry, thenFlowFull);
    FuzzyRule *rule17 = new FuzzyRule(17, temperatureHotAndHumidityModerate, thenFlowFull);
    FuzzyRule *rule18 = new FuzzyRule(18, temperatureHotAndHumidityWet, thenFlowFull);

    // Add Fuzzy Rules
    fuzzy->addFuzzyRule(rule1);
    fuzzy->addFuzzyRule(rule2);
    fuzzy->addFuzzyRule(rule3);
    fuzzy->addFuzzyRule(rule4);
    fuzzy->addFuzzyRule(rule5);
    fuzzy->addFuzzyRule(rule6);
    fuzzy->addFuzzyRule(rule7);
    fuzzy->addFuzzyRule(rule8);
    fuzzy->addFuzzyRule(rule9);
    fuzzy->addFuzzyRule(rule10);
    fuzzy->addFuzzyRule(rule11);
    fuzzy->addFuzzyRule(rule12);
    fuzzy->addFuzzyRule(rule13);
    fuzzy->addFuzzyRule(rule14);
    fuzzy->addFuzzyRule(rule15);
    fuzzy->addFuzzyRule(rule16);
    fuzzy->addFuzzyRule(rule17);
    fuzzy->addFuzzyRule(rule18);
}

void loop()
{
    float temperature;
    float humidity;

    // Lectura de los valores del sensor
    temperature = sht10.readTemperatureC();
    humidity = sht10.readHumidity();

    fuzzy->setInput(1, temperature);
    fuzzy->setInput(2, humidity);
    fuzzy->fuzzify();

    float fanVel = fuzzy->defuzzify(1);
    float waterC = fuzzy->defuzzify(2);
    // Mandamos los valores al monitor serie para comprobar el funcionamiento
    Serial.print("Temperature: ");
    Serial.print(temperature, DEC);
    Serial.print("C / ");
    Serial.print(humidity);
    Serial.println("%");
    Serial.print("Motor: ");
    Serial.print(fanVel);
    Serial.print("  Caudal: ");
    Serial.print(waterC);
    Serial.println("%");

    delay(2000);
}
