#include <GxEPD2_BW.h>
#include <SD.h>
#include <SPI.h>

// Pin definitions for XIAO ESP32-C3
#define SCREEN_CS   1
#define SCREEN_DC   3
#define SCREEN_RST  4
#define SCREEN_BUSY 5
#define SD_CS       2
#define BTN_UP      0
#define BTN_DOWN    7

// Initialize the 2.13" display (standard 250x122 resolution)
GxEPD2_BW<GxEPD2_213_BN, GxEPD2_213_BN::HEIGHT> display(GxEPD2_213_BN(SCREEN_CS, SCREEN_DC, SCREEN_RST, SCREEN_BUSY));

File myBook;
String currentLine = "";

void setup() {
  Serial.begin(115200);

  // Set up buttons with internal pullups
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);

  // Start the screen
  display.init();
  display.setRotation(1); // Landscape mode
  display.setTextColor(GxEPD_BLACK);
  display.setFont(NULL); // Uses default system font

  // Start the SD card
  if (!SD.begin(SD_CS)) {
    showError("SD Card Failed");
    return;
  }

  // Open a test file
  myBook = SD.open("/book.txt");
  if (!myBook) {
    showError("No book.txt found");
  } else {
    displayPage();
  }
}

void loop() {
  // Check for page down button
  if (digitalRead(BTN_DOWN) == LOW) {
    delay(200); // Simple debounce
    displayPage();
  }

  // If both buttons are held, you could add code here to return to a menu
  if (digitalRead(BTN_UP) == LOW && digitalRead(BTN_DOWN) == LOW) {
    showError("Returning to Menu...");
    delay(1000);
  }
}

void displayPage() {
  display.fillScreen(GxEPD_WHITE);
  display.setCursor(5, 10);
  
  // Read a chunk of text from the SD card
  for (int i = 0; i < 10; i++) { // Show 10 lines per page
    if (myBook.available()) {
      String line = myBook.readStringUntil('\n');
      display.println(line);
    }
  }

  display.display(); // Push to the E-Ink screen
}

void showError(String msg) {
  display.clearScreen();
  display.setCursor(10, 50);
  display.print(msg);
  display.display();
}