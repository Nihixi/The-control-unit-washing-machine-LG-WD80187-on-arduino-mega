//---------------------------- отображение информации и опрос кнопок -----------------------
// Одно срабатывание таймера, вывод одного разряда, displayBuf или опрос одной линии кнопок
// Порядок вывода информации на дисплей:
// Выводимая информация должна находится в массиве displayBuf
// 1. Сначала гасим текущий элемент offPins()
// 2. Формируем вывод setSegments(int n)
// 3. Включаем нужный элемент setPins()


int segPins[] = {26, 28, 30, 34, 32, 36, 40};                   // { A B C D E F G ) семисегментный индикатор
int displayPins[] = {39, 27, 25, 23, 29, 31, 33, 35};           // pin'ы для управления разрядами
int curr_digit_number = 0;                                      // Текущий разряд дисплея, который выводится в этом цикле таймера


/*
    A
   ---
F |   | B
  | G |
   ---
E |   | C
  |   |
   ---
    D
 */

byte segCode[24][7] = {
//  A  B  C  D  E  F  G 
  { 1, 1, 1, 1, 1, 1, 0 },  // 0
  { 0, 1, 1, 0, 0, 0, 0 },  // 1
  { 1, 1, 0, 1, 1, 0, 1 },  // 2
  { 1, 1, 1, 1, 0, 0, 1 },  // 3
  { 0, 1, 1, 0, 0, 1, 1 },  // 4
  { 1, 0, 1, 1, 0, 1, 1 },  // 5
  { 1, 0, 1, 1, 1, 1, 1 },  // 6
  { 1, 1, 1, 0, 0, 0, 0 },  // 7
  { 1, 1, 1, 1, 1, 1, 1 },  // 8
  { 1, 1, 1, 1, 0, 1, 1 },  // 9
  { 1, 1, 1, 0, 1, 1, 1 },  // A
  { 1, 0, 0, 1, 1, 1, 0 },  // C
  { 1, 0, 0, 1, 1, 1, 1 },  // E
  { 1, 0, 0, 0, 1, 1, 1 },  // F
  { 0, 1, 1, 0, 1, 1, 1 },  // H
  { 0, 0, 0, 1, 1, 1, 0 },  // L
  { 1, 1, 0, 0, 1, 1, 1 },  // P
  { 0, 1, 1, 1, 1, 1, 0 },  // U
  { 0, 1, 1, 1, 1, 0, 1 },  // d
  { 0, 0, 1, 0, 1, 0, 1 },  // n
  { 0, 0, 0, 0, 1, 0, 1 },  // r
  { 0, 0, 0, 1, 1, 1, 1 },  // t
  { 0, 0, 0, 0, 0, 0, 0 },  // пробел
  { 0, 0, 0, 0, 0, 0, 1 }   // -
};

void setProrgam(int n_prog) {
  current_program = n_prog;
  for (int i=0; i < 19; i++) {
    if (i == n_prog) {
      prog[i] = 1;
    } else {
      prog[i] = 0;
    }
  }  
}

int getCode(String str) {
  int ret = 0;
  if (str != "0") {
    int x = str.toInt();
    if (x >= 1 && x <= 9) {
      ret = x;
    } else {
      if (str == "A") ret = 10;
      if (str == "C") ret = 11;
      if (str == "E") ret = 12;
      if (str == "F") ret = 13;
      if (str == "H") ret = 14;
      if (str == "L") ret = 15;
      if (str == "P") ret = 16;
      if (str == "U") ret = 17;
      if (str == "d") ret = 18;
      if (str == "n") ret = 19;
      if (str == "r") ret = 20;
      if (str == "t") ret = 21;
      if (str == " ") ret = 22;
      if (str == "-") ret = 23;
    }
  }
  return ret;
}

void display_init() {
  for (int i=0; i < sizeof(segPins) / sizeof(int); i++)  {
    pinMode(segPins[i], OUTPUT);
  }
  for (int i=0; i < sizeof(displayPins) / sizeof(int); i++)  {
    pinMode(displayPins[i], OUTPUT);
  }
  for (int i=3; i < sizeof(displayBuf) / sizeof(int); i++)  {
    displayBuf[i] = 0;
  }
    displayBuf[0] = 22;
    displayBuf[1] = 22;
    displayBuf[2] = 22;
}

// Вывод цифр
void setSegments(int n) {
  for (int i=0; i < 7; i++) {
    digitalWrite(segPins[i], segCode[n][i]);
  }
}

