#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD I2C address and dimensions
#define TRIG_PIN 2
#define ECHO_PIN 3
#define LED_RED 6
#define LED_GREEN 7
#define BUTTON_CYCLE 9
#define BUTTON_READ 10
float distance = 0;
bool displayInches = false;
bool instructionsShown = false;
unsigned long lastButtonPress = 0;
const unsigned long debounceTime = 250;
void setup() {
  // put your setup code here, to run once:
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_RED, HIGH); //this turns off the led because it is active low
  digitalWrite(LED_GREEN, HIGH); //this turns off the led because it is active low
  pinMode(BUTTON_CYCLE, INPUT);
  pinMode(BUTTON_READ, INPUT);
  //LCD setup
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  delay(1500);
  lcd.clear();
  showInstructions();
  instructionsShown = true;
}
void showInstructions() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Btn2: Read dist");
  lcd.setCursor(0,1);
  if(displayInches){
    lcd.print("Btn1: Inches");
  } else {
    lcd.print("Btn1: Centimeters");
  }
}
float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  // timeout → no echo received
  if (duration == 0) {
    return -1;
  }
  float distanceCm = (duration / 2.0) * 0.0343;
  return distanceCm;
}
void updateLEDs() {
  if (distance < 0) {
    // ERROR → red ON
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
  } else {
    // NOT ERROR → green ON
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
  }
}
void updateLCD(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Distance:");
  lcd.setCursor(0,1);   // switches the cursor to the second line
  if(distance < 0) {
    lcd.print("ERROR");
  } else {
    if(displayInches){
      float distanceInches = distance * 0.393701; // Convert cm to inches
      lcd.print(distanceInches, 2); // Display distance in inches with 2 decimal places
      lcd.print(" in");
    } else {
      lcd.print(distance, 2); // Display distance in cm with 2 decimal places
      lcd.print(" cm");
    }
  }
}
void loop() {
  unsigned long currentMillis = millis();
  // ----- Button 1: toggle cm/inches -----
  if(digitalRead(BUTTON_CYCLE) == HIGH && currentMillis - lastButtonPress > debounceTime){
    displayInches = !displayInches;
    lastButtonPress = currentMillis;
    if(instructionsShown){
      // Update instructions line
      lcd.setCursor(0,1);
      lcd.print("                "); // clear line
      if(displayInches){
        lcd.setCursor(0,1);
        lcd.print("Btn1: Inches");
      } else {
        lcd.setCursor(0,1);
        lcd.print("Btn1: Centimeters");
      }
    } else {
      // Already have a measurement → switch units immediately
      updateLCD();
    }
  }
  // ----- Button 2: take reading -----
  if(digitalRead(BUTTON_READ) == HIGH && currentMillis - lastButtonPress > debounceTime){
    distance = readDistance();
    updateLEDs();
    if(instructionsShown){
      lcd.clear();
      instructionsShown = false;
    }
    updateLCD();
    lastButtonPress = currentMillis;
  }
}
