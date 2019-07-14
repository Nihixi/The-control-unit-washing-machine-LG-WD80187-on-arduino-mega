//******************** работа с клапанами налива воды *******************************

// Открытие клапана стирки/полоскания
// Не обязательный параметр, время включения в сек, по умолчанию 30 сек
void water_valte1_on (unsigned long timer) {
  digitalWrite(PIN_WATER_VALVE1, HIGH);
  time_stop_water_valte1 = millis() + timer * 1000;
  flag_water_valte1 = true;
}

// Открытие клапана предварительной стирки
// Не обязательный параметр, время включения в сек, по умолчанию 30 сек
void water_valte2_on (unsigned long timer) {
  digitalWrite(PIN_WATER_VALVE2, HIGH);
  time_stop_water_valte2 = millis() + timer * 1000;
  flag_water_valte2 = true;
}

// Открытие 2-х клапанов, для подачи раствора на последнем этапе полоскания
// Не обязательный параметр, время включения в сек, по умолчанию 30 сек
void water_valte_all_on (unsigned long timer) {
  digitalWrite(PIN_WATER_VALVE1, HIGH);
  digitalWrite(PIN_WATER_VALVE2, HIGH);
  flag_water_valte1 = true;
  flag_water_valte2 = true;
  time_stop_water_valte1 = millis() + timer * 1000;
  time_stop_water_valte2 = millis() + timer * 1000;
}

void water_valte1_off () {
  digitalWrite(PIN_WATER_VALVE1, LOW);
  flag_water_valte1 = false;
}

void water_valte2_off () {
  digitalWrite(PIN_WATER_VALVE2, LOW);
  flag_water_valte2 = false;
}

void water_valte_all_off () {
  digitalWrite(PIN_WATER_VALVE1, LOW);
  digitalWrite(PIN_WATER_VALVE2, LOW);
  flag_water_valte1 = false;
  flag_water_valte2 = false;
}

// mode - режим набора воды:
// 1 - набор через клапан предварительной стирки
// 2 - набор воды через клапан основной стирки
// 3 - набор воды через два клапана, (полоскание с ополаскивателем)
void water_pour(int mode, int level) {
  boolean fl_water_on = false;

  unsigned long time_Stop_water = millis() + WATER_WAIT;
  int levelMax = level - WATER_LEVEL_delta;
  
  for (int i = 0; i < countRetryWater; i = i + 1) {
    while (millis() < time_Stop_water) {
      // Специально наливаем чуть больше, в расчете на то, что белье впитает воду, после вращения барабаном
      // и нам не придется наливать по капли
      if (current_water_level < levelMax) {
        fl_water_on = true; // Воду набрали
        break;
      } else {
        if (mode == 1 && flag_water_valte1 == false) {
          delay2(5000); // Пауза перед открытием клапана, т.к. клапан мог быть закрыт только что по таймауту из таймера.
          water_valte1_on(WATER_LEVEL_TIME);
        } else if (mode == 2 && flag_water_valte2 == false) {
          delay2(5000); // Пауза перед открытием клапана, т.к. клапан мог быть закрыт только что по таймауту из таймера.
          water_valte2_on(WATER_LEVEL_TIME);
        } else if (mode == 3 && flag_water_valte1 == false && flag_water_valte2 == false) {
          delay2(5000); // Пауза перед открытием клапана, т.к. клапан мог быть закрыт только что по таймауту из таймера.
          water_valte_all_on(WATER_LEVEL_TIME);
        }
      }
      addLog("0. Уровень воды: " + String(current_water_level) + " Hz, нужно: " + String(level) + " Hz");
      delay2(500);    
    }

    water_valte_all_off (); // Закрываем все клапаны
    addLog("Закрываем все клапаны");

    // Ошибка вода не льется или слабый напор воды, возможно засорился фильтр
    if (fl_water_on == false) {
      error_1e ();
    }
    
    // Покрутим мотором, что бы намочить белье
    addLog("1. Уровень воды: " + String(current_water_level) + " Hz, нужно: " + String(level) + " Hz");
    MOTOR_direction = !MOTOR_direction;
    Motor(MOTOR_direction, MOTOR_SPEED_Water_ON, MOTOR_POWER_Time_Water_ON);
    delay2(10000);
    addLog("2. Уровень воды: " + String(current_water_level) + " Hz, нужно: " + String(level) + " Hz");

    // Проверяем объем воды еще раз, если чуть, чуть не хватает, на WATER_LEVEL_delta, не дергаемся лишний раз
    if (current_water_level - WATER_LEVEL_delta < level) {
      break;
    } else {
      fl_water_on = false; // Воду еще не набрали
    }
  }

  // Ошибка вода не доливается, после смачивания
  if (fl_water_on == false) {
    error_1e ();
  }

}
