/*
 * Project: ESP32 Portable Wi-Fi Spectrum Analyzer
 * Version: 1.0.0 (Stable Release)
 * Platform: ESP32 / Arduino Framework
 * Description: 
 * A handheld network analysis tool utilizing ESP32's Wi-Fi module in Station Mode.
 * Features include real-time RSSI monitoring, channel congestion analysis (graph),
 * and security protocol detection.
 * * Hardware:
 * - ESP32 Development Board (WROOM-32)
 * - 0.96" SSD1306 OLED Display (I2C)
 * - 4x Tactile Buttons (Input Pull-up)
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>

// --- DISPLAY CONFIGURATION ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- PIN DEFINITIONS ---
// Buttons configured as Active LOW (Input Pull-Up)
#define BTN_UP      18 
#define BTN_DOWN    19 
#define BTN_MODE    5  
#define BTN_SELECT  4  

// --- STATE MACHINE ---
enum AppMode {
  MODE_LIST,   // Displays list of available networks
  MODE_DETAIL, // Displays detailed info for specific network
  MODE_GRAPH   // Displays channel congestion histogram
};

AppMode currentMode = MODE_LIST;

// --- GLOBAL VARIABLES ---
int networkCount = 0;
int scrollIndex = 0;    
int selectedLine = 0;   
int detailScrollOffset = 0;    

int channelDistribution[15]; // Stores count of APs per channel (1-14)
unsigned long lastInputTime = 0; // For debounce logic

// Custom Icon: Lock (5x7 px)
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
  
  // Initialize Input Pins
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);

  // Initialize Wi-Fi in Station Mode
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Initialize Display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); 
  }
  
  // Boot Screen
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
  display.println(F("v1.0"));
  display.display();
  delay(2000);
  
  // Initial Scan
  scanNetworks();
}

void loop() {
  unsigned long currentTime = millis();

  switch (currentMode) {
    
    // --- MODE: LIST VIEW ---
    case MODE_LIST:
      // Debounce delay: 200ms
      if (currentTime - lastInputTime > 200) { 
        
        // Navigation: Down
        if (digitalRead(BTN_DOWN) == LOW) {
          if (selectedLine < networkCount - 1) {
            selectedLine++;
            // Scroll logic for 4 visible lines
            if (selectedLine >= scrollIndex + 4) scrollIndex++;
            drawListScreen();
            lastInputTime = currentTime;
          }
        }
        // Navigation: Up
        else if (digitalRead(BTN_UP) == LOW) {
          if (selectedLine > 0) {
            selectedLine--;
            if (selectedLine < scrollIndex) scrollIndex--;
            drawListScreen();
            lastInputTime = currentTime;
          }
        }
        // Action: Select (Go to Detail)
        else if (digitalRead(BTN_SELECT) == LOW) {
          currentMode = MODE_DETAIL;
          detailScrollOffset = 0; 
          drawDetailScreen(selectedLine);
          lastInputTime = currentTime;
        }
        // Action: Mode Switch (Go to Graph)
        else if (digitalRead(BTN_MODE) == LOW) {
          currentMode = MODE_GRAPH;
          drawGraphScreen();
          lastInputTime = currentTime;
        }
      }
      break;

    // --- MODE: DETAIL VIEW ---
    case MODE_DETAIL:
      // Fast Scroll for Detail View (No heavy debounce)
      
      // Scroll Down
      if (digitalRead(BTN_DOWN) == LOW) {
        if (detailScrollOffset > -100) { 
          detailScrollOffset -= 6; 
          drawDetailScreen(selectedLine); 
          delay(20); // Small delay for smooth animation
        }
      }
      // Scroll Up
      else if (digitalRead(BTN_UP) == LOW) {
        if (detailScrollOffset < 0) { 
          detailScrollOffset += 6; 
          if (detailScrollOffset > 0) detailScrollOffset = 0; 
          drawDetailScreen(selectedLine); 
          delay(20);
        }
      }
      // Return to List
      else if (currentTime - lastInputTime > 300) {
         if (digitalRead(BTN_SELECT) == LOW || digitalRead(BTN_MODE) == LOW) {
          currentMode = MODE_LIST;
          drawListScreen();
          lastInputTime = currentTime;
        }
      }
      break;

    // --- MODE: GRAPH VIEW ---
    case MODE_GRAPH:
      if (currentTime - lastInputTime > 200) {
        // Return to List
        if (digitalRead(BTN_MODE) == LOW) { 
          currentMode = MODE_LIST;
          drawListScreen();
          lastInputTime = currentTime;
        }
        // Rescan Network
        else if (digitalRead(BTN_SELECT) == LOW) { 
          scanNetworks();
          currentMode = MODE_GRAPH;
          drawGraphScreen();
          lastInputTime = currentTime;
        }
      }
      break;
  }
}

/**
 * Performs a Wi-Fi scan with optimized parameters for balance
 * between speed and sensitivity.
 */
