/*
Name: Schweizisk Armeficklampa (Swizz army knife)
Creator: Robin Almström, Gothenburg, Sweden

Last edited: 2025-02-06 14:00

Description:
The code for my "flashlight". If you move the joystick, it changes the light, and if you press it, it makes a sound.
(There is a blue button on the inside that controls the simulated battery, but that's not for the users.)
*/

// Definitions
#include "U8glib.h"

#include <math.h>

#include <Adafruit_NeoPixel.h>

#include "pitches.h"

#include "music.h"

#define BUZZER_PIN 8

#define PIN       7
#define NUM_LEDS 24

//Neopixel stuff
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB);

// Important u8g display magic.
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);


// Nokia Ringtone notes
int melody[] = {
  NOTE_E5, NOTE_D5, NOTE_FS4, NOTE_GS4, 
  NOTE_CS5, NOTE_B4, NOTE_D4, NOTE_E4, 
  NOTE_B4, NOTE_A4, NOTE_CS4, NOTE_E4,
  NOTE_A4
};

//Nokia ringtone note durations
int durations[] = {
  8, 8, 4, 4,
  8, 8, 4, 4,
  8, 8, 4, 4,
  2
};




// Storing Neopixel-values as the colour they look like.
uint32_t Black = strip.Color(0, 0, 0); 
uint32_t Grey = strip.Color(30, 30, 30); 
uint32_t White = strip.Color(250, 250, 250); 
uint32_t Purple = strip.Color(30, 0, 30); 
uint32_t Red = strip.Color(200, 10, 0); 
uint32_t DarkRed = strip.Color(100, 10, 0); 
uint32_t DesatRed = strip.Color(120, 40, 10); 
uint32_t Orang = strip.Color(100, 50, 0); 
uint32_t Yelow = strip.Color(100, 100, 0); 
uint32_t Gren = strip.Color(10, 50, 10); 
uint32_t Blu = strip.Color(0, 10, 50); 
uint32_t TransPink = strip.Color(130, 70, 90); 
uint32_t BebeBlu = strip.Color(10, 50, 70);
uint32_t Hotpink = strip.Color(50, 0, 20);
uint32_t Desatpink = strip.Color(80, 10, 50);
uint32_t LessHotpink = strip.Color(80, 0, 40);
uint32_t Darkpink = strip.Color(50, 0, 25);


const int SW_pin = 4; // digital pin connected  to switch output
const int X_pin = 1; // analog pin connected to X output
const  int Y_pin = 0; // analog pin connected to Y output
const int Blu_pin = 2; // digital pin conected to blue button output(on the inside)

int ActiveLight = 0; // the value of the current active light
float Battery = 100; // battery percentage (simulated)
String CurrentLightName = "Gay"; // name of the current light
String TextETT = ""; // top row being displayed on the OLED
String TextTVA = ""; // bottom row displayed on the OLED





// The setup
void  setup() {
  Serial.begin(9600); // opens a serial communication with the monitor with the frequency 9600 baud.
  pinMode(SW_pin, INPUT); // sets the pin connected to the button on the joystick to INPUT
  digitalWrite(SW_pin, HIGH); // the code breaks without this, don't question it unless you want infinite nokia ringtone
  
  pinMode(Blu_pin, INPUT); // sets the pin connected to the blue button inside the box to be an input.

  strip.begin();           
  strip.show();            // Initialize all Neoring pixels to 'off'
  strip.setBrightness(40); // 40/255 brightness (about 15%)

  
  u8g.setFont(u8g_font_unifont); // sets the font of the OLED
}




// the looop
void loop() {
  
  if (Battery <= 0){ // if the battery drops to zero or lower:
    DrawOLED("Battery:", "Zero"); // the OLED shows the battery is dead
    delay(1000); // waits for a second
    Nolight(); // turns of the Neopixel
    if (digitalRead(Blu_pin)){ // if the blue button is pressed:
      Battery += 100; //Adds 100% to the battery, thus turning it on
    }
  }else{ // if the battery is above zero precent:
    Battery -= 0.01; // Lowers the simulated battery by a precent of a precent.
    TellSerialJoystick(); // writes debugging/maintenance data on the serial monitor.
    Picklight(); // Refreshes the light so that it is accurate and controllable.
    TextETT = "Battery: " + String(int(Battery)) + "%"; // Sets the upper row of the OLED to tell the current battery in whole percent.
    TextTVA = "Light: " + CurrentLightName; // Sets the lower row of the OLED to tell the name of the current light.
    DrawOLED(TextETT, TextTVA); // Writes the stuff on the OLED.

    if (digitalRead(Blu_pin)){ //If the blue button is pressed (and battery is over 0%):
      Battery -= 10; // lower the battery by 10%
    }

    if (!digitalRead(SW_pin)){ //If joystick pressed:
      Playsong(); // Play the nokia ringtone
    }
  }
}






/*
Writes general information and the status of the buttons and joystick on the serial monitor.
Input: Void,  Output: None
*/
void TellSerialJoystick() {
  Serial.print("X-axis:  ");
  Serial.print(analogRead(X_pin));
  Serial.print("\
  ");
  Serial.print("Y-axis:  ");
  Serial.println(analogRead(Y_pin));
  Serial.print("\
\
");
  Serial.print("Button:  ");
  Serial.println(digitalRead(SW_pin));
  Serial.print("Blue Button:  ");
  Serial.println(digitalRead(Blu_pin));
  Serial.print("Light:  ");
  Serial.println(CurrentLightName);
  
}


