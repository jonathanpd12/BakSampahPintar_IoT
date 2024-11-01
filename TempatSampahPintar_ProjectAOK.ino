#define BLYNK_TEMPLATE_ID "TMPL6q0tYkRGs"   // Blynk Template ID
#define BLYNK_TEMPLATE_NAME "Project AOK"   // Blynk Template Name
#define BLYNK_AUTH_TOKEN "ArZi77LupyLwnASRB8I0Bxg3IEwRFhPb" // Blynk Authentication Token

#define BLYNK_PRINT Serial      // Agar bisa menampilkan pesan debug pada Serial Monitor

#include <ESP8266WiFi.h>        // Library untuk menghubungkan NodeMCU (ESP8266) ke jaringan WiFi
#include <BlynkSimpleEsp8266.h> // Library untuk mengintegrasikan NodeMCU dengan platform Blynk
#include <NewPing.h>            // Library untuk mengatur dan membaca data dari sensor ultrasonik HC-SR04
#include <Servo.h>              // Library untuk mengontrol servo motor

    /* Set Pin */
// Sensor Ultrasonic 1
#define TRIGGER_PIN1  D1  // Pin Trigger HC-SR04 Sampah Basah pada NodeMCU
#define ECHO_PIN1     D2  // Pin Echo HC-SR04 Sampah Basah pada NodeMCU
  // Sensor Ultrasonic 2 
#define TRIGGER_PIN2  D5  // Pin Trigger HC-SR04 Sampah Kering pada NodeMCU
#define ECHO_PIN2     D6  // Pin Echo HC-SR04 Sampah Kering pada NodeMCU
  // Maksimum Pembacaan Jarak pada Sensor UltraSonic (cm)
#define MAX_DISTANCE 400 
  // Sensor Soil Moisture 
#define SoilMoisture_PIN A0
  // Servo Motor
#define Servo_PIN     D5
  // Setup Pin HC-SR04 dan Jarak Pembacaan dengan Fungsi Library NewPing
NewPing sonar1(TRIGGER_PIN1, ECHO_PIN1, MAX_DISTANCE);
NewPing sonar2(TRIGGER_PIN2, ECHO_PIN2, MAX_DISTANCE);
  // inisialisasi objek servo
Servo motorServo;

  // Setup WiFi agar NodeMCU bisa terhubung ke WiFi
char ssid[] = "Abcd";       // Nama Wifi 
char pass[] = "28112004";   // Password Wifi
BlynkTimer timer; //  Objek ini akan digunakan untuk menjadwalkan pemanggilan fungsi-fungsi pada interval waktu tertentu

void setup() {
  motorServo.attach(Servo_PIN);   // Menghubungkan Servo Motor 
  motorServo.write(90);           // Standby Servo Motor

  pinMode(SoilMoisture_PIN, INPUT); // Menetapkan PIN ini sebagai input
  pinMode(TRIGGER_PIN1, OUTPUT);    // Menetapkan PIN ini sebagai output 
  pinMode(ECHO_PIN1, INPUT);        // Menetapkan PIN ini sebagai input 
  pinMode(TRIGGER_PIN2, OUTPUT);    // Menetapkan PIN ini sebagai output 
  pinMode(ECHO_PIN2, INPUT);        // Menetapkan PIN ini sebagai input 
  Serial.begin(9600);               // Memulai komunikasi serial dengan kecepatan baud rate 9600
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);  //  Menginisialisasi koneksi ke server Blynk 

  // Menjadwalkan pemanggilan fungsi setiap 2 detik menggunakan objek "timer"
  timer.setInterval(2000L, SensorKelembapanSampah);  
  timer.setInterval(2000L, SensorSampahKering);     
  timer.setInterval(2000L, SensorSampahBasah);      
}

void loop() {
  Blynk.run();
  timer.run();  
}

