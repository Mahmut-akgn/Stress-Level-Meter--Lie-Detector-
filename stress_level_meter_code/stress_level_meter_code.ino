#include <PulseSensorPlayground.h>

const int GSR_PIN = A0;         // GSR sensörünün bağlı olduğu analog pin
const int PULSE_PIN = A1;       // Pulse sensörünün bağlı olduğu analog pin

long gsrSensorValue = 0;        // GSR değerini saklamak için değişken
long pulseSensorValue = 0;      // Pulse sensöründen okunan değer
long totalValue = 0;            // Sensörlerdeki değerleri toplamak için
long readings = 0;              // Sensörde kaç değer okunduğunu gösterir
long baseValue = 0;             // Ortalama değer
long lowThresholdGSR = 0;       // Düşük eşik değeri GSR için
long mediumThresholdGSR = 0;    // Orta eşik değeri GSR için
long highThresholdGSR = 0;      // Yüksek eşik değeri GSR için
long lowThresholdPulse = 0;     // Düşük eşik değeri Pulse için         
long mediumThresholdPulse = 0;  // Orta eşik değeri Pulse için         
long highThresholdPulse = 0;    // Yüksek eşik değeri Pulse için  
bool answerPart = false;        // Cevap verme kısmında mı       

const int PULSE_BLINK = LED_BUILTIN;
const int PULSE_FADE = 5;
const int THRESHOLD = 550;

PulseSensorPlayground pulseSensor;

void setup() {
    Serial.begin(9600);
    pinMode(2, OUTPUT);     // Yeşil LED
    pinMode(3, OUTPUT);     // Sarı LED
    pinMode(4, OUTPUT);     // Kırmızı LED
    pinMode(6, OUTPUT);     // Mavi LED
    pinMode(7, INPUT);      // Buton 1
    pinMode(8, INPUT);      // Buton 2

    pulseSensor.analogInput(PULSE_PIN);
    pulseSensor.blinkOnPulse(PULSE_BLINK);
    pulseSensor.fadeOnPulse(PULSE_FADE);

    pulseSensor.setSerial(Serial);
    pulseSensor.setThreshold(THRESHOLD);

    // PulseSensor başlatma işlemi kontrol ediliyor
    if (!pulseSensor.begin()) {
        while (true) {
            digitalWrite(PULSE_BLINK, LOW);
            delay(50); 
            Serial.println('!');
            digitalWrite(PULSE_BLINK, HIGH);
            delay(50);
        }
    }
}

void loop() {
    if (digitalRead(7) == 1) { 
        calibrateSensors();
    }

    if (digitalRead(8) == 1) {
        answerPart = true;
    } else {
        answerPart = false;
    }

    controlLEDs();

    sendToSerialPlotter();

    delay(20);
}

void calibrateSensors() {

    // ledleri söndür
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(6, LOW);


    Serial.println("Kalibrasyon başladı...");
    unsigned long startTime = millis();
    totalValue = 0;
    readings = 0;
    baseValue = 0;

    while (millis() - startTime < 5000) {
        gsrSensorValue = analogRead(GSR_PIN);  // GSR sensöründen değeri oku
        totalValue += gsrSensorValue;
        readings++;
        Serial.println(gsrSensorValue);
        delay(50);  // 20ms gecikme
    }

    baseValue = totalValue / readings; // GSR sensörü için ortalama değeri hesapla

    lowThresholdGSR = baseValue + 2; 
    mediumThresholdGSR = baseValue + 4;
    highThresholdGSR = baseValue + 7;

    startTime = millis(); 
    totalValue = 0; 
    readings = 0; 
    baseValue = 0; 

    while (millis() - startTime < 5000) {
        pulseSensorValue = pulseSensor.getBeatsPerMinute(); // Pulse sensöründen değeri oku
        totalValue += pulseSensorValue;
        readings++;
        Serial.println(pulseSensorValue);
        delay(50); 
    }

    baseValue = totalValue / readings; // Pulse sensörü için ortalama değeri hesapla

    lowThresholdPulse = baseValue + 2; 
    mediumThresholdPulse = baseValue + 4;
    highThresholdPulse = baseValue + 7;

    Serial.println("Kalibrasyon işlemi tamamlandı.");
}

void controlLEDs() {
    gsrSensorValue = analogRead(GSR_PIN);
    pulseSensorValue = pulseSensor.getBeatsPerMinute(); // Pulse sensöründen nabız verisini al

    if(gsrSensorValue > lowThresholdGSR && pulseSensorValue > lowThresholdPulse) {
        digitalWrite(2, HIGH);
    } else {
        digitalWrite(2, LOW);
    }

    if(gsrSensorValue > mediumThresholdGSR && pulseSensorValue > mediumThresholdPulse) {
        digitalWrite(3, HIGH);
    } else {
        digitalWrite(3, LOW);
    }

    if(gsrSensorValue > highThresholdGSR && pulseSensorValue > highThresholdPulse) {
        digitalWrite(4, HIGH);
    } else {
        digitalWrite(4, LOW);
    }

    if(answerPart) {
        digitalWrite(6, HIGH);
    } else {
        digitalWrite(6, LOW);
    }
}

void sendToSerialPlotter() {
    if (answerPart) {
        Serial.print("1");
        Serial.print(",");
    }

    Serial.print(gsrSensorValue);
    Serial.print(",");
    Serial.print(lowThresholdGSR);
    Serial.print(",");
    Serial.print(mediumThresholdGSR);
    Serial.print(",");
    Serial.print(highThresholdGSR);
    Serial.print(",");
    Serial.print(pulseSensorValue);
    Serial.print(",");
    Serial.print(lowThresholdPulse);
    Serial.print(",");
    Serial.print(mediumThresholdPulse);
    Serial.print(",");
    Serial.println(highThresholdPulse);
}