void scanNetworks() {
  display.clearDisplay();
  display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
  
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(30, 25);
  display.println(F("SCANNING..."));
  
  // Draw Loading Bar Container
  display.drawRect(10, 45, 108, 8, WHITE);
  display.display();

  // Simulated Loading Animation
  for(int i=0; i<80; i+=10) {
      display.fillRect(12, 47, i, 4, WHITE);
      display.display();
      delay(30);
  }
  
  /* * SCAN PARAMETERS:
   * async = false
   * show_hidden = true
   * passive = false
   * time_per_channel = 250ms (Optimized for reliable detection)
   */
  networkCount = WiFi.scanNetworks(false, true, false, 250);
  
  // Fill Loading Bar
  display.fillRect(12, 47, 104, 4, WHITE);
  display.display();
  delay(100); 

  // Analyze Channel Distribution for Graph Mode
  for(int i=0; i<15; i++) channelDistribution[i] = 0;
  for (int i = 0; i < networkCount; i++) {
    int ch = WiFi.channel(i);
    if (ch > 0 && ch < 15) channelDistribution[ch]++;
  }
  
  // Reset navigation
  scrollIndex = 0; 
  selectedLine = 0;
  
  if(currentMode == MODE_GRAPH) drawGraphScreen();
  else drawListScreen();
}

/**
 * Renders the list of networks with RSSI and Security Status.
 * Displays 4 items per page to prevent UI overlap.
 */
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
  
  // Loop through visible lines (Max 4 lines)
  for (int i = scrollIndex; i < scrollIndex + 4; ++i) {
    if (i >= networkCount) break;
    
    // Highlight Selection
    if (i == selectedLine) {
       display.fillRect(0, y-1, 100, 10, WHITE);
       display.setTextColor(BLACK); 
       display.setCursor(2, y); 
       display.print(F(">")); 
    } else {
       display.setTextColor(WHITE); 
       display.setCursor(2, y); 
       display.print(F(" ")); 
    }
    
    // Truncate SSID if too long
    String ssid = WiFi.SSID(i);
    if(ssid.length() > 10) ssid = ssid.substring(0, 10);
    display.print(ssid);
    
    // Draw Lock Icon for Secured Networks
    if (WiFi.encryptionType(i) != WIFI_AUTH_OPEN) {
       int color = (i == selectedLine) ? BLACK : WHITE;
       display.drawBitmap(70, y, lockIcon, 5, 7, color);
    }
    
    // Display RSSI
    display.setTextColor(WHITE); 
    display.setCursor(90, y); 
    display.print(WiFi.RSSI(i));
    y += 10;
  }
  
  // Footer
  display.drawLine(0, 56, SCREEN_WIDTH, 56, WHITE);
  display.setCursor(2, 57);
  display.setTextSize(1);
  display.print(F("K3:Graph  K4:Detail"));
  
  display.display();
}

/**
 * Renders detailed information for a selected network.
 * Supports scrolling for extended data.
 */
