#include <TFT_eSPI.h>
#include <SPI.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <OneButton.h> 

#include "colors.h"
#include "eye.h"
#include "freefonts.h"
#include "meshbg.h"
#include "startscreen.h"

TFT_eSPI tft = TFT_eSPI();

const int buttonPin37 = 37;
const int buttonPin38 = 38;
const int buttonPin39 = 39;
const int triangleSize = 50;
const int triangleGap = 5;
const int eyeSize = 25;                       // EYE IMAGE SIZE

int counter37 = 0;
int counter39 = 0;
bool button37Pressed = false;
bool button39Pressed = false;

// WIFI CREDENTIALS
const char *ssid = "Gspot2.0";
const char *password = "3940518389651270";

// DEFINE NTP CLIENT
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "us.pool.ntp.org");

// Timer variables
unsigned long startMillis = 0;                            // VARIABLE TO STORE THE INITIAL TIME WHEN TIMER STARTS
bool timerRunning = false;                                // FLAG TO INDICATE IF THE TIMER IS RUNNING
int elapsedSeconds = 0;
int elapsedMinutes = 0;
const int minuteReset = 60 * 1000 * 60;                   // 60 MINUTES IN MILLISECONDS
OneButton button37(buttonPin37, true);                    // TRUE INDICATES THAT THE BUTTON IS ACTIVE LOW
OneButton button38(buttonPin38, true);

void drawCounter(int x, int y, int value, uint16_t textColor);
void drawCounter2(int x, int y, int value, uint16_t textColor);
void showStartScreen();
void drawTriangles();
void drawCircles();
void scoreGraphics();
void drawTimer(int x, int y, int minutes, int seconds, uint16_t textColor, uint16_t bgColor);

void setup()
{
  tft.init();
  tft.setRotation(0);

  showStartScreen();                                      // DISPLAY SPLASHSCREEN FOR X SECONDS

  tft.fillScreen(0x090A);

  int thick = 3;                                          // SET THE LINE THICKNESS
  int radius = 5;
  for (int i = 0; i < thick; i++)
  {

    tft.drawRoundRect(0 + i, 0 + i, 240 - i * 2, 320 - i * 2, 5, GOLD);              // BORDER
    tft.drawRoundRect(0 + i, 75 + i, 70 - i * 2, 320 - i * 2, 5, GOLD);              // TRAINGLE DICE BORDER
  }

  drawTriangles();
  scoreGraphics();
  drawCounter(117, 236, counter37, GOLD);                 // DRAW THE PLAYER 1 INITIAL COUNTER (0)
  drawCounter2(172, 236, counter39, BLACK);               // DRAW THE PLAYER 2 INITIAL COUNTER (0)
  pinMode(buttonPin37, INPUT_PULLUP);
  pinMode(buttonPin38, INPUT_PULLUP);
  pinMode(buttonPin39, INPUT_PULLUP);

  WiFi.begin(ssid, password);                             // CONNECT TO WIFI
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  timeClient.begin();                                     // INITIALIZE NTP CLIENT
  timeClient.setTimeOffset(-25200);                       // UTC -7 hours for PDT, UTC -8 hours for PST (Daylight Saving Time is automatically taken care of by the NTPClient)

  // Attach long press event handlers to buttons
  button37.attachLongPressStart([]() {
    if (timerRunning) {
      timerRunning = false;
      elapsedSeconds = 0;
      elapsedMinutes = 0;
      drawTimer(100, 150, elapsedMinutes, elapsedSeconds, GREENYELLOW, BLACK);      // RESET TIMER DISPLAY TO 00:00
    }
  });
  button38.attachLongPressStart([]() {
    timerRunning = true;
    startMillis = millis();
  });
}

