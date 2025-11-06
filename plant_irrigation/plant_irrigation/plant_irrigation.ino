// 🌿 Smart Plant Watering System (Auto + Manual via Blynk IoT)

// 👉 Must match your Blynk template
#define BLYNK_TEMPLATE_ID "TMPL3DD4wEhC7"
#define BLYNK_TEMPLATE_NAME "Irrigation"
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Your Blynk credentials
char auth[] = "DXreITrSyMFbMdPgTM07uawcaBbxO7Od"; // Auth token
char ssid[] = "motorola edge 50 pro_2224";        // WiFi name
char pass[] = "Lani.916";                         // WiFi password

// Pins
#define sensor A0
#define waterPump D3

BlynkTimer timer;

// Manual override flags
bool manualOverride = false; // true if user manually toggled pump
bool manualState = 0;        // last manual state of pump

void setup() {
  Serial.begin(9600);
  pinMode(waterPump, OUTPUT);
  digitalWrite(waterPump, HIGH); // Pump OFF initially

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  Serial.println("🌱 System Starting...");
  delay(2000);

  // Soil moisture reading every 1 second
  timer.setInterval(1000L, soilMoistureSensor);
}

// 🔹 Manual button (V1) - overrides auto mode
BLYNK_WRITE(V1) {
  manualState = param.asInt();
  manualOverride = true; // manual control activated

  if (manualState == 1) {
    digitalWrite(waterPump, LOW); // Pump ON
    Serial.println("🟢 Manual: Pump ON");
  } else {
    digitalWrite(waterPump, HIGH); // Pump OFF
    Serial.println("🔴 Manual: Pump OFF");
  }
}

// 🔹 Optional: Reset auto mode button (V2)
BLYNK_WRITE(V2) {
  bool resetAuto = param.asInt();
  if (resetAuto) {
    manualOverride = false; // re-enable auto mode
    Serial.println("♻️ Auto mode re-enabled by user");
  }
}

// 🌾 Soil moisture reading + auto control
void soilMoistureSensor() {
  int value = analogRead(sensor);
  value = map(value, 0, 1024, 0, 100);
  value = (value - 100) * -1; // convert to 0 = dry, 100 = wet

  Blynk.virtualWrite(V0, value); // send moisture to app
  Serial.print("🌡️ Moisture: ");
  Serial.println(value);

  // Auto control only if manual override is not active
  if (!manualOverride) {
    if (value >= 1 && value <= 50) { // needs watering
      digitalWrite(waterPump, LOW);
      Serial.println("💧 Auto: Soil 0–55 → Pump ON");
      Blynk.virtualWrite(V1, 1); // update app button
    } else { // >55 → enough moisture
      digitalWrite(waterPump, HIGH);
      Serial.println("☀️ Auto: Soil >55 → Pump OFF");
      Blynk.virtualWrite(V1, 0); // update app button
    }
  }
}

void loop() {
  Blynk.run();
  timer.run();
}
