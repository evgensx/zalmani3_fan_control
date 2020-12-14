/*  По умолчанию позиция переключателя 2- минимальная скорость вращения
 *  Соотношение позиции и напряжения на контакте идущего на Gate npn Mosfeta
 *  Pos | Volt  | PWM
 *  -----------------
 *  1   | 0     | 0
 *  2   | 1.556 | 85
 *  3   | 3.509 | 170
 *  4   | 5.073 | 255
 *  Частота internal osc. 9.6 Мгц по умолчанию 
 *  Ядро Gyver Core
 *  Микросхема Attiny 13A
 *  Программатор Usbasp microCore
 *  Распиновка
 *  PB5 -1* 8- Vcc
 *  PB3 -2  7- PB2 fan speed button -
 *  PB4 -3  6- PB1 button fan speed +
 *  GND -4  5- PB0 pwm*/
 
// Задаём номера выводов:
const int bPlusPin  = PB1;
const int bMinusPin = PB2; 
const int pwmPin = PB0; 
byte pos = 2; // Позиция переключателя 1 - 4
bool flagBtnPlus = false;
bool flagBtnMinus = false;

void switchFun(byte pos) { // Переключение скорости
  switch (pos) {
    case 1:
      analogWrite(pwmPin, 0);
      break;
    case 2:
      analogWrite(pwmPin, 85);
      break;
    case 3:
      analogWrite(pwmPin, 170);
      break;
    case 4:
      analogWrite(pwmPin, 255);
      break;
  }
}

void setup() {
  pinMode(bPlusPin, INPUT);
  pinMode(bMinusPin, INPUT);
  pinMode(pwmPin, OUTPUT);
  
  switchFun(pos);
}

void loop() {
  // читаем инвертированное значение для удобства
  bool btnPlusState  = !digitalRead(bPlusPin); // считываем состояние кнопки +
  bool btnMinusState = !digitalRead(bMinusPin); // считываем состояние кнопки -
  
  if (btnPlusState && !flagBtnPlus) { // pos < 4
    flagBtnPlus = true;
    if (pos < 4) {
      analogWrite(pwmPin, ++pos); // скорость вентиля при нажатии кнопки +
      switchFun(pos); // Переключаем скорость
    }
  }
  if (!btnPlusState && flagBtnPlus) {
    flagBtnPlus = false; // обработчик отпускания кнопки +
  }
  
  if (btnMinusState && !flagBtnMinus) { // pos > 1
    flagBtnMinus = true;
    if (pos > 1) {
      analogWrite(pwmPin, pos--); // понижение скорости вентиля при нажатии кнопки -
      switchFun(pos);
    }
  }
  if (!btnMinusState && flagBtnMinus) {
    flagBtnMinus = false; // обработчик отпускания -
  }
}