void loop()
{
  button37.tick();                                                // HANDLE BUTTON EVENTS
  button38.tick();

  timeClient.update();                                            // UPDATE NTP TIME

  String currentTime = timeClient.getFormattedTime();

  tft.setTextColor(GOLD, DARKBLUE);
  tft.setTextSize(2);
  tft.setCursor(30, 30);
  tft.print(currentTime);

  if (digitalRead(buttonPin37) == LOW && !button37Pressed)
  {
    button37Pressed = true;                                       // SET THE FLAG TO INDICATE THE BUTTON IS PRESSED
    counter37 = (counter37 + 1) % 8;                              // INCREMENT THE COUNTER AND WRAP AROUND AT 7
    drawCounter(117, 236, counter37, GOLD);                       // UPDATE THE DISPLAY COUNTER VALUE FOR BUTTONPIN37 USING GOLD TEXT
    delay(200);                                                   // DEBOUNCE DELAY
  }

  if (digitalRead(buttonPin37) == HIGH)
  {
    button37Pressed = false;                                      // RESET THE FLAG WHEN BUTTON IS RELEASED
  }

  if (digitalRead(buttonPin38) == LOW)
  {
    drawTriangles();                                              // DRAW THE TRIANGLES
    delay(1000);
  }

  if (digitalRead(buttonPin39) == LOW && !button39Pressed)
  {
    button39Pressed = true;                                       // SET THE FLAG TO INDICATE THE BUTTON IS PRESSED
    counter39 = (counter39 + 1) % 8;                              // INCREMENT THE COUNTER AND WRAP AROUND AT 7 wrap around at 7
    drawCounter2(172, 236, counter39, BLACK);                     // UPDATE THE DISPLAY COUNTER VALUE FOR BUTTONPIN39 USING GOLD TEXT
    delay(200);                                                   // DEBOUNCE DELAY
  }

  if (digitalRead(buttonPin39) == HIGH)
  {
    button39Pressed = false;                                      // RESET THE FLAG WHEN BUTTON IS RELEASED
  }

  // Timer update logic
  if (timerRunning && elapsedMinutes < 60)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - startMillis >= 1000)
    {
      elapsedSeconds++;
      if (elapsedSeconds >= 60)
      {
        elapsedSeconds = 0;
        elapsedMinutes++;
      }
      startMillis = currentMillis;
    }
  }

  // UPDATE TIMER AT X COORDINATES
  drawTimer(100, 150, elapsedMinutes, elapsedSeconds, GOLD, DARKBLUE);
}

void drawCounter(int x, int y, int value, uint16_t textColor)
{
  tft.setTextColor(GOLD, BLACK);
  tft.setTextSize(4);

  tft.setCursor(x, y);
  tft.print(value); // PRINT THE UPDATED COUNTER VALUE
}

void drawCounter2(int x, int y, int value, uint16_t textColor)
{
  tft.setTextColor(BLACK, GOLD);
  tft.setTextSize(4);
  tft.setCursor(x, y);
  tft.print(value); // PRINT THE UPDATED COUNTER VALUE
}

void drawTimer(int x, int y, int minutes, int seconds, uint16_t textColor, uint16_t bgColor)
{
  tft.setTextColor(textColor, bgColor);
  tft.setTextSize(2);
  tft.setCursor(x, y);
  tft.printf("%02d:%02d", minutes, seconds);
}

void drawTriangles() {
  int x = 10;
  int y = 310 - 4 * (triangleSize + triangleGap);   // ADJUST THE STARTING Y POSITION FOR THE 4 TRIANGLES
  for (int i = 0; i < 4; i++) {
    // DRAW FILLED EQUILATERAL TRIANGLE
    int x1 = x + triangleSize / 2;
    int y1 = y;
    int x2 = x;
    int y2 = y + triangleSize;
    int x3 = x + triangleSize;
    int y3 = y + triangleSize;
    tft.fillTriangle(x1, y1, x2, y2, x3, y3, GOLD);

    // CALCULATE THE POSITION TO CENTER THE EYE IMAGE WITHIN THE TRIANGLE AND MOVE IT DOWN BY X PIXELS
    int eyeX = x1 - eyeSize / 2;
    int eyeY = y1 + (triangleSize - eyeSize) / 2 + 11;

    // RANDOMLY DRAW THE EYE IMAGE
    if (random(2) == 0) {
      // DRAW THE EYE IMAGE CENTERED WITHIN THE TRIANGLE
      tft.pushImage(eyeX, eyeY, eyeSize, eyeSize, eye);
    }

    y += triangleSize + triangleGap;
  }
}

void scoreGraphics() {      
  tft.fillRoundRect(102, 200, 49, 52, 5, BLACK);        // PLAYER 1 GRAPHICS MENU
  tft.fillSmoothCircle(126, 250, 25, BLACK);
  tft.drawSmoothCircle(126, 250, 25, GOLD, GOLD);
  tft.fillRoundRect(102, 192, 105, 30, 5, BLACK);
  tft.setCursor(108, 200);
  tft.setTextSize(2);
  tft.setTextColor(GOLD);
  tft.print("PLAYER 1");

  tft.fillRoundRect(156, 251, 51, 50, 5, GOLD);         // PLAYER 2 GRAPHICS MENUU
  tft.fillSmoothCircle(182, 250, 25, GOLD);
  tft.drawSmoothCircle(182, 250, 25, BLACK, BLACK);
  tft.fillRoundRect(101, 280, 105, 25, 5, GOLD);
  tft.setCursor(108, 285);
  tft.setTextColor(BLACK);
  tft.print("PLAYER 2");
}

void showStartScreen() {
  tft.setSwapBytes(true);
  tft.pushImage(0, 0, 240, 320, startscreen);           // SET THE SPLASH SCREEN
  delay(3000);                                          // WAIT FOR X SECONDS (X milliseconds)
}
