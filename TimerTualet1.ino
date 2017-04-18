// таймер включения вентилятора в зависимости от выключателя света
// 2 коротких включения - вентилятор работает 3 минуты
// длительное включение света > 1,5 минут - вентилятор включается и работает ещё 5 минут после выключения света 
bool     button_state      = false;
bool     button_long_state = false;
uint32_t ms_button = 0;
int dobl_Pres = 0;
long lastMsg = 0;
int Period_3Minuti = 180;  // проветриваем 3 минуты
int Period_5Minut = 300;  // проветриваем 5 минут
int timeCount_3Minuti = 0, timeCount_5Minut = 0;
int Shotr_Pres = 0, Long_press =  0;
uint32_t  ms_flash = 0; 
int ledState = HIGH;
 
void setup()
{
   Serial.begin(9600);
   pinMode(13, OUTPUT);
   pinMode(10, OUTPUT); // Выход на тиристор
   pinMode(9, INPUT);   // Вход с делителя сетевой напруги
}
 
void loop(){
   uint32_t ms    = millis();
   bool pin_state = digitalRead(9);
   
   if( (ledState == HIGH) && ( ms - ms_flash ) >= 20 ){  // мигаем светодиодом раз в 5 сек
      ledState = LOW; // выключаем
      ms_flash = ms;
      digitalWrite(13, ledState);     
   }
   else if ((ledState == LOW) && (ms - ms_flash >= 5000)){  // мигаем светодиодом раз в 5 сек 
      ledState = HIGH; // выключаем
      ms_flash = ms;
      digitalWrite(13, ledState);
   }
// Фиксируем нажатие кнопки   
   if( pin_state  == HIGH && !button_state && ( ms - ms_button ) > 250 ){
      button_state      = true;
      button_long_state = false;  
      ms_button         = ms;  
   }
// Фиксируем длинное нажатие кнопки   
   if( pin_state  == HIGH && !button_long_state && ( ms - ms_button ) > 99990 ){ // 1,5 мин. - длинное включение света
      button_long_state = true;
//      Serial.println("Long press key");   
      Long_press = 1;
      dobl_Pres = 0; 
      timeCount_5Minut = 0;
   }
// Фиксируем отпускание кнопки   
   if( pin_state == LOW && button_state && ( ms - ms_button ) > 250  ){
      button_state      = false;   
      ms_button         = ms;
      if( !button_long_state ){
//        Serial.println("Press key");  // Фиксируем выключения света
        dobl_Pres = dobl_Pres + 1;
        if (dobl_Pres == 2){          // Фиксируем двойное выключения светаи 
            dobl_Pres = 0;
            Shotr_Pres = 1;
//            Serial.println("Shotr_Pres");
         }
      }     
   }

   if( pin_state == LOW  && ( ms - ms_button ) > 2000  ){ // прошло 1,2 сек. и свет не включен - 
      dobl_Pres = 0;                                                      // сбрасываем двойное нажатие
//      Shotr_Pres = 0;
//      Serial.println("Shotr_Pres = 0000");
      ms_button         = ms;
   }
   
   if (ms - lastMsg >= 1000) {  // 1секунда
//     Serial.print(" c");
     lastMsg = ms;
     if (Shotr_Pres == 1 && pin_state == LOW){  // есть двойноевыключения света - включаем вентилятор
        timeCount_3Minuti = timeCount_3Minuti + 1;
        digitalWrite(10, HIGH);
//        digitalWrite(13, HIGH);
//        Serial.println("Shotr_Pres = 1");
     }
     if (timeCount_3Minuti > Period_3Minuti && pin_state == LOW){ // 3 мин. прошло и свет не горит - вЫключаем вентилятор
        timeCount_3Minuti = 0;
        Shotr_Pres = 0;
        digitalWrite(10, LOW);
 //       digitalWrite(13, LOW);
 //       Serial.println("Shotr_Pres = 0");
     }
     if (Long_press == 1 ){             // есть длинное выключения света - включаем вентилятор
        digitalWrite(10, HIGH);
//        digitalWrite(13, HIGH);
//        Serial.println("Long_Pres = 1");
        if (pin_state == LOW){          // свет не горит можно отсчитывать 5 минут
          timeCount_5Minut = timeCount_5Minut + 1;
//          Serial.println("pin_state == LOW");
        }
     }
     if (timeCount_5Minut > Period_5Minut && pin_state == LOW){ // 5 мин. прошло и свет не горит - вЫключаем вентилятор
        timeCount_5Minut = 0;
        Long_press= 0;
        digitalWrite(10, LOW);
//        digitalWrite(13, LOW);
 //       Serial.println("Long_Pres = 0");
     }
   }
}
