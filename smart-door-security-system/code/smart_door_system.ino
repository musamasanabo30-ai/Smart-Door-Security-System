#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>

// === WiFi Credentials ===
const char* ssid = "YOUR_WIFI";         
const char* password = "YOUR_PASSWORD"; 

// === CallMeBot Settings ===
String phoneNumber = "YOUR_NUMBER";  // <-- your WhatsApp number in international format
String apiKey = "YOUR_API_KEY";             // <-- your CallMeBot API key

// === RFID ===
#define RST_PIN 21
#define SS_PIN 5 // SDA for RC522

// === LEDs ===
#define GREEN_LED 2  // Access Granted
#define RED_LED 4    // Access Denied

// === LCD ===
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

// === RFID Object ===
MFRC522 mfrc522(SS_PIN, RST_PIN);

// --------------------------
// Function to send WhatsApp
// --------------------------
void sendWhatsApp(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String encodedMsg = urlencode(message);
    String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber +
                 "&text=" + encodedMsg +
                 "&apikey=" + apiKey;

    http.begin(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode == 200) {
      Serial.println("✅ WhatsApp message sent successfully!");
    } else {
      Serial.print("❌ Error sending message. HTTP code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("⚠ WiFi not connected!");
  }
}

// --------------------------
// URL Encode Helper Function
// --------------------------
String urlencode(String str) {
  String encoded = "";
  char c;
  char code0;
  char code1;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (isalnum(c)) {
      encoded += c;
    } else {
      encoded += '%';
      code0 = (c >> 4) & 0xF;
      code1 = c & 0xF;
      if (code0 > 9) code0 += 'A' - 10;
      else code0 += '0';
      if (code1 > 9) code1 += 'A' - 10;
      else code1 += '0';
      encoded += code0;
      encoded += code1;
    }
  }
  return encoded;
}

void setup() {
  Serial.begin(115200);

  // --- I2C for LCD ---
  Wire.begin(33, 25); // SDA, SCL
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RFID Access Sys");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");

  // --- Connect to Wi-Fi ---
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Wi-Fi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  delay(1000);

  // --- SPI for RC522 ---
  SPI.begin(18, 19, 23, 5); // SCK, MISO, MOSI, SS
  mfrc522.PCD_Init();
  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan your card");

  // --- LEDs setup ---
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);

  Serial.println("System Ready — Scan your RFID tag...");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  // --- Read card UID ---
  Serial.print("Card UID: ");
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  content.toUpperCase();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Card UID:");
  lcd.setCursor(0, 1);
  lcd.print(content.substring(0, 8));

  // --- Access Control ---
  if (content == "459C7406") {  // ✅ Replace with your own UID
    Serial.println("Access Granted");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Granted");

    // Green LED blinks once
    digitalWrite(GREEN_LED, HIGH);
    delay(300);
    digitalWrite(GREEN_LED, LOW);

    delay(1500);
  } else {
    Serial.println("Access Denied");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied");

    // Red LED blinks twice
    for (int i = 0; i < 2; i++) {
      digitalWrite(RED_LED, HIGH);
      delay(200);
      digitalWrite(RED_LED, LOW);
      delay(200);
    }

    // --- Send WhatsApp Alert ---
    String alertMsg = "🚨 ALERT: Unauthorized RFID tag detected! UID: " + content;
    sendWhatsApp(alertMsg);

    delay(1500);
  }

  delay(500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan next card");
  mfrc522.PICC_HaltA();
}