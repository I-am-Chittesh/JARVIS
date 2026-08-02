#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// --- PIN DEFINITIONS ---
#define TFT_CS   17
#define TFT_RST  20
#define TFT_DC   21
#define TFT_MOSI 19
#define TFT_CLK  18
#define TFT_MISO -1

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

#define BTN_A 14
#define BTN_B 15
#define ENC_SW 4
#define ENC_CLK 2
#define ENC_DT 3

// --- STATE MACHINE ENUMS ---
enum State { BOOT, MENU, TIMER_SELECT, TIMER_RUN, CAMERA, MEDIA };
State currentState = BOOT;

// --- GLOBAL VARIABLES ---
int menuIndex = 0;       
int timerIndex = 0;      
long timerDuration = 0;
unsigned long timerStartMillis = 0;
long lastSecondsRemaining = -1;
int mediaVolume = 50;

// --- HARDWARE STATE TRACKING (For Smooth Inputs) ---
bool btnA_LastState = HIGH;
bool btnB_LastState = HIGH;
bool enc_LastState = HIGH;
int lastClkState;

unsigned long btnB_PressTime = 0;
bool btnB_LongPressTriggered = false;

// Input Flags
bool btnA_ShortPress = false;
bool btnB_ShortPress = false;
bool enc_ShortPress = false;

void setup() {
  Serial.begin(115200);

  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);
  pinMode(ENC_CLK, INPUT);
  pinMode(ENC_DT, INPUT);

  tft.begin();
  tft.setRotation(0); 
  tft.fillScreen(ILI9341_BLACK);

  lastClkState = digitalRead(ENC_CLK);
}

void loop() {
  // --- 1. NON-BLOCKING INPUT POLLING & EDGE DETECTION ---
  btnA_ShortPress = false;
  btnB_ShortPress = false;
  enc_ShortPress = false;

  // Button A Edge Detection
  bool btnA_State = digitalRead(BTN_A);
  if (btnA_State == LOW && btnA_LastState == HIGH) {
    btnA_ShortPress = true;
  }
  btnA_LastState = btnA_State;

  // Encoder Switch Edge Detection
  bool enc_State = digitalRead(ENC_SW);
  if (enc_State == LOW && enc_LastState == HIGH) {
    enc_ShortPress = true;
  }
  enc_LastState = enc_State;

  // Button B (Short Press vs 2-Second Long Press)
  bool btnB_State = digitalRead(BTN_B);
  
  if (btnB_State == LOW && btnB_LastState == HIGH) {
    btnB_PressTime = millis();
    btnB_LongPressTriggered = false;
  }
  
  // Check for Long Press (Override to Menu)
  if (btnB_State == LOW && !btnB_LongPressTriggered && (millis() - btnB_PressTime > 2000)) {
    btnB_LongPressTriggered = true;
    if (currentState != BOOT && currentState != MENU) {
      currentState = MENU;
      drawMenu();
      btnB_LastState = btnB_State; 
      return; 
    }
  }
  
  // Register Short Press only upon release if a long press didn't happen
  if (btnB_State == HIGH && btnB_LastState == LOW) {
    if (!btnB_LongPressTriggered) {
      btnB_ShortPress = true; 
    }
  }
  btnB_LastState = btnB_State;

  // --- 2. FINITE STATE MACHINE (FSM) ---
  switch (currentState) {
    case BOOT:
      drawBootScreen();
      delay(1000); // Sped up boot
      currentState = MENU;
      drawMenu();
      break;

    case MENU:
      handleMenuLogic();
      break;

    case TIMER_SELECT:
      handleTimerSelectLogic();
      break;

    case TIMER_RUN:
      handleTimerRunLogic();
      break;

    case CAMERA:
      handleCameraLogic();
      break;

    case MEDIA:
      handleMediaLogic();
      break;
  }
}

// ==========================================
// STATE: BOOT SCREEN
// ==========================================
void drawBootScreen() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(3);
  tft.setCursor(20, 140);
  tft.print("J.A.R.V.I.S.");
}

// ==========================================
// STATE: MAIN MENU (Smooth Updating)
// ==========================================
void drawMenu() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 20);
  tft.print("MAIN MENU");
  tft.drawLine(10, 40, 230, 40, ILI9341_DARKGREY);

  String apps[3] = {"Focus Timer", "Optical Log", "Media Sync"};
  for (int i = 0; i < 3; i++) {
    tft.setCursor(30, 70 + (i * 40));
    if (i == menuIndex) {
      tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
      tft.print("> " + apps[i] + "  ");
    } else {
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      tft.print("  " + apps[i] + "  ");
    }
  }
}

void updateMenuCursor(int oldIdx, int newIdx) {
  String apps[3] = {"Focus Timer", "Optical Log", "Media Sync"};
  tft.setTextSize(2);
  
  // Clear old cursor location
  tft.setCursor(30, 70 + (oldIdx * 40));
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.print("  " + apps[oldIdx] + "  ");
  
  // Draw new cursor location
  tft.setCursor(30, 70 + (newIdx * 40));
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.print("> " + apps[newIdx] + "  ");
}

void handleMenuLogic() {
  if (btnA_ShortPress) { // Move Up
    int oldIdx = menuIndex;
    menuIndex--;
    if (menuIndex < 0) menuIndex = 2;
    updateMenuCursor(oldIdx, menuIndex);
  }
  
  if (btnB_ShortPress) { // Move Down
    int oldIdx = menuIndex;
    menuIndex++;
    if (menuIndex > 2) menuIndex = 0;
    updateMenuCursor(oldIdx, menuIndex);
  }
  
  if (enc_ShortPress) { // Select
    if (menuIndex == 0) {
      currentState = TIMER_SELECT;
      drawTimerSelect();
    } else if (menuIndex == 1) {
      currentState = CAMERA;
      drawCameraReady();
    } else if (menuIndex == 2) {
      currentState = MEDIA;
      drawMediaScreen();
    }
  }
}

