#include <HC05.h> // pt bluetooth
#include <Keypad.h> // libraria pentru tastatura
#include <LiquidCrystal_I2C.h> // libraria pentru I2C
#include <Servo.h> // libraria pt servomotor

LiquidCrystal_I2C lcd(0x27, 20, 2); // creeaza obiectul LCD de tipul clasei LiquidCrystal_I2C; 0x27 e adresa lcd-ului; 20 nr de coloane si 2 nr de randuri
Servo servo; // creeaza obiectul servo de tipul clasei Servo

int buzzer = 13;

#define Password_Length 5 // setez lungimea parolei + 1 cifra

int Position = 0; // pozitia servomotorului

char Particular[Password_Length]; // lungimea parolei
char Specific[Password_Length] = "1234"; // PAROLA CORECTA

byte Particular_Count = 0, Specific_Count = 0; // numara cate cifre are parola si daca e corect numarul

char Key;
char data;

const byte ROWS = 4; // nr de randuri de la tastatura
const byte COLS = 4; // nr de coloane de la tastatura

char keys[ROWS][COLS] = { // fac o matrice cu nr de linii si coloane pe care le are tastatura 4x4
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

bool SmartDoor = true; // pentru servomotor

// pinii pt care conectez tastatura
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad myKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); // creez obiectul myKeypad in care pun datele obtinute mai sus

// locked character
byte Locked[8] = {
  B01110,
  B10001,
  B10001,
  B11111,
  B11011,
  B11011,
  B11011,
  B11111
};

// open character
byte Opened[8] = {
  B01110,
  B00001,
  B00001,
  B11111,
  B11011,
  B11011,
  B11011,
  B11111
};
void setup()
{
  servo.attach(10); // atașează servo la pinul 10 de la Arduino
  ServoClose(); // închide servo motorul

  lcd.init(); // initializează LCD-ul
  lcd.backlight(); // activează iluminarea fundalului pentru a se vedea scrisul
  lcd.setCursor(0, 0); // plasează cursorul în poziția stânga sus a ecranului
  lcd.print("Proiect sistem"); // afișează textul

  lcd.setCursor(0, 1); // plasează cursorul pe a doua linie a ecranului
  lcd.print("de securitate"); // afișează textul

  delay(2000); // așteaptă 2 secunde

  lcd.clear(); // curăță afișajul LCD

  Serial.begin(9600); // inițializează comunicația serială

  pinMode(13, OUTPUT); // setează pinul 13 ca ieșire pentru buzzer
}
void loop()
{
  if (SmartDoor == 0) // deschidem
  {
    Key = myKeypad.getKey(); // iau valoarea tastei pe care am apăsat

    if (Key == '#') // apăs # și închei scrierea parolei
    {
      lcd.clear(); // șterge ce e pe display
      ServoClose(); // închide servo motorul

      lcd.setCursor(2, 0); // setez cursorul
      lcd.print("INCHIS"); // afișez obiect închis

      lcd.createChar(0, Locked); // creează caracterul pentru obiect închis
      lcd.setCursor(14, 0); // setez cursorul
      lcd.write(0); // afișează caracterul pentru obiect închis

      delay(3000); // întârziere

      SmartDoor = 1; // închid obiect
    }
  }
  else Open(); // altfel își păstrează starea
}
void clearData() // funcția ce șterge datele
{
  while (Particular_Count != 0) // cât timp cifrele pe care le-am apăsat există
  {
    Particular[Particular_Count--] = 0; // numără câte cifre am și le resetează
  }
  return; // returnează numărul de cifre
}

void ServoOpen() // deschide servomotorul
{
  for (Position = 180; Position >= 0; Position -= 5) { // se mută de la 180 la 0 grade
    servo.write(Position); // se mută la poziția dată de mine
    delay(15); // întârziere
  }
}

void ServoClose() // închide servomotorul
{
  for (Position = 0; Position <= 180; Position += 5) { // se mută de la 0 la 180 grade
    servo.write(Position); // mută servomotorul la poziția dată
    delay(15); // așteaptă 15 milisecunde
  }
}
void Open() // PRINCIPAL
{
  if (Serial.available() > 0)
  {
    data = Serial.read();
    delay(1000);

    if (data == '1')
    {
      digitalWrite(13, HIGH);
      Serial.println("Buzzer on");
    }

    if (data == '0')
    {
      digitalWrite(13, LOW);
      Serial.println("Buzzer off");
    }
  }

  lcd.setCursor(0, 0); // setez cursorul pe primul rând
  lcd.print("Introdu parola"); // afișez mesajul

  Key = myKeypad.getKey(); // iau tasta pe care am apăsat

  if (Key)
  {
    Particular[Particular_Count] = Key;
    lcd.setCursor(Particular_Count, 1); // setez cursorul pe LCD
    lcd.print("*"); // afișez * în loc de parolă
    Particular_Count++; // numără lungimea parolei
  }
}
if (Particular_Count == Password_Length - 1) // verifică dacă lungimea parolei este corectă
{
  if (!strcmp(Particular, Specific)) // compară parola introdusă cu cea corectă
  {
    lcd.clear();
    ServoOpen(); // mută servomotorul la 180 grade

    lcd.setCursor(2, 0); // setează cursorul pe LCD
    lcd.print("Obj deschis"); // afișează mesajul "Obiect deschis"

    lcd.createChar(1, Opened); // creează caracterul pentru obiect deschis
    lcd.setCursor(14, 0);
    lcd.write(1); // afișează caracterul obiect deschis

    lcd.setCursor(0, 1);
    lcd.print("Apasa # sa inchizi"); // afișează mesajul pentru închidere

    SmartDoor = 0; // setează variabila pentru a indica faptul că ușa este deschisă
  }
}
else {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Parola incorecta"); // Afișează mesajul "Parola incorectă"
  lcd.setCursor(0, 1);
  lcd.print("Incearca in"); // Afișează mesajul "Încearcă în"

  lcd.setCursor(13, 1);
  lcd.print("10");
  delay(1000);

  lcd.setCursor(13, 1);
  lcd.print("09");
  delay(1000);

  lcd.setCursor(13, 1);
  lcd.print("08");
  delay(1000);

  lcd.setCursor(13, 1);
  lcd.print("07");
  delay(1000);

  lcd.setCursor(13, 1);
  lcd.print("06");
  delay(1000);

  lcd.setCursor(13, 1);
  lcd.print("05");
  delay(1000);

  lcd.setCursor(13, 1);
  lcd.print("04");
  delay(1000);

  lcd.setCursor(13, 1);
  lcd.print("03");
  delay(1000);

  lcd.setCursor(13, 1);
  lcd.print("02");
  delay(1000);
}
delay(1000);
lcd.setCursor(13, 1);
lcd.print("01");

delay(1000);
lcd.setCursor(13, 1);
lcd.print("00");

delay(1000);
lcd.clear();

SmartDoor = 1; // închide

clearData(); // curăță datele
