#include <GxEPD2_BW.h>
#include <SD.h>
#include <SPI.h>

#define SCREEN_CS   3
#define SCREEN_DC   5
#define SCREEN_RST  6
#define SCREEN_BUSY 7
#define SD_CS       4
#define SPI_MOSI    10
#define SPI_MISO    9
#define SPI_SCK     8
#define BTN_UP      2
#define BTN_DOWN    20

GxEPD2_BW<GxEPD2_213_BN, GxEPD2_213_BN::HEIGHT> display(
  GxEPD2_213_BN(SCREEN_CS, SCREEN_DC, SCREEN_RST, SCREEN_BUSY)
);

#define DISP_W 250
#define DISP_H 122

static const int ENTRY_ROW_TOP[6] = {14, 28, 42, 56, 70, 84};
static const int ENTRY_TEXT_Y[6]  = {17, 31, 45, 59, 73, 87};
#define MENU_ENTRIES   6
#define HDR_H         12
#define HDR_TY         2
#define HINT_TOP     112
#define HINT_TY      113

#define CHARS_PER_LINE 41
#define LINES_PER_PAGE 10
#define READ_LINE_H    11
#define READ_FIRST_Y    1
#define SCROLL_STEP     3

#define MAX_BOOKS 20
char bookNames[MAX_BOOKS][44];
char bookFiles[MAX_BOOKS][48];
int  bookCount   = 0;
int  menuIndex   = 0;
int  menuScroll  = 0;
int  readingBook = -1;

File bookFile;
long pageStart     = 0;
long prevPageStart = 0;

#define HOLD_MS     1800
#define DEBOUNCE_MS  30

struct Btn {
  bool     lastRaw;
  bool     confirmed;
  uint32_t downAt;
  bool     holdFired;
} bUp={}, bDown={};

uint8_t pollBtn(int pin, Btn &s) {
  bool raw = (digitalRead(pin) == LOW);

  if (raw && !s.lastRaw) {
    s.downAt    = millis();
    s.confirmed = false;
    s.holdFired = false;
  }

  if (raw && !s.confirmed && (millis() - s.downAt > DEBOUNCE_MS)) {
    s.confirmed = true;
  }

  if (s.confirmed && !s.holdFired && (millis() - s.downAt >= HOLD_MS)) {
    s.holdFired = true;
    s.lastRaw   = raw;
    return 2;
  }

  if (!raw && s.lastRaw) {
    bool fire = s.confirmed && !s.holdFired;
    s.confirmed = false;
    s.holdFired = false;
    s.lastRaw   = false;
    return fire ? 1 : 0;
  }

  s.lastRaw = raw;
  return 0;
}

void stemToTitle(const char *stem, char *out, int maxLen) {
  int i = 0;
  while (stem[i] && i < maxLen - 1) {
    out[i] = (stem[i] == '_') ? ' ' : stem[i];
    i++;
  }
  out[i] = '\0';
}

void showMessage(const char *l1, const char *l2 = nullptr) {
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(5, 20);
    display.print(l1);
    if (l2) { display.setCursor(5, 36); display.print(l2); }
  } while (display.nextPage());
}

void drawSplash() {
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);

    
    display.drawRect(0, 0, DISP_W, DISP_H, GxEPD_BLACK);
    display.drawRect(3, 3, DISP_W-6, DISP_H-6, GxEPD_BLACK);

    
    int bx=14, by=12, bw=40, bh=52;
    display.fillRect(bx+3, by+3, bw, bh, GxEPD_BLACK);   // shadow
    display.fillRect(bx, by, bw, bh, GxEPD_WHITE);
    display.drawRect(bx, by, bw, bh, GxEPD_BLACK);
    display.fillRect(bx, by, 5, bh, GxEPD_BLACK);         // spine
    display.drawFastVLine(bx+bw-3, by+2, bh-4, GxEPD_BLACK); // page stack
    display.drawFastVLine(bx+bw-5, by+2, bh-4, GxEPD_BLACK);
    for (int li=0; li<5; li++) {                           // cover lines
      int lw = (li==0)?24:(li%2==0?18:14);
      display.fillRect(bx+8, by+8+li*8, lw, 2, GxEPD_BLACK);
    }
    display.setCursor(bx+22, by+bh-10);
    display.print("mk");

    
    display.setCursor(72, 20);  display.print("m  i  n  i");
    display.fillRect(72, 31, 162, 2, GxEPD_BLACK);
    display.setCursor(72, 35);  display.print("K  I  N  D  L  E");
    display.setCursor(72, 52);  display.print("distraction-free");
    display.setCursor(72, 64);  display.print("e-reader");
    display.setCursor(72, 80);  display.print("by groberts822");

    display.fillRect(6, DISP_H-9, DISP_W-12, 2, GxEPD_BLACK);

  } while (display.nextPage());
  delay(3000);
}

void discoverBooks() {
  bookCount = 0;
  File root = SD.open("/");
  while (bookCount < MAX_BOOKS) {
    File e = root.openNextFile();
    if (!e) break;
    if (e.isDirectory()) { e.close(); continue; }
    const char *name = e.name();
    int len = strlen(name);
    if (len > 4 && name[0] != '.' &&
        tolower(name[len-4])=='.' && tolower(name[len-3])=='t' &&
        tolower(name[len-2])=='x' && tolower(name[len-1])=='t') {
      snprintf(bookFiles[bookCount], 48, "/%s", name);
      char stem[44]; int sl=len-4; if(sl>43)sl=43;
      strncpy(stem, name, sl); stem[sl]='\0';
      stemToTitle(stem, bookNames[bookCount], 44);
      bookCount++;
    }
    e.close();
  }
  root.close();
}