// ==========================================
// APP 1: FOCUS TIMER (Smooth Updating)
// ==========================================
void drawTimerSelect() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setCursor(10, 20);
  tft.print("SELECT TIMER");
  tft.drawLine(10, 40, 230, 40, ILI9341_DARKGREY);

  String timers[2] = {"Work (25:00)", "Rest (05:00)"};
  for (int i = 0; i < 2; i++) {
    tft.setCursor(30, 80 + (i * 40));
    if (i == timerIndex) {
      tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
      tft.print("> " + timers[i] + "  ");
    } else {
      tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
      tft.print("  " + timers[i] + "  ");
    }
  }
}

void updateTimerCursor(int oldIdx, int newIdx) {
  String timers[2] = {"Work (25:00)", "Rest (05:00)"};
  tft.setTextSize(2);
  
  tft.setCursor(30, 80 + (oldIdx * 40));
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.print("  " + timers[oldIdx] + "  ");
  
  tft.setCursor(30, 80 + (newIdx * 40));
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.print("> " + timers[newIdx] + "  ");
}

void handleTimerSelectLogic() {
  if (btnA_ShortPress || btnB_ShortPress) {
    int oldIdx = timerIndex;
    timerIndex = !timerIndex; 
    updateTimerCursor(oldIdx, timerIndex);
  }
  
  if (enc_ShortPress) {
    timerDuration = (timerIndex == 0) ? (25 * 60) : (5 * 60);
    timerStartMillis = millis();
    lastSecondsRemaining = -1; 
    currentState = TIMER_RUN;
    tft.fillScreen(ILI9341_BLACK);
  }
}

void handleTimerRunLogic() {
  long elapsedSecs = (millis() - timerStartMillis) / 1000;
  long remainingSecs = timerDuration - elapsedSecs;
  if (remainingSecs < 0) remainingSecs = 0;

  if (remainingSecs != lastSecondsRemaining) {
    lastSecondsRemaining = remainingSecs;
    int mins = remainingSecs / 60;
    int secs = remainingSecs % 60;
    
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft.setTextSize(3);
    tft.setCursor(10, 20);
    tft.print(timerIndex == 0 ? "FOCUS: WORK" : "FOCUS: REST");
    
    tft.setTextSize(6);
    tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
    tft.setCursor(40, 120);
    
    if (mins < 10) tft.print("0");
    tft.print(mins);
    tft.print(":");
    if (secs < 10) tft.print("0");
    tft.print(secs);
  }
}

// ==========================================
// APP 2: OPTICAL LOGGING (CAMERA)
// ==========================================
void drawCameraReady() {
  tft.fillScreen(ILI9341_BLACK);
  tft.drawRect(20, 60, 200, 160, ILI9341_WHITE); 
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(45, 20);
  tft.print("CAMERA READY");
  
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setTextSize(1);
  tft.setCursor(45, 250);
  tft.print("PRESS ENCODER TO SHUTTER");
}

void handleCameraLogic() {
  if (enc_ShortPress || btnA_ShortPress) {
    tft.fillScreen(ILI9341_WHITE);
    delay(50); // Faster shutter flash
    tft.fillScreen(ILI9341_BLACK);
    
    tft.setTextColor(ILI9341_GREEN);
    tft.setTextSize(2);
    tft.setCursor(20, 140);
    tft.print("[ CAPTURED ]");
    tft.setCursor(20, 170);
    tft.print("SYNCING...");
    
    delay(750); // Faster processing delay
    drawCameraReady(); 
  }
}

// ==========================================
// APP 3: MEDIA SYNC
// ==========================================
void drawMediaScreen() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(50, 20);
  tft.print("NOW PLAYING");
  tft.drawLine(10, 40, 230, 40, ILI9341_DARKGREY);
  
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setCursor(10, 80);
  tft.print("Song: Unknown Artist");
  
  drawMediaVolume();
}

void drawMediaVolume() {
  tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setCursor(10, 200);
  tft.print("VOL: ");
  tft.print(mediaVolume);
  tft.print("%  "); 
  
  tft.drawRect(10, 230, 200, 20, ILI9341_WHITE);
  tft.fillRect(11, 231, 198, 18, ILI9341_BLACK); 
  tft.fillRect(11, 231, (mediaVolume * 198) / 100, 18, ILI9341_CYAN); 
}

void handleMediaLogic() {
  if (btnA_ShortPress) {
    tft.fillRect(10, 120, 200, 30, ILI9341_BLACK);
    tft.setCursor(10, 120);
    tft.setTextColor(ILI9341_YELLOW);
    tft.print("-> NEXT TRACK");
    delay(300); // Quick, non-obtrusive flash
    tft.fillRect(10, 120, 200, 30, ILI9341_BLACK);
  }
  
  if (btnB_ShortPress) {
    tft.fillRect(10, 120, 200, 30, ILI9341_BLACK);
    tft.setCursor(10, 120);
    tft.setTextColor(ILI9341_YELLOW);
    tft.print("<- PREV TRACK");
    delay(300);
    tft.fillRect(10, 120, 200, 30, ILI9341_BLACK);
  }

  int clkState = digitalRead(ENC_CLK);
  if (clkState != lastClkState && clkState == HIGH) {
    if (digitalRead(ENC_DT) != clkState) {
      mediaVolume += 5;
    } else {
      mediaVolume -= 5;
    }
    if (mediaVolume > 100) mediaVolume = 100;
    if (mediaVolume < 0) mediaVolume = 0;
    drawMediaVolume();
  }
  lastClkState = clkState;
}