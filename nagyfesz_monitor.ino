#if defined(__AVR__)
#define imagedatatype  unsigned int
#elif defined(__PIC32MX__)
#define imagedatatype  unsigned short
#elif defined(__arm__)
#define imagedatatype  unsigned short
#endif

#include <UTFT.h>
#include <UTouch.h>
#include <UTFT_Buttons.h>
#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac;

extern uint8_t BigFont[];

extern imagedatatype minus[];
extern imagedatatype plus[];
extern imagedatatype STOP[];
extern imagedatatype left[];
extern imagedatatype right[];

extern imagedatatype plusGray[];
extern imagedatatype minusGray[];
extern imagedatatype STOPGray[];
extern imagedatatype leftGray[];
extern imagedatatype rightGray[];

extern imagedatatype logo[];

UTFT    myGLCD(ITDB32S, 38, 39, 40, 41);

UTouch  myTouch( 6, 5, 4, 3, 2);

UTFT_Buttons  myButtons(&myGLCD, &myTouch);

double hv = (5.0 / 1024.0) * 600.0;
double ov = 4096.0 / 3000.0;

void drawLabels() {
  myGLCD.print("Vout:", 80, 10);                          // Vout: felirat
  myGLCD.printNumI(0000, 160, 10);                           // kijelzett feszültség érték
  myGLCD.print("V", 224, 10);                               // V felirat

  myGLCD.print("Iout:", 72, 88);                          // Iout: felirat
  myGLCD.printNumI(0000, 152, 88);                           // kijelzett áramerősség érték
  myGLCD.print("uA", 216, 88);                              // uA felirat

  myGLCD.print("Scale", 23, 140);                         // Scale: felirat
  myGLCD.printNumI(1, 55, 160);                              // kijelzett skála érték
}

void setup() {
  
 // analogReference(EXTERNAL);
 // analogRead(0);
  
  myGLCD.InitLCD();
  myGLCD.clrScr();

  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);

  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(0, 0, 0);

  myButtons.setTextFont(BigFont);

  // drawButtons();
  drawLabels();

  Serial.begin(9600);
  dac.begin(0x62);
}