void drawMenu() {
  if (menuIndex < menuScroll) menuScroll = menuIndex;
  if (menuIndex >= menuScroll + MENU_ENTRIES)
    menuScroll = menuIndex - MENU_ENTRIES + 1;

  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);

    
    display.fillRect(0, 0, DISP_W, HDR_H, GxEPD_BLACK);
    display.setTextColor(GxEPD_WHITE);
    display.setCursor(6, HDR_TY);
    display.print("MK LIBRARY");
    char cnt[12]; snprintf(cnt, 12, "%d books", bookCount);
    display.setCursor(DISP_W - (int)strlen(cnt)*6 - 4, HDR_TY);
    display.print(cnt);
    display.setTextColor(GxEPD_BLACK);

  
    for (int i = 0; i < MENU_ENTRIES; i++) {
      int bi = menuScroll + i;
      if (bi >= bookCount) break;

      if (bi == menuIndex) {
        display.fillRect(0, ENTRY_ROW_TOP[i], DISP_W, 14, GxEPD_BLACK);
        display.setTextColor(GxEPD_WHITE);
        display.setCursor(8, ENTRY_TEXT_Y[i]);
        display.print(bookNames[bi]);
        display.setTextColor(GxEPD_BLACK);
      } else {
        display.setCursor(8, ENTRY_TEXT_Y[i]);
        display.print(bookNames[bi]);
      }
    }

    
    display.fillRect(0, HINT_TOP, DISP_W, DISP_H-HINT_TOP, GxEPD_BLACK);
    display.setTextColor(GxEPD_WHITE);
    display.setCursor(6, HINT_TY);
    display.print("UP/DN:scroll  Hold UP:open");
    display.setTextColor(GxEPD_BLACK);

  } while (display.nextPage());
}

void renderPage(int scrollLines) {
  prevPageStart = pageStart;
  bookFile.seek(pageStart);

  const int BUF = LINES_PER_PAGE + 4;
  String lines[BUF];
  int lc = 0;
  String cur = "";

  while (lc < BUF && bookFile.available()) {
    char c = bookFile.read();
    if (c == '\r') continue;
    if (c == '\n') { lines[lc++] = cur; cur = ""; continue; }
    cur += c;
    if ((int)cur.length() >= CHARS_PER_LINE) {
      int sp = cur.lastIndexOf(' ');
      if (sp > 0) {
        lines[lc++] = cur.substring(0, sp);
        String ov = cur.substring(sp+1);
        bookFile.seek(bookFile.position() - ov.length());
        cur = "";
      } else { lines[lc++] = cur; cur = ""; }
    }
  }
  if (lc < BUF && cur.length() > 0) lines[lc++] = cur;

  if (scrollLines > 0) {
    bookFile.seek(pageStart);
    int skipped = 0; String tmp = "";
    while (skipped < scrollLines && bookFile.available()) {
      char c = bookFile.read();
      if (c == '\r') continue;
      if (c == '\n') { skipped++; tmp=""; continue; }
      tmp += c;
      if ((int)tmp.length() >= CHARS_PER_LINE) {
        int sp = tmp.lastIndexOf(' ');
        if (sp > 0) {
          skipped++;
          String ov = tmp.substring(sp+1);
          bookFile.seek(bookFile.position() - ov.length());
          tmp = "";
        } else { skipped++; tmp=""; }
      }
    }
    pageStart = bookFile.position();
  }

  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setTextColor(GxEPD_BLACK);
    for (int i = 0; i < LINES_PER_PAGE && i < lc; i++) {
      display.setCursor(2, READ_FIRST_Y + i * READ_LINE_H);
      display.print(lines[i]);
    }
    
    if (bookFile.size() > 0) {
      int barW = (int)((float)pageStart / bookFile.size() * (DISP_W-4));
      display.drawFastHLine(2, 119, DISP_W-4, GxEPD_BLACK);
      display.fillRect(2, 119, barW, 2, GxEPD_BLACK);
    }
  } while (display.nextPage());
}

void setup() {
  Serial.begin(115200);
  delay(300);
  pinMode(BTN_UP,   INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  display.init(115200, true, 50, false);
  display.setRotation(3);
  display.setFont(NULL);
  display.setTextSize(1);
  display.setTextColor(GxEPD_BLACK);
  display.setTextWrap(false);
  drawSplash();
  showMessage("Loading...");
  if (!SD.begin(SD_CS, SPI)) {
    showMessage("SD Card Failed", "Check wiring");
    while (true);
  }
  discoverBooks();
  if (bookCount == 0) {
    showMessage("No .txt files", "found on SD card");
    while (true);
  }
  drawMenu();
}

void loop() {
  uint8_t up   = pollBtn(BTN_UP,   bUp);
  uint8_t down = pollBtn(BTN_DOWN, bDown);

  if (readingBook < 0) {
    
    if      (up   == 1) { menuIndex = (menuIndex - 1 + bookCount) % bookCount; drawMenu(); }
    else if (down == 1) { menuIndex = (menuIndex + 1) % bookCount;              drawMenu(); }
    else if (up   == 2) {
      readingBook = menuIndex;
      pageStart = prevPageStart = 0;
      bookFile = SD.open(bookFiles[readingBook]);
      if (!bookFile) {
        showMessage("Failed to open", bookFiles[readingBook]);
        readingBook = -1; drawMenu(); return;
      }
      renderPage(0);
    }
  } else {
    
    if (down == 1) {
      if (bookFile.available()) renderPage(SCROLL_STEP);
      else showMessage("End of book.", "Hold DOWN:menu");
    }
    else if (up   == 1) { pageStart = prevPageStart; bookFile.seek(pageStart); renderPage(0); }
    else if (down == 2) { bookFile.close(); readingBook = -1; drawMenu(); }
  }
}
