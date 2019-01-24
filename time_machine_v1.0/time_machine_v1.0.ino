#include"pitches.h"
#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>
SoftwareSerial mySerial(19, 18); // RX, TX

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 8
#define NUM_LEDS 68
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

#define VOL 15 // громкость ээфектов ( 0-30)

// СВЕТ НА ДВЕРИ
#define MACHINE_LED 2
#define OLDTIME_LED 3 
#define MIDLETIME_LED 4
#define USSR_LED 5
#define FUTURE_LED 6
#define EXIT_LED 7
#define POWERON_RELE 37
//ОТКРЫТИЕ ДВЕРЕЙ
#define EXIT_DOOR 34
#define OLDTIME_DOOR 35
#define FUTURE_DOOR 36
#define MIDLETIME_DOOR 39
#define USSR_DOOR 38
// КНОПКИ 
#define OURTIME_BUT 52 // если сигнал пришел - в нашей эре иначе не в нашей 
#define ONE 43
#define TWO 42
#define THREE 48
#define FOUR 40
#define FIVE 44
#define SIX 49
#define SEVEN 41
#define EIGHT 45
#define NINE 46
#define ZERO 47
#define PUSK 53
#define NAZAD 50
#define VKL 51

int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

bool waiting = 1; // флаг говорит что действий нет
byte somethingHorible[15]; // состояния каждой из кнопок ( тумблеров)
int buttonStates[14];
unsigned long ms = 0;

byte auto_dzin = 0; // отигрывание пищалок
unsigned long autoDzinMs = 0;

byte auto_main = 0; // основной алгоритм программы
unsigned long autoMainMs =0;
byte machineLedCur = 0;

byte auto_main_prev =99;

byte auto_startMachine = 0; //  автомат самый глобальный. отвечает за вкл\ откл машины

unsigned int toneFreq = 500; // частота звучания для машины
unsigned long msRecharge = 0; // время удержания кнопок для перезарядки 
void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
  mySerial.begin (9600);
  mp3_set_serial (mySerial); //set Serial for DFPlayer-mini mp3 module 
  mp3_set_volume (15);
  delay(100);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
// начальное заполнение массива состояний кнопок
for (int i = 0; i<14; i++){
  buttonStates[i] = 1;
}
  Serial.begin(115200);

// конфигурация пинов
for(int i = 40; i<=53; i++){
Serial.println(i);
  pinMode(i, INPUT_PULLUP);
}
for(int i = 34; i<=39; i++){
Serial.println(i);
  pinMode(i, OUTPUT);
  digitalWrite(i, 1);

}
//rainbowCycle(1);

/* chek ability of DF player
mp3_play(1);
delay(500);
mp3_stop();
*/
}//setup

