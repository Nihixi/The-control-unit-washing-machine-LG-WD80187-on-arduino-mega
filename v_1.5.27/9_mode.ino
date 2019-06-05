//******************** Режимы/кнопки *******************************
void On_Mashine()  {
  addLog("Включение СМ");
  addLog("Настройка параметров...");

  mode_spin = 800;
  mode_child_lock = 0;
  fl_Dots = 0;                        // Флаг разделителя

  pwmOut = 0;
  Dimmer = 255;
  attachInterrupt(0, detect_up, LOW);         // настроить срабатывание прерывания переход через ноль interrupt0 на низкий уровень
  attachInterrupt(1, detect_tacho, FALLING);  // настроить срабатывание прерывания от таходатчика interrupt1 по спаду
  attachInterrupt(4, detect_water, FALLING);  // настроить срабатывание прерывания от пресостата

  display_init();
  DisplayStart();
}

void Off_Mashine() {
  addLog("Отключаем все электроприборы");
  digitalWrite(PIN_TEN, LOW);         // Отключаю ТЭН
  digitalWrite(PIN_BLOCK_DOOR, LOW);  // Снимаем блокировку с двери
  digitalWrite(PIN_MOTOR_LR, LOW);    // Отключаем реле реверса
  digitalWrite(PIN_AC, LOW);          // Отключаем общее питания 220V в СМ  

  MOTOR_direction = false;
  //Off_Display();
  
  addLog("Останавливаем все прерывания");
  detachInterrupt(0); // переход через ноль
  detachInterrupt(1); // от таходатчика 
  detachInterrupt(4); // от пресостата
  
}

void Off_Display() {

  addLog("Погасим светодиоды");
  fl_Bio = 0;               // флаг Биостирки
  fl_PoloskanirPlus = 0;    // Флаг Подоскание+
  fl_Del_payten = 0;        // Флаг Удаления пятен (предварительная стирка)
  fl_Easy = 0;              // Флаг Легкая глажка
  fl_800 = 0;               // Флаг 800 оборотов
  fl_600 = 0;               // Флаг 600 оборотов
  fl_500 = 0;               // Флаг 500 оборотов
  fl_400 = 0;               // Флаг 400 оборотов
  fl_0 = 0;                 // Флаг Без отжима
  fl_Wait = 0;              // Флаг Отложенного отжима
  fl_StartStop = 0;         // Флаг индикатора start/pause
  fl_BlockB = 0;            // Флаг индикатора Блокировки
  fl_Dots = 0;              // Флаг разделителя
  fl_mode_child_lock = 0;   // Флаг отображения блокировки от детей

  addLog("Погасим индикацию энкодера");
  for (int i=0; i < 19; i++) {
    prog[i] = 0;
  }  

  addLog("Погасим индикатор");
  indicator[0] = 22;  // 
  indicator[1] = 22;  //
  indicator[2] = 22;  //
  
  setInfoDisplay();
}

// Окончание работы СМ
void EndMashine() {
  Off_Mashine();

  mode_child_lock = 0;
  delay2(1000);
  
  indicator[0] = 12;  // E
  indicator[1] = 19;  // n
  indicator[2] = 18;  // d
  fl_Dots = 0;
  setInfoDisplay();
  
  for (int i=0; i < 5; i++) {
    tone(PIN_BEEP, 3000, 1000);
    delay2(2000);
  } 
     
  addLog("Ждем 30 мин и отключаем всю индикацию");
  delay2(1800000);
  Off_Display();

  addLog("Останавливаем главный таймер");
  timer_stop_ISR(_timer_MAIN);
  
  addLog("Останавливаем таймер управления мощностью мотора");
  timer_stop_ISR(_timer_MOTOR);

  addLog("Останавливаем таймер дисплея");
  timer_stop_ISR(_timer_DISPLAY);

  addLog("Перезагрузка");
  Reset();
}

// Экстренная остановка СМ из за какой либо ошибки
void StopMashine() {
  water_valte_all_off ();             // Отключаем забор воды
  pump_off ();                        // Отключаем насос

  Off_Mashine();

  for (int i=0; i < 5; i++) {
    tone(PIN_BEEP, 3000, 2000);
    delay2(3000);
  } 
  
  addLog("Ждем 30 мин и отключаем всю индикацию");
  delay2(1800000);

  Off_Display();

  addLog("Останавливаем главный таймер");
  timer_stop_ISR(_timer_MAIN);
  
  addLog("Останавливаем таймер управления мощностью мотора");
  timer_stop_ISR(_timer_MOTOR);

  addLog("Останавливаем таймер дисплея");
  timer_stop_ISR(_timer_DISPLAY);

  addLog("Перезагрузка");
  Reset();

}