void loop() {

  int logoButton;
  int plusButton;
  int minusButton;
  int stopButton;
  int leftButton;
  int rightButton;
  int pressedButton = 0;

  int x = 0;
  int y = 0;
  int plusButtonGray;
  int minusButtonGray;
  int stopButtonGray;
  int leftButtonGray;
  int rightButtonGray;

  boolean canI = false;
  
  double voltage = 0.0;
  double current = 0.0;
  double voltagePrint = 0.0;
  double voltagePrintTemp = 0.0;
  double currentPrint = 0.0;
  double outputVoltage = 0.0;
  int voltageScale = 0;

  int maxVoltage = 0;
  int maxCurrent = 0;
  int v;
  int c;
  
  plusButton = myButtons.addButton(165, 36, 48, 42, plus, BUTTON_NO_BORDER);
  minusButton = myButtons.addButton(107, 36, 48, 42, minus, BUTTON_NO_BORDER);
  stopButton = myButtons.addButton(230, 150, 72, 72, STOP, BUTTON_NO_BORDER);
  leftButton = myButtons.addButton(10, 189, 48, 41, left, BUTTON_NO_BORDER);
  rightButton = myButtons.addButton(68, 189, 48, 41, right, BUTTON_NO_BORDER);

  logoButton = myButtons.addButton(278, 10, 32, 44, logo, BUTTON_NO_BORDER);

  myButtons.drawButtons();

  plusButtonGray = myButtons.addButton(165, 36, 48, 42, plusGray, BUTTON_NO_BORDER);
  minusButtonGray = myButtons.addButton(107, 36, 48, 42, minusGray, BUTTON_NO_BORDER);
  stopButtonGray = myButtons.addButton(230, 150, 72, 72, STOPGray, BUTTON_NO_BORDER);
  leftButtonGray = myButtons.addButton(10, 189, 48, 41, leftGray, BUTTON_NO_BORDER);
  rightButtonGray = myButtons.addButton(68, 189, 48, 41, rightGray, BUTTON_NO_BORDER);

  dac.setVoltage(0, false);

  while (true) {

    if (myTouch.dataAvailable()) {
      myTouch.read();
      x = myTouch.getX();
      y = myTouch.getY();

      if (x > 164 && x < 214 && y > 35 && y < 79) {
        waitForIt(plusButton, plusButtonGray);
        pressedButton = plusButton;

      //  myButtons.drawButton(plusButtonGray);

        maxVoltage = 0;
        maxCurrent = 0;
      }
      else if (x > 106 && x < 156 && y > 35 && y < 79) {
        waitForIt(minusButton, minusButtonGray);
        pressedButton = minusButton;

      //  myButtons.drawButton(minusButtonGray);

        maxVoltage = 0;
        maxCurrent = 0;
      }
      else if (x > 229 && x < 303 && y > 149 && y < 223) {
        waitForIt(stopButton, stopButtonGray);
        pressedButton = stopButton;

      //  myButtons.drawButton(stopButtonGray);
      }
      else if (x > 9 && x < 59 && y > 188 && y < 231) {
        waitForIt(leftButton, leftButtonGray);
        pressedButton = leftButton;

      //  myButtons.drawButton(leftButtonGray);
      }
      else if (x > 67 && x < 117 && y > 188 && y < 231) {
        waitForIt(rightButton, rightButtonGray);
        pressedButton = rightButton;

      //  myButtons.drawButton(rightButtonGray);
      }
      else{
        pressedButton = -1;
      }
/*
     pressedButton = myButtons.checkButtons();

     if(pressedButton == rightButton){
       myButtons.drawButton(rightButtonGray);
     }
     else if(pressedButton == leftButton){
       myButtons.drawButton(leftButtonGray);
     }
     else if(pressedButton == stopButton){
       myButtons.drawButton(stopButtonGray);
     }
     else if(pressedButton == plusButton){
       maxVoltage = 0;
       maxCurrent = 0;

       myButtons.drawButton(plusButtonGray);
     }
     else{                                    //minusButton
       maxVoltage = 0;
       maxCurrent = 0;

       myButtons.drawButton(minusButtonGray);
     }
*/
     Serial.print(pressedButton);
   //   if(canI == false){
        if (pressedButton == plusButton) {            // plus
          if (voltageScale == 0) {
            if (voltagePrint < 1600)                  // 3000
              voltagePrint++;
          }
          else if (voltageScale == 1) {
            if (voltagePrint < 1591)                    // 2991
              voltagePrint = voltagePrint + 10;
          }
          else if (voltageScale == 2) {
            if (voltagePrint < 1576)                      // 2976
              voltagePrint = voltagePrint + 25;
          }
          else if (voltageScale == 3) {
            if (voltagePrint < 1551)                        // 2951
              voltagePrint = voltagePrint + 50;
          }
          else if (voltageScale == 4) {
            if (voltagePrint < 1501)                        // 2901
              voltagePrint = voltagePrint + 100;
          }
          else {                                                        // voltageScale == 5
            if (voltagePrint < 1401)                          // 2801
              voltagePrint = voltagePrint + 200;
          }
        }
        else if (pressedButton == minusButton) {          // minus
          if (voltageScale == 0) {
            if (voltagePrint > 0)
              voltagePrint--;
          }
          else if (voltageScale == 1) {
            if (voltagePrint > 9)
              voltagePrint = voltagePrint - 10;
          }
          else if (voltageScale == 2) {
            if (voltagePrint > 24)
              voltagePrint = voltagePrint - 25;
          }
          else if (voltageScale == 3) {
            if (voltagePrint > 49)
              voltagePrint = voltagePrint - 50;
          }
          else if (voltageScale == 4) {
            if (voltagePrint > 99)
              voltagePrint = voltagePrint - 100;
          }
          else {                                                    // voltageScale == 5
            if (voltagePrint > 199)
              voltagePrint = voltagePrint - 200;
          }
        }
        else if (pressedButton == stopButton) {           // stop
          outputVoltage = 0.0;
          dac.setVoltage(0, false);
          voltagePrint = 0.0;
          voltagePrintTemp = 0.0;
          voltageScale = 0;
          maxVoltage = 0;
          maxCurrent = 0;
  
          myGLCD.print("   ", 39, 160);
          myGLCD.printNumI(1, 55, 160);
  
          myGLCD.print("    ", 160, 10);
          myGLCD.printNumI(0000, 160, 10);
          myGLCD.print("    ", 152, 88);
          myGLCD.printNumI(0000, 152, 88);
        }
        else if (pressedButton == leftButton) {           // scale left
          if (voltageScale > 0) {
            voltageScale--;
  
            if (voltageScale == 0) {
              myGLCD.print("   ", 39, 160);
              myGLCD.printNumI(1, 55, 160);
            }
            else if (voltageScale == 1) {
              myGLCD.print("   ", 39, 160);
              myGLCD.printNumI(10, 47, 160);
            }
            else if (voltageScale == 2) {
              myGLCD.print("   ", 39, 160);
              myGLCD.printNumI(25, 47, 160);
            }
            else if (voltageScale == 3) {
              myGLCD.print("   ", 39, 160);
              myGLCD.printNumI(50, 47, 160);
            }
            else if (voltageScale == 4) {
              myGLCD.print("   ", 39, 160);
              myGLCD.printNumI(100, 39, 160);
            }
            else {                                      // voltageScale == 5
              myGLCD.print("   ", 39, 160);
              myGLCD.printNumI(200, 39, 160);
            }
          }
        }
        else if (pressedButton == rightButton) {          // scale right
          if (voltageScale < 5) {
            voltageScale++;
  
            if (voltageScale == 0) {
              myGLCD.print("   ", 39, 160);
              myGLCD.printNumI(1, 55, 160);
              Serial.println(voltageScale);
            }
            else if (voltageScale == 1) {
              myGLCD.print("   ", 39, 160);
              myGLCD.printNumI(10, 47, 160);
              Serial.println(voltageScale);
            }
            else if (voltageScale == 2) {
              myGLCD.print("   ", 39, 160);
              myGLCD.printNumI(25, 47, 160);
              Serial.println(voltageScale);
            }
            else if (voltageScale == 3) {
              myGLCD.print("   ", 39, 160);
              myGLCD.printNumI(50, 47, 160);
              Serial.println(voltageScale);
            }
            else if (voltageScale == 4) {
              myGLCD.print("   ", 39, 160);
              myGLCD.printNumI(100, 39, 160);
              Serial.println(voltageScale);
            }
            else {                                          // voltageScale == 5
              myGLCD.print("   ", 39, 160);
              myGLCD.printNumI(200, 39, 160);
              Serial.println(voltageScale);
            }
          }
        }
   //   }
      
 //     canI = true;
    }
   /* else if(pressedButton != -1){
      myButtons.drawButton(pressedButton);
      pressedButton = -1;

      canI = false;
    }
*/
    if(voltagePrint != voltagePrintTemp){
      outputVoltage = ov * voltagePrint;
      dac.setVoltage(outputVoltage, false);

      voltagePrintTemp = voltagePrint;
    }

    v = analogRead(0);
   // c = analogRead(0);

    if(v == 0){
      voltage = 0;
      myGLCD.print("    ", 160, 10);
      myGLCD.printNumI(voltage, 160, 10);
    }
    if(v > maxVoltage || (v <= (maxVoltage - 3))){
      maxVoltage = v;

      voltage = hv * v;
      myGLCD.print("    ", 160, 10);
      myGLCD.printNumI(voltage, 160, 10);
    //  Serial.print("IM IN");
    //  Serial.print("   ");
    }
    else{
    //  Serial.print("Im not in");
    //  Serial.print("   ");
    }
    if(c > maxCurrent){
      maxCurrent = c;

      current = (1300.0 / 1024.0) * c;
      myGLCD.print("    ", 152, 88);
      myGLCD.printNumI(current, 152, 88);
    }

    Serial.print("DAC num: ");
    Serial.print(outputVoltage);
    Serial.print("   ");
    Serial.print("vPrint: ");
    Serial.print(voltagePrint);
    Serial.print("   ");
    Serial.print("Vout: ");
    Serial.print(voltage);
    Serial.print("   ");
    Serial.print("A0: ");
    Serial.println(analogRead(0));
/*    Serial.print("   ");
    Serial.print("Cpin: ");
    Serial.println(analogRead(currentPin));
*/

  }
}

void waitForIt(int button, int buttonGray)
{
  myButtons.drawButton(buttonGray);
  while (myTouch.dataAvailable())
    myTouch.read();
  myButtons.drawButton(button);
}

























