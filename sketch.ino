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

#define BTN_PWR 13 // NEW: Power Button
#define BTN_A 14
#define BTN_B 15
#define ENC_SW 4
#define ENC_CLK 2
#define ENC_DT 3

// --- STATE MACHINE ENUMS ---
enum State { OFF, BOOT, MENU, TIMER_SELECT, TIMER_RUN, CAMERA, MEDIA };
State currentState = OFF; // System starts in deep sleep

// --- GLOBAL VARIABLES ---
int menuIndex = 0;       
int timerIndex = 0;      
long timerDuration = 0;
unsigned long timerStartMillis = 0;
long lastSecondsRemaining = -1;
int mediaVolume = 50;

// Non-Blocking UI Timers
unsigned long bootTime = 0;
unsigned long uiTimer = 0;
int uiSubState = 0; 

// --- HARDWARE STATE TRACKING ---
bool btnPwr_LastState = false;
bool btnA_LastState = false;
bool btnB_LastState = false;
bool enc_LastState = false;
int lastClkState;

unsigned long btnB_PressTime = 0;
bool btnB_LongPressTriggered = false;

unsigned long btnPwr_PressTime = 0;
bool btnPwr_ActionTriggered = false;

// Input Flags (Fired once per action)
bool btnPwr_ShortPress = false;
bool btnPwr_LongPress = false;
bool btnA_ShortPress = false;
bool btnB_ShortPress = false;
bool enc_ShortPress = false;
int enc_Rotated = 0; 

void setup() {
  Serial.begin(115200);

  pinMode(BTN_PWR, INPUT_PULLUP);
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);
  pinMode(ENC_CLK, INPUT);
  pinMode(ENC_DT, INPUT);

  tft.begin();
  tft.setRotation(0); 
  tft.fillScreen(ILI9341_BLACK); // Start with screen off

  lastClkState = digitalRead(ENC_CLK);
}

void loop() {
  readInputs(); 

  // --- POWER MANAGEMENT LOGIC ---
  if (currentState == OFF) {
    // If asleep, only listen for a power button press to wake up
    if (btnPwr_ShortPress || btnPwr_LongPress) {
      drawBootScreen();
      bootTime = millis();
      currentState = BOOT;
    }
    return; // Block all other processing while OFF
  } 
  else {
    // If awake, a long press on the power button puts it to sleep
    if (btnPwr_LongPress) {
      tft.fillScreen(ILI9341_BLACK); // Kill screen
      currentState = OFF;
      return; 
    }
  }

  // --- FINITE STATE MACHINE (FSM) ---
  switch (currentState) {
    case BOOT:
      if (millis() - bootTime > 1500) { 
        currentState = MENU;
        drawMenu();
      }
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
      
    case OFF:
      // Handled above
      break;
  }
}

