  #define BLYNK_TEMPLATE_ID "TMPL36HJwYXLn"
  #define BLYNK_TEMPLATE_NAME "precision Agirculture node with energy harvesting"
  #define BLYNK_DEVICE_NAME "precision Agirculture node with energy harvesting"
  #define BLYNK_AUTH_TOKEN "je01p1xt3_TRXbLjlKbOQCtg7pMAeZ9S"

  #include <ESP8266WiFi.h>
  #include <BlynkSimpleEsp8266.h>
  #include <DHT.h>

  char ssid[] = "laptop";
  char pass[] = "123456789";

 

  #define DHTPIN D5
  #define DHTTYPE DHT11
  DHT dht(DHTPIN, DHTTYPE);

  #define SOIL_PIN D6  
  #define LDR_PIN D1    

  BlynkTimer timer;


  int soilValue = 50;   
  int lightValue = 50; 

  bool soilAlert = false;
  bool tempAlert = false;
  bool humAlert = false;
  bool lightAlert = false;


  void sendData()
  {
    int soilState = digitalRead(SOIL_PIN);
    int ldrState = digitalRead(LDR_PIN);

    if (soilState == 0) {
      soilValue = min(soilValue + 5, 100);
    } else {
      soilValue = max(soilValue - 5, 0);
    }

    if (ldrState == 0) {   
      lightValue = min(lightValue + 10, 100);
    } else {               
      lightValue = max(lightValue - 10, 0);
    }

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (isnan(temp) || isnan(hum)) {
      Serial.println("DHT ERROR");
      return;
    }

    Serial.printf("Soil:%d%% | Temp:%.2f°C | Hum:%.2f%% | Light:%d%%\n",
                  soilValue, temp, hum, lightValue);

    if (Blynk.connected()) {
      Blynk.virtualWrite(V0, soilValue);
      Blynk.virtualWrite(V1, temp);
      Blynk.virtualWrite(V2, hum);
      Blynk.virtualWrite(V3, lightValue);
    }


    if (soilValue < 30 && !soilAlert) {
      Blynk.logEvent("low_moisture", "⚠ Soil is dry! Water needed.");
      soilAlert = true;
    }
    if (soilValue > 40) soilAlert = false;

    if (temp > 35 && !tempAlert) {
      Blynk.logEvent("high_temp", "🌡 High temperature detected!");
      tempAlert = true;
    }
    if (temp < 33) tempAlert = false;

    if (hum < 40 && !humAlert) {
      Blynk.logEvent("low_humidity", "💧 Humidity is low!");
      humAlert = true;
    }
    if (hum > 45) humAlert = false;

    if (ldrState == 0 && !lightAlert) {  
      Blynk.logEvent("high_light", "☀ Bright light detected!");
      lightAlert = true;
    }
    if (ldrState == 1) lightAlert = false;
  }


  void setup()
  {
    Serial.begin(9600);

    pinMode(SOIL_PIN, INPUT);
    pinMode(LDR_PIN, INPUT);

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

    dht.begin();

    Blynk.syncAll();

    timer.setInterval(5000L, sendData);
  }


  void loop()
  {
    Blynk.run();
    timer.run();
  }