// Записать в indicator время, в часах и в минутах
// time_m - время в минутах
void setTime (int time_m) {
  if (time_m < 1) time_m = 1; // Расчетное время не может быть меньше 1 минуты, т.к. либо должно быть 1 минута, либо End на экране
  if (!PAUSE) {
    if(OLD_time_m > time_m) { // Время не может быльше уже показанного
      OLD_time_m = time_m;
    } else {
      time_m = OLD_time_m;
    }
  }
  indicator[0] = time_m / 60;
  int str_m = time_m % 60;
  if (str_m < 10) {
    indicator[1] = 0;
    indicator[2] = str_m;
  } else {
    String str_t_m = String(time_m % 60);
    indicator[1] = str_t_m.substring(0, 1).toInt();
    indicator[2] = str_t_m.substring(1, 2).toInt();
  }
  
  // Зпомним переменные, что бы време не отображалось старое, при блокировке.
  displayBuf[0] = indicator[0];
  displayBuf[1] = indicator[1];
  displayBuf[2] = indicator[2];
  indicator0 = displayBuf[0];
  indicator1 = displayBuf[1];
  indicator2 = displayBuf[2];

  fl_Dots = 1; // Включить разделитель часы:минуты
  setInfoDisplay();
}

// Установка флагов для индикации отжима
void setSpin () {
  if (mode_spin == 800) {
    fl_800 = 1;
    fl_600 = 0;
    fl_500 = 0;
    fl_400 = 0;
    fl_0 = 0;
    fl_Wait = 0;
  } else if (mode_spin == 600) {
    fl_800 = 0;
    fl_600 = 1;
    fl_500 = 0;
    fl_400 = 0;
    fl_0 = 0;
    fl_Wait = 0;
  } else if (mode_spin == 500) {
    fl_800 = 0;
    fl_600 = 0;
    fl_500 = 1;
    fl_400 = 0;
    fl_0 = 0;
    fl_Wait = 0;
  } else if (mode_spin == 400) {
    fl_800 = 0;
    fl_600 = 0;
    fl_500 = 0;
    fl_400 = 1;
    fl_0 = 0;
    fl_Wait = 0;
  } else if (mode_spin == 0) {
    fl_800 = 0;
    fl_600 = 0;
    fl_500 = 0;
    fl_400 = 0;
    fl_0 = 1;
    fl_Wait = 0;
  } else if (mode_spin == 1) {
    fl_800 = 0;
    fl_600 = 0;
    fl_500 = 0;
    fl_400 = 0;
    fl_0 = 0;
    fl_Wait = 1;
  }
}

/* returns change in encoder state (-1,0,1) */
int8_t read_encoder() {
  static uint8_t old_AB = 0;
  old_AB <<= 2;
  old_AB |= ((digitalRead(ENC_B)) ? (1 << 1) : 0) | ((digitalRead(ENC_A)) ? (1 << 0) : 0);
  return (enc_states[(old_AB & 0x0f)]);
}