void SensorKelembapanSampah(){
  // Untuk Mengetahui Kelembapan Sampah yang di deteksi
  int sensorvalue = analogRead(SoilMoisture_PIN); // Kelembapan nya menyesuaikan tempat/ruangan
  Serial.println("\n------------------------------");
  Serial.println("-- Sensor Kelembapan Sampah --");
  Serial.print("Kelembaban Sampah : ");
  Serial.print(sensorvalue);

  if(sensorvalue < 600){
    Serial.println("\nSampah Basah");
    Serial.println("------------------------------\n");
    motorServo.write(0);
    delay(2000); 
    motorServo.write(90); 
  } else if(sensorvalue < 645 && sensorvalue > 589){
      Serial.println("\nSampah Kering");
      Serial.println("------------------------------\n");
      motorServo.write(180);
      delay(2000); 
      motorServo.write(90);
  } else if (sensorvalue > 647) {
      Serial.println("\nTidak Ada Sampah");
      Serial.println("------------------------------\n");
  }
  Blynk.virtualWrite(A0, sensorvalue); // Nilai Sensor kelembapan-nya dikirim ke Pin A0
  delay(1000);                         // agar bisa mengetahui Kelembapan Sampah nya
}

void SensorSampahBasah(){
  // ("-- Monitoring Sensor Bak Sampah 1 --")
  float jarak1 = sonar1.ping_cm(); // Melakukan Pembacaan Jarak dalam bentuk cm
  if (jarak1 > 46) {  
    Serial.println("\n------------------------------");
    Serial.println("-- Sensor Bak Sampah Basah --");
    Serial.print("Jarak (cm)   : ");
    Serial.print(jarak1);
    Serial.print(" CM\n");
    delay(1000); 
  } else if (jarak1 >= 44 && jarak1 < 46)  { 
      Serial.println("------------------------------");
      Serial.println("-- Sensor Bak Sampah Basah --");
      Serial.print("Jarak (cm)   : ");
      Serial.print(jarak1);
      Serial.print(" CM\n");
      Serial.println("Bak Sampah Telah Terisi Setengah");
      Blynk.logEvent("sampahsetengah", "Bak Sampah Basah Telah Terisi Setengah");
      delay(1000); 
  } else if (jarak1 < 30 ){  
      Serial.println("------------------------------");
      Serial.println("-- Sensor Bak Sampah Basah --");
      Serial.print("Jarak (cm)   : ");
      Serial.print(jarak1);
      Serial.print(" CM\n");
      Serial.println("Bak Sampah Telah Terisi Penuh");
      Blynk.logEvent("sampahfull", "Bak Sampah Basah Telah Terisi Penuh");
      delay(1000); 
  }
  Blynk.virtualWrite(V2, jarak1); // Nilai Sensor ultrasonic-nya dikirim ke Pin V2 pada Blynk 
  delay(1000);                    // agar bisa mengetahui kapasitas bak sampah pada Blynk 
}

void SensorSampahKering() {
  // ("-- Monitoring Sensor Bak Sampah 2 --")
  float jarak2 = sonar2.ping_cm(); // Melakukan Pembacaan Jarak dalam bentuk cm
  if (jarak2 > 46) {
    Serial.println("\n-----------------------------");
    Serial.println("-- Sensor Bak Sampah Kering --");
    Serial.print("Jarak (cm)   : ");
    Serial.print(jarak2);
    Serial.print(" CM\n");
  } else if (jarak2 >= 44  && jarak2 < 46)  {
      Serial.println("\n-----------------------------");
      Serial.println("-- Sensor Bak Sampah Kering --");
      Serial.print("Jarak (cm)   : ");
      Serial.print(jarak2);
      Serial.print(" CM\n");
      Serial.println("Bak Sampah Telah Terisi Setengah");
      Blynk.logEvent("sampahsetengah", "Bak Sampah Kering Telah Terisi Setengah");
  } else if (jarak2 < 30) {
      Serial.println("\n-----------------------------");
      Serial.println("-- Sensor Bak Sampah Basah --");
      Serial.print("Jarak (cm)   : ");
      Serial.print(jarak2);
      Serial.print(" CM\n");
      Serial.println("Bak Sampah Telah Terisi Penuh");
      Blynk.logEvent("sampahfull", "Bak Sampah Kering Telah Terisi Penuh");
  }
  Blynk.virtualWrite(V1, jarak2); // Nilai Sensor ultrasonic-nya dikirim ke Pin V1 pada Blynk
  delay(1000);                    // agar bisa mengetahui kapasitas bak sampah pada Blynk
}