// Включение нужных светодиодов
void setSegments2(int n) {
  for (int i=0; i < 7; i++) {
    digitalWrite(segPins[i], bitRead(n, i));
  }
}

void offPins() {
  digitalWrite(displayPins[curr_digit_number], LOW);  
}

void setPins() {
  curr_digit_number++;
  if (curr_digit_number > sizeof(displayPins) / sizeof(int) - 1) {
    curr_digit_number = 0;
  }
  digitalWrite(displayPins[curr_digit_number], HIGH);  
}

void setInfoDisplay() {
  
  int db3 = 0;
  int db4 = 0;
  int db5 = 0;
  int db6 = 0;
  int db7 = 0;

  bitWrite(db7, 0, prog[0]);
  bitWrite(db7, 1, prog[1]);
  bitWrite(db7, 2, prog[2]);
  bitWrite(db7, 3, prog[3]);
  bitWrite(db7, 4, prog[4]);
  bitWrite(db7, 5, prog[5]);
  bitWrite(db7, 6, prog[6]);
  bitWrite(db6, 0, prog[7]);
  bitWrite(db6, 1, prog[8]);
  bitWrite(db6, 2, prog[9]);
  bitWrite(db6, 3, prog[10]);
  bitWrite(db6, 4, prog[11]);
  bitWrite(db6, 5, prog[12]);
  bitWrite(db6, 6, prog[13]);
  bitWrite(db5, 0, prog[14]);
  bitWrite(db5, 1, prog[15]);
  bitWrite(db5, 2, prog[16]);
  bitWrite(db5, 3, prog[17]);
  bitWrite(db5, 4, prog[18]);
  bitWrite(db5, 5, fl_Bio);
  bitWrite(db5, 6, fl_PoloskanirPlus);
  bitWrite(db4, 0, fl_Del_payten);
  bitWrite(db4, 1, fl_Easy);
  bitWrite(db4, 2, fl_800);
  bitWrite(db4, 3, fl_600);
  bitWrite(db4, 4, fl_500);
  bitWrite(db4, 5, fl_400);
  bitWrite(db4, 6, fl_0);
  bitWrite(db3, 0, fl_Wait);
  bitWrite(db3, 3, fl_StartStop);
  bitWrite(db3, 4, fl_BlockB);
  bitWrite(db3, 5, fl_Dots);
  
  displayBuf[0] = indicator[0];
  displayBuf[1] = indicator[1];
  displayBuf[2] = indicator[2];
  displayBuf[3] = db3;
  displayBuf[4] = db4;
  displayBuf[5] = db5;
  displayBuf[6] = db6;
  displayBuf[7] = db7;
}

// Отображение одного разряда, вызывается из таймера
void viewDisplay() {
    offPins();
    // Вывод информации
    if (curr_digit_number < 3) {
      setSegments(displayBuf[curr_digit_number]); // Вывод цифр
    } else {
      setSegments2(displayBuf[curr_digit_number]); // Вывод информации на светодиоды
    }
    setPins();
    getEncoder();   // Опрос энкодера
    getButton();    // Опрос кнопок
}

// Элюминация и инициализация параметров отображения дисплея при включения стиральной машины
void DisplayStart() {
  indicator[0] = 23; // - 
  indicator[1] = 23; // - 
  indicator[2] = 23; // - 

  indicator[0] = 22; // 
  indicator[1] = 14; // H
  indicator[2] = 1;  // 1

  // Включим светодиоды энкодера
  for (int i=0; i < 19; i++) {
    prog[i] = 1;
    setInfoDisplay();
    delay2(50);
  }
  // Выключим светодиоды энкодера
  for (int i=0; i < 19; i++) {
    prog[i] = 0;
    setInfoDisplay();
    delay2(50);
  }

  // Устанавливаем, программу умолчанию
  for (int i=0; i <= DEFAULT_PROG; i++) {
    prog[i] = 1;
    if (i > 0) {
      prog[i - 1] = 0;
    }
    setInfoDisplay();
    delay2(50);
  }
  
  // Устанавливаем значение энкодера на программу по умолчанию
  current_program = DEFAULT_PROG;
  encoder_value = DEFAULT_PROG * 4;

  // Включим режимы по умолчанию
  fl_StartStop = 1;
  fl_BlockB = 1;
  fl_PoloskanirPlus = 0;
  setSpin();
  mode_child_lock = 0;

  getTotalTime(0, 0);
  setInfoDisplay();
}
