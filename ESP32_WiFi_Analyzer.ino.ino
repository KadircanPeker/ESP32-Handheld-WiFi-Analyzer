/*
 * Project: ESP32 Professional Wi-Fi Analyzer
 * Version: 1.0.0 (Stable Release)
 * Hardware: ESP32 + SSD1306 OLED (I2C)
 * Author: Engineering Student
 * License: MIT License
 * Description: 
 * A handheld network analysis tool featuring Real-time RSSI monitoring,
 * Channel Congestion Graph, and Detailed Security Analysis.
 * Optimized for 128x64 OLED displays with Safe Zone rendering.
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>

// --- CONFIGURATION ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

// --- PIN DEFINITIONS ---
#define BTN_UP      18 
#define BTN_DOWN    19 
#define BTN_MODE    5  
#define BTN_SELECT  4  

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- STATE MACHINE ---
enum AppMode {
  MODE_LIST,   
  MODE_DETAIL, 
  MODE_GRAPH   
};

AppMode currentMode = MODE_LIST;

// --- GLOBAL VARIABLES ---
int networkCount = 0;
int scrollIndex = 0;    
int selectedLine = 0;   
int detailScrollOffset = 0;    
unsigned long lastInputTime = 0; 

int channelData[15]; 

// Lock Icon (5x7 px)
const unsigned char lockIcon [] PROGMEM = {
  0x70, 0x50, 0x70, 0x70, 0x70, 0x70, 0x70
};

// --- FUNCTION PROTOTYPES ---
void scanNetworks();
void drawListScreen();
void drawDetailScreen(int networkId);
void drawGraphScreen();

void setup() {
  Serial.begin(115200);
  
  // Input Initialization
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);

  // Network Initialization
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Display Initialization
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); 
  }
  
  // Layout Protection
  display.setTextWrap(false);
  
  // Boot Screen (Professional & Clean)
  display.clearDisplay();
  display.setTextColor(WHITE);
  
  display.setTextSize(1);
  display.setCursor(20, 20);
  display.println(F("ESP32 NETWORK"));
  
  display.setTextSize(2);
  display.setCursor(15, 35);
  display.println(F("ANALYZER"));
  
  display.setTextSize(1);
  display.setCursor(95, 55);
  display.println(F("v1.0")); // Semantic Version
  
  display.display();
  delay(2000);
  
  scanNetworks();
}

void loop() {
  unsigned long currentTime = millis();
  
  // --- STATE 1: LIST MODE ---
  if (currentMode == MODE_LIST) {
    if (currentTime - lastInputTime > 150) { 
      if (digitalRead(BTN_DOWN) == LOW) {
        if (selectedLine < networkCount - 1) {
          selectedLine++;
          if (selectedLine >= scrollIndex + 4) scrollIndex++;
          drawListScreen();
          lastInputTime = currentTime;
        }
      }
      else if (digitalRead(BTN_UP) == LOW) {
        if (selectedLine > 0) {
          selectedLine--;
          if (selectedLine < scrollIndex) scrollIndex--;
          drawListScreen();
          lastInputTime = currentTime;
        }
      }
      else if (digitalRead(BTN_SELECT) == LOW) {
        currentMode = MODE_DETAIL;
        detailScrollOffset = 0; 
        drawDetailScreen(selectedLine);
        lastInputTime = currentTime;
      }
      else if (digitalRead(BTN_MODE) == LOW) {
        currentMode = MODE_GRAPH;
        drawGraphScreen();
        lastInputTime = currentTime;
      }
    }
  }
  
  // --- STATE 2: DETAIL MODE ---
  else if (currentMode == MODE_DETAIL) {
    if (digitalRead(BTN_DOWN) == LOW) {
      if (detailScrollOffset > -100) { 
        detailScrollOffset -= 4; 
        drawDetailScreen(selectedLine);
        delay(10); 
      }
    }
    else if (digitalRead(BTN_UP) == LOW) {
      if (detailScrollOffset < 0) {
        detailScrollOffset += 4; 
        if (detailScrollOffset > 0) detailScrollOffset = 0; 
        drawDetailScreen(selectedLine);
        delay(10);
      }
    }
    else if (currentTime - lastInputTime > 300) {
       if (digitalRead(BTN_SELECT) == LOW || digitalRead(BTN_MODE) == LOW) {
        currentMode = MODE_LIST;
        drawListScreen();
        lastInputTime = currentTime;
      }
    }
  }

  // --- STATE 3: GRAPH MODE ---
  else if (currentMode == MODE_GRAPH) {
    if (currentTime - lastInputTime > 200) {
      if (digitalRead(BTN_MODE) == LOW) {
        currentMode = MODE_LIST;
        drawListScreen();
        lastInputTime = currentTime;
      }
      else if (digitalRead(BTN_SELECT) == LOW) {
        scanNetworks();
        currentMode = MODE_GRAPH;
        drawGraphScreen();
        lastInputTime = currentTime;
      }
    }
  }
}

// --- CORE FUNCTIONS ---

void scanNetworks() {
  display.clearDisplay();
  display.fillRect(0,0,128,64,BLACK);
  
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(30,30); 
  display.println(F("SCANNING..."));
  
  // Loading Bar Container
  display.drawRect(14, 45, 100, 6, WHITE);
  display.display();
  
  // Animation START (Simulate activity)
  for(int i=0; i<80; i+=5) {
      display.fillRect(16, 47, i, 2, WHITE); 
      display.display();
      delay(10);
  }
  
  // SCAN (Blocking Operation - 200ms/ch)
  networkCount = WiFi.scanNetworks(false, true, false, 200);
  
  // Animation END (Complete)
  display.fillRect(16, 47, 96, 2, WHITE);
  display.display();
  delay(100);

  // Data Processing for Graph
  for(int i=0; i<15; i++) channelData[i] = 0;
  for (int i = 0; i < networkCount; i++) {
    int ch = WiFi.channel(i);
    if (ch > 0 && ch < 15) channelData[ch]++;
  }
  
  scrollIndex = 0; 
  selectedLine = 0;
  
  if(currentMode == MODE_GRAPH) drawGraphScreen();
  else drawListScreen();
}

void drawListScreen() {
  display.clearDisplay();
  
  // Header
  display.fillRect(0, 0, SCREEN_WIDTH, 12, WHITE);
  display.setTextColor(BLACK); 
  display.setCursor(2, 2);
  display.print(F("NETWORKS (")); 
  display.print(networkCount); 
  display.print(F(")"));
  display.setTextColor(WHITE);

  int y = 16;
  for (int i = scrollIndex; i < scrollIndex + 4; ++i) {
    if (i >= networkCount) break;
    
    // Selection Bar
    if (i == selectedLine) {
       display.fillRect(0, y-1, 120, 10, WHITE); 
       display.setTextColor(BLACK); 
       display.setCursor(2, y); 
       display.print(F(">")); 
    } else {
       display.setTextColor(WHITE); 
       display.setCursor(2, y); 
       display.print(F(" ")); 
    }
    
    // SSID Logic
    String ssid = WiFi.SSID(i);
    if (ssid.length() == 0) ssid = F("<?>"); 
    if (ssid.length() > 14) ssid = ssid.substring(0, 14); 
    display.print(ssid);
    
    // Lock Icon
    if (WiFi.encryptionType(i) != WIFI_AUTH_OPEN) {
       int color = (i == selectedLine) ? BLACK : WHITE;
       display.drawBitmap(92, y, lockIcon, 5, 7, color);
    }
    
    // RSSI
    display.setTextColor(WHITE); 
    if (i == selectedLine) display.setTextColor(BLACK); 
    display.setCursor(102, y); 
    display.print(WiFi.RSSI(i));
    y += 10;
  }
  
  // Footer (Safe Zone: Y=54)
  display.drawLine(0, 54, 128, 54, WHITE);
  display.setCursor(2, 55);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.print(F("K3:GRAPH  K4:VIEW"));
  
  display.display();
}

void drawDetailScreen(int id) {
  display.clearDisplay();
  int y = 16 + detailScrollOffset;
  
  display.setTextColor(WHITE); 
  display.setTextSize(1);
  
  String ssid = WiFi.SSID(id);
  if (ssid.length() == 0) ssid = F("<HIDDEN>");
  
  // SSID
  display.setCursor(0, y); display.print(F("SSID:"));
  display.setCursor(0, y+10); display.println(ssid);
  
  // MAC
  y += 25;
  display.setCursor(0, y); display.print(F("MAC:"));
  display.setCursor(0, y+10); display.println(WiFi.BSSIDstr(id));
  
  // Technical
  y += 25;
  display.setCursor(0, y);
  display.print(F("CH:")); display.print(WiFi.channel(id));
  display.print(F("  ")); 
  display.print(WiFi.RSSI(id)); display.println(F("dBm")); 

  // Signal Bar
  y += 15;
  int rssi = WiFi.RSSI(id);
  int barW = map(rssi, -100, -30, 0, 120);
  if(barW<0) barW=0; if(barW>120) barW=120;
  display.drawRect(0, y, 122, 4, WHITE);
  display.fillRect(2, y+1, barW, 2, WHITE);
  
  // Security
  y += 15;
  display.setCursor(0, y); display.print(F("SECURITY: "));
  wifi_auth_mode_t type = WiFi.encryptionType(id);
  switch (type) {
    case WIFI_AUTH_OPEN: display.print(F("OPEN")); break;
    case WIFI_AUTH_WEP:  display.print(F("WEP")); break;
    case WIFI_AUTH_WPA_PSK: display.print(F("WPA")); break;
    case WIFI_AUTH_WPA2_PSK: display.print(F("WPA2")); break;
    case WIFI_AUTH_WPA_WPA2_PSK: display.print(F("WPA/WPA2")); break;
    case WIFI_AUTH_WPA3_PSK: display.print(F("WPA3")); break;
    default: display.print(F("UNKNOWN")); break;
  }
  
  // Footer
  y += 20;
  display.setCursor(0, y);
  display.setTextColor(INVERSE); display.print(F(" [#] BACK "));
  
  // Header Masking
  display.fillRect(0, 0, 128, 14, BLACK); 
  display.fillRect(0, 0, 128, 12, WHITE); 
  display.setTextColor(BLACK);
  display.setCursor(40, 2); display.print(F("NET INFO")); 
  display.drawLine(0, 13, 128, 13, WHITE); 

  display.display();
}

void drawGraphScreen() {
  display.clearDisplay();
  
  display.setCursor(0, 0); display.println(F("SPECTRUM"));
  display.drawLine(0, 10, 128, 10, WHITE);
  
  // Graph Axes (Safe Zone)
  display.drawLine(10, 53, 128, 53, WHITE); 
  display.drawLine(10, 12, 10, 53, WHITE);  
  
  for (int ch = 1; ch <= 13; ch++) {
    int count = channelData[ch];
    int barHeight = count * 6; 
    if (barHeight > 40) barHeight = 40; 
    
    int xPos = 12 + (ch * 8); 
    display.fillRect(xPos, 53 - barHeight, 5, barHeight, WHITE);
    
    if (ch % 2 != 0) { 
      display.setCursor(xPos, 55); 
      display.print(ch); 
    }
  }
  
  display.setCursor(80, 0); display.print(F("[K4]Scan"));
  display.display();
}