// Чтение кнопок, работает на отпускание кнопки
// Процедура обрабатывающая отпускание кнопки, должна сбрасывать соотвествующий флаг
void getButton() {
  if (displayPins[curr_digit_number] == 23 && !mode_child_lock) {
    if (digitalRead(PIN_BUTTON_INPUT1) && WORK) {
          fl_BUTTON_START_PAUSE = true;
    } else {
      if (fl_BUTTON_START_PAUSE && WORK) {
        if (Last_Time_BUTTON_START_PAUSE == 0) Last_Time_BUTTON_START_PAUSE = millis();
        if (Last_Time_BUTTON_START_PAUSE + BUTTON_TIMER_WAIT < millis()) {
          BUTTON_START_PAUSE = true;
          fl_BUTTON_START_PAUSE = false;
          Last_Time_BUTTON_START_PAUSE = 0;
        }
      }
    }
    if (PAUSE) {
      if (digitalRead(PIN_BUTTON_INPUT2)) {
        fl_BUTTON_ON = true;
      } else {
        if (fl_BUTTON_ON) {
          if (Last_Time_BUTTON_ON == 0) Last_Time_BUTTON_ON = millis();
          if (Last_Time_BUTTON_ON + BUTTON_TIMER_WAIT < millis()) {
            BUTTON_ON = true;
            fl_BUTTON_ON = false;
            Last_Time_BUTTON_ON = 0;
          }
        }
      }
    }
  } else if (displayPins[curr_digit_number] == 25 && !fl_BUTTON_CL && !mode_child_lock && PAUSE && WORK) {
    if (digitalRead(PIN_BUTTON_INPUT1)) {
      fl_BUTTON_PREWASHING = true;
    } else {
      if (fl_BUTTON_PREWASHING) {
        if (Last_Time_BUTTON_PREWASHING == 0) Last_Time_BUTTON_PREWASHING = millis();
        if (Last_Time_BUTTON_PREWASHING + BUTTON_TIMER_WAIT < millis()) {
          BUTTON_PREWASHING = true;
          fl_BUTTON_PREWASHING = false;
          Last_Time_BUTTON_PREWASHING = 0;
        }
      }
    }
    if (digitalRead(PIN_BUTTON_INPUT2)) {
      fl_BUTTON_TIMER = true;
    } else {
      if (fl_BUTTON_TIMER) {
        if (Last_Time_BUTTON_TIMER == 0) Last_Time_BUTTON_TIMER = millis();
        if (Last_Time_BUTTON_TIMER + BUTTON_TIMER_WAIT < millis()) {
          BUTTON_TIMER = true;
          fl_BUTTON_TIMER = false;
          Last_Time_BUTTON_TIMER = 0;
        }
      }
    }
  } else  if (displayPins[curr_digit_number] == 27 && !mode_child_lock && PAUSE && WORK) {
    if (digitalRead(PIN_BUTTON_INPUT1)) {
      fl_BUTTON_SPIN = true;
    } else {
      if (fl_BUTTON_SPIN) {
        if (Last_Time_BUTTON_SPIN == 0) Last_Time_BUTTON_SPIN = millis();
        if (Last_Time_BUTTON_SPIN + BUTTON_TIMER_WAIT < millis()) {
          BUTTON_SPIN = true;
          fl_BUTTON_SPIN = false;
          Last_Time_BUTTON_SPIN = 0;
        }
      }
    }
    if (digitalRead(PIN_BUTTON_INPUT2) && !fl_BUTTON_CL) {
      fl_BUTTON_BIOWASHING = true;
    } else {
      if (fl_BUTTON_BIOWASHING) {
        if (Last_Time_BUTTON_BIOWASHING == 0) Last_Time_BUTTON_BIOWASHING = millis();
        if (Last_Time_BUTTON_BIOWASHING + BUTTON_TIMER_WAIT < millis()) {
          BUTTON_BIOWASHING = true;
          fl_BUTTON_BIOWASHING = false;
          Last_Time_BUTTON_BIOWASHING = 0;
        }
      }
    }
  }
  
  // Блокировка от детей, фиксирует событие нажатие двух кнопок
  if (displayPins[curr_digit_number] == 25 && WORK) {
    if (digitalRead(PIN_BUTTON_INPUT1)) {
      if (Last_Time_BUTTON_PREWASHING2 == 0) Last_Time_BUTTON_PREWASHING2 = millis();
      if (Last_Time_BUTTON_PREWASHING2 + BUTTON_TIMER_WAIT < millis()) {
        BUTTON_PREWASHING2 = true;
        Last_Time_BUTTON_PREWASHING2 = 0;
      }
    } else {
      BUTTON_PREWASHING2 = false;
    }
  } else if (displayPins[curr_digit_number] == 27 && WORK) {
    if (digitalRead(PIN_BUTTON_INPUT2)) {
        if (Last_Time_BUTTON_BIOWASHING2 == 0) Last_Time_BUTTON_BIOWASHING2 = millis();
        if (Last_Time_BUTTON_BIOWASHING2 + BUTTON_TIMER_WAIT < millis()) {
          BUTTON_BIOWASHING2 = true;
          Last_Time_BUTTON_BIOWASHING2 = 0;
        }
      } else {
        BUTTON_BIOWASHING2 = false;
      }
  }
  if (BUTTON_PREWASHING2 && BUTTON_BIOWASHING2 && !fl_BUTTON_CL) {
    if (Last_Time_BUTTON_CL == 0) Last_Time_BUTTON_CL = millis();
    if (Last_Time_BUTTON_CL + BUTTON_TIMER_WAIT_CL < millis()) {
      BUTTON_CL = true;
      fl_BUTTON_CL = true;
      Last_Time_BUTTON_CL = 0;

      BUTTON_PREWASHING = false;
      fl_BUTTON_PREWASHING = false;
      Last_Time_BUTTON_PREWASHING = 0;

      BUTTON_BIOWASHING = false;
      fl_BUTTON_BIOWASHING = false;
      Last_Time_BUTTON_BIOWASHING = 0;
    }
  }
  
  // Блокировка от детей, фиксирует событие отпускание любой из двух кнопок
  if (displayPins[curr_digit_number] == 25) {
    if (!digitalRead(PIN_BUTTON_INPUT1) && fl_BUTTON_CL) {
      if (Last_Time_BUTTON_PREWASHING == 0) Last_Time_BUTTON_PREWASHING = millis();
      if (Last_Time_BUTTON_PREWASHING + BUTTON_TIMER_WAIT < millis()) {
        BUTTON_PREWASHING = false;
        fl_BUTTON_PREWASHING = false;
        Last_Time_BUTTON_PREWASHING = 0;
        Last_Time_BUTTON_PREWASHING2 = 0;
        fl_BUTTON_CL = false;
      }
    }
  } else if (displayPins[curr_digit_number] == 27) {
    if (!digitalRead(PIN_BUTTON_INPUT2) && fl_BUTTON_CL) {
        if (Last_Time_BUTTON_BIOWASHING == 0) Last_Time_BUTTON_BIOWASHING = millis();
        if (Last_Time_BUTTON_BIOWASHING + BUTTON_TIMER_WAIT < millis()) {
          BUTTON_BIOWASHING = false;
          fl_BUTTON_BIOWASHING = false;
          Last_Time_BUTTON_BIOWASHING = 0;
          Last_Time_BUTTON_BIOWASHING2 = 0;
          fl_BUTTON_CL = false;
        }
      }
  }
}