/*
The main code for the light; changes the light if the joystick is moved to the left or right on the X-axis.
Input: Void,  Output: None
*/
void Picklight() {
  if (analogRead(X_pin) > 600) {
      ActiveLight += 1;
      delay(10);
    }else if (analogRead(X_pin) < 400) {
      ActiveLight -= 1;
      delay(10);
    }

    // if you go beyond the defined values of 'ActiveLight' you loop pack to the other end.
    if (ActiveLight < -1) {
      ActiveLight = 6;
    }else if(ActiveLight > 6) {
      ActiveLight = -1;
    }

  //Assigns a name and a light pattern for every value of 'ActiveLigth'.
  if (ActiveLight == 0){Gaylight();CurrentLightName="Gay";}else if (ActiveLight == -1){
    Boringlight();CurrentLightName="White";}else if (ActiveLight == 1){Translight();CurrentLightName="Trans";}else if (ActiveLight == 2){NonBlight();CurrentLightName="NB";}else if (ActiveLight == 3){Lesbilight();CurrentLightName="Lesbian";}else if (ActiveLight == 4){Bilight();CurrentLightName="Bi";}else if (ActiveLight == 5){Acelight();CurrentLightName="Ace";}else if (ActiveLight == 6){Panlight();CurrentLightName="Pan";}
}


/*
Turns of all lights on Neopixel.
Input: Void,  Output: None
*/
void Nolight() {
  LightSpecific(0,23,Black);
}


/*
Draws the current light pattern on the Neopixel.
Input: Void,  Output: None
*/
void Boringlight() {
  LightSpecific(0,23,White);
}


/*
Draws the current light pattern on the Neopixel.
Input: Void,  Output: None
*/
void Acelight() {
  LightSpecific(0,5,Black);
  LightSpecific(6,11,Grey);
  LightSpecific(12,17,White);
  LightSpecific(18,23,Purple);
}

/*
Draws the current light pattern on the Neopixel.
Input: Void,  Output: None
*/
void Gaylight() {
  LightSpecific(0,3,Red);
  LightSpecific(4,7,Orang);
  LightSpecific(8,11,Yelow);
  LightSpecific(12,15,Gren);
  LightSpecific(16,19,Blu);
  LightSpecific(20,23,Purple);
}

/*
Draws the current light pattern on the Neopixel.
Input: Void,  Output: None
*/
void Translight() {
  LightSpecific(0,4,BebeBlu);
  LightSpecific(5,9,TransPink);
  LightSpecific(10,13,White);
  LightSpecific(14,18,TransPink);
  LightSpecific(19,23,BebeBlu);
}

/*
Draws the current light pattern on the Neopixel.
Input: Void,  Output: None
*/
void Panlight() {
  LightSpecific(0,7,Hotpink);
  LightSpecific(8,15,Yelow);
  LightSpecific(16,23,BebeBlu);
}

/*
Draws the current light pattern on the Neopixel.
Input: Void,  Output: None
*/
void NonBlight() {
  LightSpecific(0,5,Yelow);
  LightSpecific(6,11,White);
  LightSpecific(12,17,Purple);
  LightSpecific(18,23,Black);
}

/*
Draws the current light pattern on the Neopixel.
Input: Void,  Output: None
*/
void Bilight() {
  LightSpecific(0,7,LessHotpink);
  LightSpecific(8,15,Purple);
  LightSpecific(16,23,Blu);
}

/*
Draws the current light pattern on the Neopixel.
Input: Void,  Output: None
*/
void Lesbilight() {
  LightSpecific(0,4,DarkRed);
  LightSpecific(5,9,DesatRed);
  LightSpecific(10,13,White);
  LightSpecific(14,18,Desatpink);
  LightSpecific(19,23,Darkpink);
}

/*
Draws the text given to it on the OLED.
Input: Text for each row,  Output: None
*/
void DrawOLED(String text, String text2){  // the base code of the OLED, i dont have time for commentating on it.
  u8g.firstPage();

  do{
    u8g.drawStr(0, 22, text.c_str()); 
    u8g.drawStr(0, 41, text2.c_str());
    //u8g.drawStr(0, 60, text3.c_str());
  }while (u8g.nextPage());
}


/*
Draws the specified colour on the neopixel.
Input: start and end light of area to colour as well as the colour.
Output: None
*/
void LightSpecific(int start, int end, uint32_t colour){
  for(int i=start; i<=end; i++) { // for every specified pixel
    strip.setPixelColor(i, colour); // Draw current pixel in colour
    strip.show(); // show the pixel
  }
}


/*
Plays the nokia ringtone.
Input: Void
Output None
*/
void Playsong() {
  int size = sizeof(durations) / sizeof(int); // calculates the length of the song.

  for (int note = 0; note < size; note++) { // for every note of the song:
    int duration = 1000 / durations[note]; // calculates the duration of the note.
    tone(BUZZER_PIN, melody[note], duration); //plays the current note value for the current duration.

    int pauseBetweenNotes = duration * 1.30; // calculates the pause between notes.
    delay(pauseBetweenNotes); // delays by that amount of time.

    noTone(BUZZER_PIN); // Stops playing the note.
  }
}