void drawDetailScreen(int id) {
  display.clearDisplay();
  int y = 16 + detailScrollOffset;
  
  display.setTextColor(WHITE); 
  display.setTextSize(1);
  
  // SSID
  display.setCursor(0, y); 
  display.print(F("SSID:"));
  display.setCursor(0, y+10); 
  display.println(WiFi.SSID(id));
  
  // BSSID (MAC)
  y += 25;
  display.setCursor(0, y); 
  display.print(F("MAC:"));
  display.setCursor(0, y+10); 
  display.println(WiFi.BSSIDstr(id));
  
  // Channel
  y += 25;
  display.setCursor(0, y);
  display.print(F("CHANNEL: ")); 
  display.print(WiFi.channel(id));
  
  // Signal Strength (RSSI) with Bar
  y += 15;
  display.setCursor(0, y);
  int rssi = WiFi.RSSI(id);
  display.print(F("RSSI: ")); 
  display.print(rssi); 
  display.println(F(" dBm"));
  
  int barWidth = map(rssi, -100, -30, 0, 120);
  if(barWidth < 0) barWidth = 0; 
  if(barWidth > 120) barWidth = 120;
  
  display.drawRect(0, y+10, 122, 6, WHITE);
  display.fillRect(2, y+12, barWidth, 2, WHITE);
  
  // Security Type
  y += 25;
  display.setCursor(0, y); 
  display.print(F("SECURITY:"));
  display.setCursor(0, y+10);
  
  wifi_auth_mode_t type = WiFi.encryptionType(id);
  switch (type) {
    case WIFI_AUTH_OPEN: display.print(F("OPEN (Unsecured)")); break;
    case WIFI_AUTH_WEP:  display.print(F("WEP (Weak)")); break;
    case WIFI_AUTH_WPA_PSK: display.print(F("WPA")); break;
    case WIFI_AUTH_WPA2_PSK: display.print(F("WPA2")); break;
    case WIFI_AUTH_WPA_WPA2_PSK: display.print(F("WPA/WPA2")); break;
    case WIFI_AUTH_WPA3_PSK: display.print(F("WPA3 (Strong)")); break;
    default: display.print(F("UNKNOWN")); break;
  }
  
  // Footer Hint
  y += 25;
  display.setCursor(0, y);
  display.setTextColor(INVERSE); 
  display.print(F(" [SELECT] RETURN "));
  
  // Fixed Header Overlay (Masking)
  display.fillRect(0, 0, SCREEN_WIDTH, 14, BLACK); 
  display.fillRect(0, 0, SCREEN_WIDTH, 12, WHITE); 
  display.setTextColor(BLACK);
  display.setCursor(30, 2); 
  display.print(F("NETWORK DETAILS"));
  display.drawLine(0, 13, SCREEN_WIDTH, 13, WHITE); 

  display.display();
}

/**
 * Renders a histogram of channel congestion.
 */
void drawGraphScreen() {
  display.clearDisplay();
  
  display.setCursor(0, 0); 
  display.println(F("CHANNEL TRAFFIC"));
  display.drawLine(0, 10, SCREEN_WIDTH, 10, WHITE);
  
  // Axes
  display.drawLine(10, 55, SCREEN_WIDTH, 55, WHITE); // X-Axis
  display.drawLine(10, 12, 10, 55, WHITE);           // Y-Axis
  
  // Draw Bars for Channels 1-13
  for (int ch = 1; ch <= 13; ch++) {
    int count = channelDistribution[ch];
    int barHeight = count * 6; // Scale factor
    if (barHeight > 40) barHeight = 40; // Max height cap
    
    int xPos = 12 + (ch * 8); 
    display.fillRect(xPos, 55 - barHeight, 5, barHeight, WHITE);
    
    // Label odd channels only to save space
    if (ch % 2 != 0) { 
      display.setCursor(xPos, 57); 
      display.print(ch); 
    }
  }
  
  display.setCursor(80, 0); 
  display.print(F("[K4]Scan"));
  display.display();
}