void getEncoder() {
  // Чтение энкодера
  if (PAUSE && !mode_child_lock && WORK) {
    encoderdata = read_encoder();
    if (encoderdata) {
      if (encoder_value == 0 && encoderdata == -1) {
        encoder_value = 18 * 4 + 3;
      } else if (encoder_value >= 18 * 4 + 3 && encoderdata == 1) {
        encoder_value = 0;
      } else {
        encoder_value += encoderdata;
      }
      if (DEBUG_LEVEL > 2) {
        addLog("Encoder: " + String(encoder_value) + " " + String(encoderdata) + " " + String(encoder_value >> 2));
      }
      setProrgam(encoder_value >> 2);
      // Задаем максимальные обороты отжима, в зависимости от программы
      if (current_program >= 14 && current_program <= 16) {
        mode_spin = 600;
      } else {
        mode_spin = 800;
      }
      if (current_program < 6 || current_program > 13) {
        fl_Del_payten = 0;
      }
      if (current_program != 6 && current_program != 11 && current_program != 12 && current_program != 13) {
        fl_Bio = 0; 
      }
      setSpin();
      getTotalTime(0, 0);
      setInfoDisplay();
    }
  }
}


void Event_Buttom_Spin () {
  if (mode_spin == 800) {
    if (current_program == 18) {
      mode_spin = 400;
    } else {
      mode_spin = 1;
    }
  } else if (mode_spin == 1) {
    mode_spin = 0;
  } else if (mode_spin == 0) {
    mode_spin = 400;
  } else if (mode_spin == 400) {
    mode_spin = 500;
  } else if (mode_spin == 500) {
    mode_spin = 600;
  } else if (mode_spin == 600) {
    // Ограничения скорости 600 оборотами, для деликатной стирки
    if (current_program >= 14 && current_program <= 16) {
      mode_spin = 1;
    } else {
      mode_spin = 800;
    }
  }
  setSpin();
  setInfoDisplay();
}

void Event_Buttom_Bio () {
  // Режим БИО работает только на синтетике и хлопке, при температурах 60, 95 градусов
  if (current_program == 6 || current_program == 11 || current_program == 12 || current_program == 13) {
    if (fl_PoloskanirPlus == 1 && fl_Bio == 0) {
      fl_Bio = 1;  
    } else if (fl_PoloskanirPlus == 1 && fl_Bio == 1) {
      fl_PoloskanirPlus = 0;  
    } else if (fl_PoloskanirPlus == 0 && fl_Bio == 1) {
      fl_Bio = 0;  
    } else if (fl_PoloskanirPlus == 0 && fl_Bio == 0) {
      fl_PoloskanirPlus = 1;  
    }
  } else {
    fl_Bio = 0;  
    fl_PoloskanirPlus = !fl_PoloskanirPlus;
  }
  setInfoDisplay();
}

void Event_Buttom_PREWASHING () {
  // Предварительная стирка работает только на синтетике и хлопке 
  if (current_program >= 6 && current_program <= 13) {
    if (fl_Easy == 1 && fl_Del_payten == 0) {
      fl_Del_payten = 1;  
    } else if (fl_Easy == 1 && fl_Del_payten == 1) {
      fl_Easy = 0;  
    } else if (fl_Easy == 0 && fl_Del_payten == 1) {
      fl_Del_payten = 0;  
    } else if (fl_Easy == 0 && fl_Del_payten == 0) {
      fl_Easy = 1;  
    }
  } else {
    fl_Del_payten = 0;  
    fl_Easy = !fl_Easy;
  }
  setInfoDisplay();
}
void Event_Buttom_BUTTON_START_PAUSE() {
  if (current_water_level == 0) error_pe(); // Проблема с пресостатом
  
}