void loop() {
ms = millis();

if(auto_main_prev != auto_main){
  auto_main_prev = auto_main;
  Serial.print("aut=");
  Serial.println(auto_main);
}
// перезарядка 
if((!digitalRead(NAZAD)) && (!digitalRead(PUSK)) && (!digitalRead(ZERO)) && digitalRead(VKL)){
  analogWrite(MACHINE_LED, 0);
  tone(10, 800, 18); // tone (pin, freq, dur)
  delay(20);
  noTone(10);
  analogWrite(MACHINE_LED, 250);
  msRecharge = ms;
  while((!digitalRead(NAZAD)) && (!digitalRead(PUSK)) && (!digitalRead(ZERO))&& digitalRead(VKL)){
    if((millis() - msRecharge) > 3000){
      mp3_set_volume(15);
      mp3_play (random(5, 12));
      msRecharge = ms;
      analogWrite(MACHINE_LED, 0);
      delay(300);
      analogWrite(OLDTIME_LED, 250);
      delay(300);
      analogWrite(MIDLETIME_LED, 250);
      delay(300);
      analogWrite(USSR_LED, 250);
      delay(300);
      analogWrite(FUTURE_LED, 250);
      delay(300);
      analogWrite(EXIT_LED, 250);
      delay(1000);
      analogWrite(MACHINE_LED, 250);
      
      //чтобы в локации горел свет
      while(!digitalRead(ZERO)); // ждем пока оператор не опустит кнопку ноль в позицию выключено
      mp3_stop ();
      analogWrite(OLDTIME_LED, 0);
      analogWrite(MIDLETIME_LED, 0);
      analogWrite(USSR_LED, 0);
      analogWrite(FUTURE_LED, 0);
      analogWrite(EXIT_LED, 0);
      analogWrite(MACHINE_LED, 250);
      digitalWrite(EXIT_DOOR, 0);
      delay(3000);
      digitalWrite(EXIT_DOOR, 1);
      for(int i = 0; i < 68; i++){
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }//for all strip      
      strip.show(); // This sends the updated pixel color to the hardware.        
      waiting = 1;
      auto_main = 0; // ПЕРЕЗАРЯДКА АВТОМАТА МАШИНЫ
      break;
    }// if ms

  }// while recharge 
}//if recharge

switch(auto_startMachine){
// глобальный автомат, который крутит в себе все остальные автоматы, когда машину включат
case 0:
// инициализаци 
analogWrite(MACHINE_LED, 255);

delay(300);
auto_startMachine = 3;
break;
case 1:
// остановка работы машины
for(int i =500; i>25; i--){
      tone(10, i*10, 5); // tone (pin, freq, dur)
    delay(6);
    noTone(10);
}//for
digitalWrite(POWERON_RELE, 1); // выключаем машину
auto_startMachine = 0; // го - инициализируемся по новой
break;
case 3:
// ожидание включения машины
if(!digitalRead(VKL)){
  delay(20);
  if(!digitalRead(VKL)){
  auto_startMachine = 5; // го на запуск машины 
}} // if VKL button switched ON
break;
case 5:
// звук запуска машины
for(int i =25; i<500; i++){
      tone(10, i*10, 5); // tone (pin, freq, dur)
    delay(6);
    if(i==200) digitalWrite(POWERON_RELE, 0); // включаем кулеры машины
    noTone(10);
}//for
auto_startMachine = 10; // го на работу основного алгоритма машины
break;
case 10:
// отработка всех основных автоматов машины внутри глобального автомата startMachine
// но если кнопку отключили, возвращаемся на начало
if(digitalRead(VKL)){
  delay(20);
  if(digitalRead(VKL)){
  auto_startMachine = 1; // го на выключение  
}} // if VKL button switched off

    //проверяем любые нажатия на кнопки и генерим для них рандомный звук
    if(waiting){
      for(int i = 40; i<=53; i++){
        if (digitalRead(i) != buttonStates[i]){
          delay(20);
          if (digitalRead(i) != buttonStates[i]){
            buttonStates[i] = digitalRead(i);
            auto_dzin = 5; // to sound random tone
        }}//if any button
      }//for 14 keys
    }//waiting 
    
    // отигрывание звуков
    switch(auto_dzin){
      case 0:
      autoDzinMs = ms;
      break;
      case 5:
        tone(10, random(100, 5000), 100);
        delay(100);
        noTone(10);
        auto_dzin = 0;
      break;
    }
    // основная последовательность игры
    
    switch(auto_main){
      case 0:
      autoMainMs = ms;
      auto_main = 5;
      break;
      case 5:
      // включаем подсветку машины 
      if((ms - autoMainMs) > 5){
        autoMainMs = ms;
        if(machineLedCur <255){
          machineLedCur++;
          analogWrite(MACHINE_LED, machineLedCur);
        } else { 
          // свет уже на полную
          auto_main = 7;
        }
      }//if ms
      break;
      case 7:
      // ожидание одной из четырех комбинаций 
      // 26138 до нашей эры  либо 1976, 1528, 2148 нашей эры
      if(!digitalRead(PUSK)){
        delay(20);
        if(!digitalRead(PUSK)){
          // ожидаем 26138 год до нашей эры   
          if(digitalRead(OURTIME_BUT) && (!digitalRead(ONE)) && (!digitalRead(TWO)) && (!digitalRead(THREE)) && (digitalRead(FOUR)) && (digitalRead(FIVE)) && (!digitalRead(SIX)) && (digitalRead(SEVEN)) && (!digitalRead(EIGHT)) && (digitalRead(NINE)) && (digitalRead(ZERO))  ){
            autoMainMs = ms;
            waiting = 0; // запрещаем режим ожидания ( для бибиканий тумблерами)
            auto_main = 11; // go эффекты 
          // ожидаем 1976  год нашей эры  
          } else if(!digitalRead(OURTIME_BUT) && (!digitalRead(ONE)) && (digitalRead(TWO)) && (digitalRead(THREE)) && (digitalRead(FOUR)) && (digitalRead(FIVE)) && (!digitalRead(SIX)) && (!digitalRead(SEVEN)) && (digitalRead(EIGHT)) && (!digitalRead(NINE)) && (digitalRead(ZERO))  ){
            autoMainMs = ms;
            waiting = 0; // запрещаем режим ожидания ( для бибиканий тумблерами)
            auto_main = 21; // go эффекты 
          // ожидаем 1528 год
          } else if(!digitalRead(OURTIME_BUT) && (!digitalRead(ONE)) && (!digitalRead(TWO)) && (digitalRead(THREE)) && (digitalRead(FOUR)) && (!digitalRead(FIVE)) && (digitalRead(SIX)) && (digitalRead(SEVEN)) && (!digitalRead(EIGHT)) && (digitalRead(NINE)) && (digitalRead(ZERO))  ){
            autoMainMs = ms;
            waiting = 0; // запрещаем режим ожидания ( для бибиканий тумблерами)
            auto_main = 31; // go эффекты 
          // ожидаем 2148 год нашей эры    
          } else if(!digitalRead(OURTIME_BUT) && (!digitalRead(ONE)) && (!digitalRead(TWO)) && (digitalRead(THREE)) && (!digitalRead(FOUR)) && (digitalRead(FIVE)) && (digitalRead(SIX)) && (digitalRead(SEVEN)) && (!digitalRead(EIGHT)) && (digitalRead(NINE)) && (digitalRead(ZERO))  ){
            autoMainMs = ms;
            waiting = 0; // запрещаем режим ожидания ( для бибиканий тумблерами)
            auto_main = 41; // go эффекты 
          }// right combination

          
          
          // right combination
      }}//if PUSK
      break;
      case 11:
      // переходные эффекты перед первой комнатой
      // тухнем на машине, мигаем светодиодной лентой
      // рандомные звуки из зумера 
      if((ms - autoMainMs) > 20){
        autoMainMs = ms;
        if(machineLedCur){
          machineLedCur--;
          analogWrite(MACHINE_LED, machineLedCur);
          // когда свет почти потух, включаем музыку машины с плеера
          if(machineLedCur == 50){
            mp3_set_volume(VOL);
            mp3_play(1);
          }//if led == 50
          // случайный светодиод на случайную яркость цвета
          strip.setPixelColor(random(250), strip.Color(random(250), random(1*(machineLedCur/50)), random(50*(machineLedCur/50))));
          strip.show(); // This sends the updated pixel color to the hardware.
          noTone(10);
          tone(10, random(100, 5000), 20);
        }// if machine still lightning
        else {
          // по окончанию запуск ленты и на разжигание двери
            rainbowCycle(2);
            rainbowCycle(2);
          autoMainMs = ms;
          auto_main = 13;
        }
      }//if ms
      break;
      case 13:
      if((ms - autoMainMs) > 20){
        autoMainMs = ms;
        // спустя секунду нажимаем на кнопку двери
        if(machineLedCur > 50){
          digitalWrite(OLDTIME_DOOR, 0); // нажимаем кнопку открывания двери
        }
        // плавно разгораемся светом на комнату и лентой тухнем
        if(machineLedCur < 255){
          machineLedCur++;
          // свет на дверь в прошлое
          analogWrite(OLDTIME_LED ,machineLedCur);  
          // все светодиоды на ленте плавно на максимум 
          for(int i = 0; i < 68; i++){
            strip.setPixelColor(i, strip.Color((255-machineLedCur), (255-machineLedCur), (255-machineLedCur)));
            strip.show(); // This sends the updated pixel color to the hardware.        
          }//for all strip  
          //плавно увеличиваем частоту звука на перемещение 
        }else{ 
          digitalWrite(OLDTIME_DOOR, 1); // отпускаем кнопку открывания двери
          analogWrite(MACHINE_LED, 1);      
          autoMainMs = ms;
          waiting = 1;
          auto_main = 14; // го на плавное разгорание света в машине времени 
        }
      }//if ms
      break;
      case 14:
      // на случай если не успели зайти в комнату
                                          if(!digitalRead(PUSK)){
                                            delay(20);
                                            if(!digitalRead(PUSK)){
                                              digitalWrite(OLDTIME_DOOR, 0); // жмем кнопку открывания двери
                                              delay(100);
                                              digitalWrite(OLDTIME_DOOR, 1); // отпускаем кнопку открывания двери
                                          }}//if but 
      // плавно тушим свет
      if(((ms - autoMainMs) > 30) && (machineLedCur > 1)){
        autoMainMs = ms;
        machineLedCur--;
        analogWrite(OLDTIME_LED ,machineLedCur);  
      }  //if ms for led
      // ЧЕРЕЗ 2 МИНУТЫ ГО НЕКСТ
      if((ms - autoMainMs) > 33333){
        autoMainMs = ms;
        analogWrite(OLDTIME_LED ,0);  
         auto_main = 15;      // го на жидание следующей комнаты 
      }//if ms
      break;
      // этап на СССР
      case 15:
      // включаем подсветку машины 
      if((ms - autoMainMs) > 5){
        autoMainMs = ms;
        if(machineLedCur <255){
          machineLedCur++;
          analogWrite(MACHINE_LED, machineLedCur);
        } else { 
          // свет уже на полную
          auto_main = 7; // на исходную 
        }
      }//if ms
      break;

      
      case 21:
      // переходные эффекты перед первой комнатой
      // тухнем на машине, мигаем светодиодной лентой
      if((ms - autoMainMs) > 20){
        autoMainMs = ms;
        if(machineLedCur){
          machineLedCur--;
          analogWrite(MACHINE_LED, machineLedCur);
          // когда свет почти потух, включаем музыку машины с плеера
          if(machineLedCur == 50){
            mp3_set_volume(VOL);
            mp3_play(2);
          }//if led == 50
          // случайный светодиод на случайную яркость цвета
          strip.setPixelColor(random(250), strip.Color(random(250), random(1*(machineLedCur/50)), random(1*(machineLedCur/50))));
          strip.show(); // This sends the updated pixel color to the hardware.
          noTone(10);
          tone(10, random(100, 5000), 20);
        }// if machine still lightning
        else {
          // по окончанию запуск ленты и на разжигание двери
          rainbowCycle(2);
          rainbowCycle(2);
          autoMainMs = ms;
          auto_main = 23;
        }
      }//if ms
      break;
      case 23:
      if((ms - autoMainMs) > 20){
        autoMainMs = ms;
        // спустя секунду нажимаем на кнопку двери
        if(machineLedCur > 50) digitalWrite(USSR_DOOR, 0); // нажимаем кнопку открывания двери
        // плавно разгораемся светом на комнату и лентой тухнем
        if(machineLedCur < 255){
          machineLedCur++;
          // свет на дверь в прошлое
          analogWrite(USSR_LED ,machineLedCur);  
          // все светодиоды на ленте на максимум 
          for(int i = 0; i < 68; i++){
            strip.setPixelColor(i, strip.Color((255-machineLedCur), (255-machineLedCur), (255-machineLedCur)));
            strip.show(); // This sends the updated pixel color to the hardware.        
          }//for all strip      
        }else{ 
          digitalWrite(USSR_DOOR, 1); // отпускаем кнопку открывания двери
          analogWrite(USSR_LED, 1);  
          analogWrite(MACHINE_LED, 1);      
          autoMainMs = ms;
          machineLedCur = 0;
          waiting = 1;
          auto_main = 24; // го на плавное разгорание света в машине времени 
        }
      }//if ms
      break;
      case 24:
      // на случай если не успели зайти в комнату
                                          if(!digitalRead(PUSK)){
                                            delay(20);
                                            if(!digitalRead(PUSK)){
                                              digitalWrite(USSR_DOOR, 0); // жмем кнопку открывания двери
                                              delay(100);
                                              digitalWrite(USSR_DOOR, 1); // отпускаем кнопку открывания двери
                                          }}//if but 
      // ЧЕРЕЗ 2 МИНУТЫ ГО НЕКСТ
      if((ms - autoMainMs) >               333){
        autoMainMs = ms;
        analogWrite(USSR_LED ,0);  
         auto_main = 25;      // го на жидание следующей комнаты 
      }//if ms
      break;
      // этап на Средневековье
      case 25:
      // включаем подсветку машины 
      if((ms - autoMainMs) > 5){
        autoMainMs = ms;
        if(machineLedCur <255){
          machineLedCur++;
          analogWrite(MACHINE_LED, machineLedCur);
        } else { 
          // свет уже на полную
          auto_main = 7; // на исходную
        }
      }//if ms
      break;

      
      case 31:
      // переходные эффекты перед первой комнатой
      // тухнем на машине, мигаем светодиодной лентой
      if((ms - autoMainMs) > 20){
        autoMainMs = ms;
        if(machineLedCur){
          machineLedCur--;
          analogWrite(MACHINE_LED, machineLedCur);
          // когда свет почти потух, включаем музыку машины с плеера
          if(machineLedCur == 50){
            mp3_set_volume(VOL);
            mp3_play(3);
          }//if led == 50
          // случайный светодиод на случайную яркость цвета
          strip.setPixelColor(random(250), strip.Color(random(1*(machineLedCur/50)), random(250), random(1*(machineLedCur/50))));
          strip.show(); // This sends the updated pixel color to the hardware.
          noTone(10);
          tone(10, random(100, 5000), 20);
        }// if machine still lightning
        else {
          // по окончанию запуск ленты и на разжигание двери
          rainbowCycle(2);
          rainbowCycle(2);
          autoMainMs = ms;
          auto_main = 33;
        }
      }//if ms
      break;
      case 33:
      if((ms - autoMainMs) > 20){
        autoMainMs = ms;
        // спустя секунду нажимаем на кнопку двери
        if(machineLedCur > 50) digitalWrite(MIDLETIME_DOOR, 0); // нажимаем кнопку открывания двери
        // плавно разгораемся светом на комнату и лентой тухнем
        if(machineLedCur < 255){
          machineLedCur++;
          // свет на дверь в прошлое
          analogWrite(MIDLETIME_LED ,machineLedCur);  
          // все светодиоды на ленте на максимум 
          for(int i = 0; i < 68; i++){
            strip.setPixelColor(i, strip.Color((255-machineLedCur), (255-machineLedCur), (255-machineLedCur)));
            strip.show(); // This sends the updated pixel color to the hardware.        
          }//for all strip      
        }else{ 
          digitalWrite(MIDLETIME_DOOR, 1); // отпускаем кнопку открывания двери
          analogWrite(MIDLETIME_LED, 1);  
          analogWrite(MACHINE_LED, 1);      
          autoMainMs = ms;
          machineLedCur = 0;
          waiting = 1;
          auto_main = 34; // го на плавное разгорание света в машине времени 
        }
      }//if ms
      break;
      case 34:
      // на случай если не успели зайти в комнату
                                          if(!digitalRead(PUSK)){
                                            delay(20);
                                            if(!digitalRead(PUSK)){
                                              digitalWrite(MIDLETIME_DOOR, 0); // жмем кнопку открывания двери
                                              delay(100);
                                              digitalWrite(MIDLETIME_DOOR, 1); // отпускаем кнопку открывания двери
                                          }}//if but 
      // ЧЕРЕЗ 2 МИНУТЫ ГО НЕКСТ
      if((ms - autoMainMs) > 33333){
        autoMainMs = ms;
        analogWrite(MIDLETIME_LED ,0);  
         auto_main = 35;      // го на жидание следующей комнаты 
      }//if ms
      break;
      case 35:
      // включаем подсветку машины 
      if((ms - autoMainMs) > 5){
        autoMainMs = ms;
        if(machineLedCur <255){
          machineLedCur++;
          analogWrite(MACHINE_LED, machineLedCur);
        } else { 
          // свет уже на полную
          auto_main = 7; //  на исходную
        }
      }//if ms
      break;
 
      case 41:
      // переходные эффекты перед первой комнатой
      // тухнем на машине, мигаем светодиодной лентой
      if((ms - autoMainMs) > 20){
        autoMainMs = ms;
        if(machineLedCur){
          machineLedCur--;
          analogWrite(MACHINE_LED, machineLedCur);
          // когда свет почти потух, включаем музыку машины с плеера
          if(machineLedCur == 50){
            mp3_set_volume(VOL);
            mp3_play(4);
          }//if led == 50
          // случайный светодиод на случайную яркость цвета
          strip.setPixelColor(random(250), strip.Color(random(1*(machineLedCur/50)), random(1*(machineLedCur/50)), random(250)) );
          strip.show(); // This sends the updated pixel color to the hardware.
          noTone(10);
          tone(10, random(100, 5000), 20);
        }// if machine still lightning
        else {
          // по окончанию запуск ленты и на разжигание двери
          rainbowCycle(2);
          rainbowCycle(2);
          autoMainMs = ms;
          auto_main = 43;
        }
      }//if ms
      break;
      case 43:
      if((ms - autoMainMs) > 20){
        autoMainMs = ms;
        // спустя секунду нажимаем на кнопку двери
        if(machineLedCur > 50) digitalWrite(FUTURE_DOOR, 0); // нажимаем кнопку открывания двери
        // плавно разгораемся светом на комнату и лентой тухнем
        if(machineLedCur < 255){
          machineLedCur++;
          // свет на дверь в прошлое
          analogWrite(FUTURE_LED ,machineLedCur);  
          // все светодиоды на ленте на максимум 
          for(int i = 0; i < 68; i++){
            strip.setPixelColor(i, strip.Color((255-machineLedCur), (255-machineLedCur), (255-machineLedCur)));
            strip.show(); // This sends the updated pixel color to the hardware.        
          }//for all strip      
        }else{ 
          digitalWrite(FUTURE_DOOR, 1); // отпускаем кнопку открывания двери
          analogWrite(FUTURE_LED, 1);  
          analogWrite(MACHINE_LED, 1);      
          autoMainMs = ms;
          machineLedCur = 0;
          waiting = 1;
          auto_main = 44; // го на плавное разгорание света в машине времени 
        }
      }//if ms
      break;
      case 44:
      // на случай если не успели зайти в комнату
                                          if(!digitalRead(PUSK)){
                                            delay(20);
                                            if(!digitalRead(PUSK)){
                                              digitalWrite(FUTURE_DOOR, 0); // жмем кнопку открывания двери
                                              delay(100);
                                              digitalWrite(FUTURE_DOOR, 1); // отпускаем кнопку открывания двери
                                          }}//if but 
      // ЧЕРЕЗ 2 МИНУТЫ ГО НЕКСТ
      if((ms - autoMainMs) > 33333){ // даем им подольше времени на вход в комнату  
        autoMainMs = ms;
        analogWrite(MIDLETIME_LED ,0);  
         auto_main = 45;      // го на жидание следующей комнаты 
      }//if ms
      break;
      // этап ИДЕМ ИЗ КВЕСТА
      case 45:
      // включаем подсветку машины 
      if((ms - autoMainMs) > 5){
        autoMainMs = ms;
        if(machineLedCur <255){
          machineLedCur++;
          analogWrite(MACHINE_LED, machineLedCur);
          analogWrite(OLDTIME_LED, machineLedCur);
          analogWrite(MIDLETIME_LED, machineLedCur);
          analogWrite(USSR_LED, machineLedCur);
          analogWrite(FUTURE_LED, machineLedCur);
        } else { 
          // свет уже на полную
          auto_main = 47;
        }
      }//if ms
      break;
      case 47:
      // выпускаем из локации
      if(!digitalRead(NAZAD)){
        delay(20);
        if(!digitalRead(NAZAD)){
          analogWrite(MACHINE_LED, 50);
          analogWrite(OLDTIME_LED, 0);
          analogWrite(MIDLETIME_LED, 0);
          analogWrite(USSR_LED, 0);
          analogWrite(FUTURE_LED, 0);
          analogWrite(EXIT_LED, 255);
          digitalWrite(EXIT_DOOR, 0);
          rainbowCycle(10);
          digitalWrite(EXIT_DOOR, 1);
      }}
      break;
    }//switch auto_main
break; // case of main auto - startMachine
}//auto_startMachine
} //loop













// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