// ==========================================
// ZERO-LAG INPUT ENGINE
// ==========================================
void readInputs() {
  btnPwr_ShortPress = false;
  btnPwr_LongPress = false;
  btnA_ShortPress = false;
  btnB_ShortPress = false;
  enc_ShortPress = false;
  enc_Rotated = 0;

  // 0. Power Button (Short Wake vs 1s Hold Sleep)
  bool currentPwr = (digitalRead(BTN_PWR) == LOW);
  if (currentPwr && !btnPwr_LastState) { 
    btnPwr_PressTime = millis();
    btnPwr_ActionTriggered = false;
  } 
  else if (currentPwr && btnPwr_LastState) { 
    if (!btnPwr_ActionTriggered && (millis() - btnPwr_PressTime > 1000)) {
      btnPwr_ActionTriggered = true; 
      btnPwr_LongPress = true; // Triggers Sleep
    }
  } 
  else if (!currentPwr && btnPwr_LastState) { 
    if (!btnPwr_ActionTriggered) {
      btnPwr_ShortPress = true; // Triggers Wake
    }
  }
  btnPwr_LastState = currentPwr;

  // If system is OFF, don't bother reading the rest of the inputs
  if (currentState == OFF) return;

  // 1. Button A (Triggers instantly on press)
  bool currentA = (digitalRead(BTN_A) == LOW);
  if (currentA && !btnA_LastState) btnA_ShortPress = true;
  btnA_LastState = currentA;

  // 2. Encoder Button (Triggers instantly on press)
  bool currentEnc = (digitalRead(ENC_SW) == LOW);
  if (currentEnc && !enc_LastState) enc_ShortPress = true;
  enc_LastState = currentEnc;

  // 3. Button B (Short Press vs 1.5s Long Press Override)
  bool currentB = (digitalRead(BTN_B) == LOW);
  
  if (currentB && !btnB_LastState) { 
    btnB_PressTime = millis();
    btnB_LongPressTriggered = false;
  } 
  else if (currentB && btnB_LastState) { 
    if (!btnB_LongPressTriggered && (millis() - btnB_PressTime > 1500)) {
      btnB_LongPressTriggered = true; 
      if (currentState != BOOT && currentState != MENU) {
        currentState = MENU;
        drawMenu();
        uiSubState = 0; 
      }
    }
  } 
  else if (!currentB && btnB_LastState) { 
    if (!btnB_LongPressTriggered) {
      btnB_ShortPress = true; 
    }
  }
  btnB_LastState = currentB;

  // 4. Rotary Encoder Dial
  int clkState = digitalRead(ENC_CLK);
  if (clkState != lastClkState && clkState == HIGH) {
    if (digitalRead(ENC_DT) != clkState) enc_Rotated = 1;  
    else enc_Rotated = -1;                                 
  }
  lastClkState = clkState;
}

// ==========================================
// UI DRAWING & LOGIC FUNCTIONS
// ==========================================

void drawBootScreen() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(3);
  tft.setCursor(20, 140);
  tft.print("J.A.R.V.I.S.");
}

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
  
  tft.setCursor(30, 70 + (oldIdx * 40));
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.print("  " + apps[oldIdx] + "  ");
  
  tft.setCursor(30, 70 + (newIdx * 40));
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.print("> " + apps[newIdx] + "  ");
}

void handleMenuLogic() {
  if (btnA_ShortPress) { 
    int oldIdx = menuIndex;
    menuIndex--;
    if (menuIndex < 0) menuIndex = 2;
    updateMenuCursor(oldIdx, menuIndex);
  }
  
  if (btnB_ShortPress) { 
    int oldIdx = menuIndex;
    menuIndex++;
    if (menuIndex > 2) menuIndex = 0;
    updateMenuCursor(oldIdx, menuIndex);
  }
  
  if (enc_ShortPress) { 
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
  if (uiSubState == 0) { 
    if (enc_ShortPress || btnA_ShortPress) {
      tft.fillScreen(ILI9341_WHITE);
      uiTimer = millis();
      uiSubState = 1; 
    }
  } 
  else if (uiSubState == 1) { 
    if (millis() - uiTimer > 50) {
      tft.fillScreen(ILI9341_BLACK);
      tft.setTextColor(ILI9341_GREEN);
      tft.setTextSize(2);
      tft.setCursor(20, 140);
      tft.print("[ CAPTURED ]");
      tft.setCursor(20, 170);
      tft.print("SYNCING...");
      uiTimer = millis();
      uiSubState = 2; 
    }
  } 
  else if (uiSubState == 2) { 
    if (millis() - uiTimer > 750) {
      drawCameraReady(); 
      uiSubState = 0; 
    }
  }
}

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
    uiTimer = millis();
    uiSubState = 1;
  }
  
  if (btnB_ShortPress) {
    tft.fillRect(10, 120, 200, 30, ILI9341_BLACK);
    tft.setCursor(10, 120);
    tft.setTextColor(ILI9341_YELLOW);
    tft.print("<- PREV TRACK");
    uiTimer = millis();
    uiSubState = 1;
  }

  if (uiSubState == 1 && (millis() - uiTimer > 300)) {
    tft.fillRect(10, 120, 200, 30, ILI9341_BLACK);
    uiSubState = 0;
  }

  if (enc_Rotated != 0) {
    if (enc_Rotated == 1) mediaVolume += 5;
    else mediaVolume -= 5;
    
    if (mediaVolume > 100) mediaVolume = 100;
    if (mediaVolume < 0) mediaVolume = 0;
    drawMediaVolume();
  }
}