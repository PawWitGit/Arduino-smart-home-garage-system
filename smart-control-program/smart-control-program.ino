#include <Stepper.h>  //biblioteka sterująca silnikiem
#include <IRremote.h> //biblioteka dla pilotów IR
#define STEPS 32    //stała krokowa dla silnika
#define wykryto_ruch 4  //definicja zmiennych i pinów
#define osw_zewn 6
#define wlacznik_garaz 5
#define osw_wewn 7

Stepper stepper (STEPS, 8, 10, 9, 11);  //piny obsługujące            //cewki silnika, góra/dół
unsigned long czas = 5000;  //deklaracja czasu 
//załączenia dla zegara procesora

int krok = 0;     //dodanie zmiennej 
//zawierającej ilość kroków silnika 
int liczSilnik = 0; // licznik dla określenia pozycji silnika  
int licznik_przycisk1 = 20; //deklaracja wartości zerowej //licznika dla przycisku
int input_pin = 2;    //deklaracja pinu 2                //odbieranie sygnału z pilota  
IRrecv irrecv(input_pin); //deklaracja pinu 2 jako //dekodujący kod z pilota                        
decode_results signals;   //dekodowanie sygnałów HEX 

void setup () {
Serial.begin(9600); //uruchomienie transmisji danych//PC/Procesor
irrecv.enableIRIn();  //uruchomienie transmisji danych //z pilotem IR
  stepper.setSpeed (800); // ustawienie prędkości silnika
  pinMode(wykryto_ruch, INPUT); // deklaracja typów I/O
  pinMode(osw_zewn, OUTPUT);
  pinMode(wlacznik_garaz, INPUT_PULLUP);
  pinMode(osw_wewn, OUTPUT);
  digitalWrite(osw_zewn, LOW);
  
}
void loop () {
  zalacz_oswietlenie_zewnetrzne(); //wywołanie funkcji //sterującej oświetleniem zewnętrznym
  zalacz_oswietlenie_wewnetrzne();  // wywołanie funkcji //sterującej oświetleniem wewnętrznym
  sterowanie_bramy(); // wywołanie funkcji sterującej //silnikiem bramy
}

void zalacz_oswietlenie_zewnetrzne()
{
  if (digitalRead(wykryto_ruch) == HIGH) { //jeśli wykryto                    //ruch
    digitalWrite(osw_zewn, HIGH); //załącz oświetlnenie                      //zewnętrzne
czas = millis(); //odczekaj 5s, millis              
     //pozwala na przerwanie
    digitalWrite(osw_zewn, LOW);  //wyłącz oświetlenie                //zewnętrzne
  }
}

void zalacz_oswietlenie_wewnetrzne()
{
  if (licznik_przycisk1 == 22)  //czy wciśnięto przycisk                 //załącz 2gi raz
  {
    digitalWrite(osw_wewn, LOW);  //wyłącz oświetlenie                   //wewnętrzne
    licznik_przycisk1 = 20;  //wyzeruj licznik
  }
  else if (digitalRead(wlacznik_garaz) == LOW)        
    //jeśli wciśnięto przycisk załącz 
  {
    delay(300);  //zwłoka czasowa dla procesora
    licznik_przycisk1++;   //zwiększ licznik o 1
  }
  if (licznik_przycisk1 == 21)     //jeśli licznik = 21
  {
    digitalWrite(osw_wewn, HIGH); //załącz oświetlenie //wewnętrzne 
  }
}
void sterowanie_bramy()
{
if (irrecv.decode(&signals)) {  //dekodowanie sygnału //z pilota
    Serial.println(signals.decode_type);  //wyświetl na 
          //ekran kod z pilota                                             
    Serial.print(F("sygnal_to_ = 0x"));               
    Serial.println(signals.value, HEX); //wynik w HEX

    if (signals.value ==  0xFFE01F && liczSilnik == 1)  
//jeśli wciśnięto przycisk zamknij licznik równy 1
    {
      krok = 512;   //1/4 obrotu w prawo
      stepper.step (krok);  //zamknij bramę
      liczSilnik = 0; // blokada przed ponownym zamykaniem, 
      //brama zamknięta
    }
    else
    {
      digitalWrite(8, LOW);  //blokada silnika w pozycji //zamkniętej            
      digitalWrite(9, LOW); //wyłączenie uzwojeń silnika
      digitalWrite(10, LOW);
      digitalWrite(11, LOW);
    }
    if (signals.value == 0xFFA857 && liczSilnik == 0)   
// jeśli wciśnięto przycisk otwórz i licznik wyzerowany
    {
      krok = -512;  //1/4 obrotu w lewo
      stepper.step (krok);   //otwórz bramę
      liczSilnik++;  //blokada przed ponowny otwieraniem,
     //brama otwarta 
    }
    else
    {
      digitalWrite(8, LOW);  // blokada silnika w pozycji //otwartej 
      digitalWrite(9, LOW);  //wyłączenie uzwojeń silnika
      digitalWrite(10, LOW);
      digitalWrite(11, LOW);
    }
    irrecv.resume();  //wznowienie transmisji IR
  }
}
