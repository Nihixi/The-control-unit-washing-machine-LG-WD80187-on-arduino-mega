//---------------------------- Функция вращения барабана -----------------------

void turbo_on() {
  addLog("Включаем режим Turbo");
  digitalWrite(PIN_MOTOR_TURBO, HIGH);
}

void turbo_off() {
  addLog("Выключаем режим Turbo");
  digitalWrite(PIN_MOTOR_TURBO, LOW);
}


void SetMinimalPOWER() {
  MOTOR_POWER_MINIMAL = MOTOR_POWER_MINIMAL_ORIGINAL;
  addLog("SetMinimalPOWER: " + String(MOTOR_POWER_MINIMAL));
  int sizeArray_POWER = sizeof(MOTOR_POWER_Array) / sizeof(int);
  for (int i = 0; i < sizeArray_POWER; i = i + 1) {
    MOTOR_POWER_Array[i] = 0;
  }
}

// Функция для расчета средней мощности подаваемой на мотор, для поддержания заданной скорости вращения
// Изменяет гл. переменную MOTOR_POWER_MINIMAL, которая не должна в итоге, быть меньше MOTOR_POWER_MINIMAL_ORIGINAL и больше MOTOR_POWER_MINIMAL_ORIGINAL + 60%
void add_array_power(int pwmOut_s) {
  if (pwmOut_s > MOTOR_POWER_MINIMAL_ORIGINAL) {
    MOTOR_POWER_Array[MOTOR_POWER_Array_pointer] = pwmOut_s;
    MOTOR_POWER_Array_pointer++;
    int sizeArray_POWER = sizeof(MOTOR_POWER_Array) / sizeof(int);
    if (MOTOR_POWER_Array_pointer > sizeArray_POWER - 1) {
      MOTOR_POWER_Array_pointer = 0;
    }
    // Получим среднее значение, без учета элементов с 0
    unsigned long n_sum = 0;
    unsigned long sum = 0;
    int sizeArray_POWER_ = sizeof(MOTOR_POWER_Array) / sizeof(int);
    for (int i = 0; i < sizeArray_POWER_; i = i + 1) {
      if (MOTOR_POWER_Array[i] != 0) {
        sum = sum + MOTOR_POWER_Array[i];
        n_sum++;
      }
    }
    if (n_sum != 0) {
      MOTOR_POWER_MINIMAL = round(sum / n_sum);
    }
    if (MOTOR_POWER_MINIMAL == 0 || MOTOR_POWER_MINIMAL < MOTOR_POWER_MINIMAL_ORIGINAL) { // Минимальная начальная скрость не может быть меньше зданной или равно 0
      MOTOR_POWER_MINIMAL = MOTOR_POWER_MINIMAL_ORIGINAL;
    }
    if (MOTOR_POWER_MINIMAL > MOTOR_POWER_MINIMAL_ORIGINAL + MOTOR_POWER_MINIMAL_ORIGINAL * 80 / 100) { // Минимальная начальная скрость не может быть больше заданной + 80%
      MOTOR_POWER_MINIMAL = MOTOR_POWER_MINIMAL_ORIGINAL + MOTOR_POWER_MINIMAL_ORIGINAL * 80 / 100;
    }
    if (DEBUG_LEVEL == 3) {
      addLog("set MOTOR_POWER_MINIMAL: " + String(MOTOR_POWER_MINIMAL));
    }
  }
}

void pwmSetMinMax() {
  pwmSetMin = pwmSet - 1;
  pwmSetMax = pwmSet + 1;
  if (DEBUG_LEVEL >= 2) {
    addLog("pwmSetMinMax: " + String(pwmSet) + " pwmSetMin:" + String(pwmSetMin) + " pwmSetMax:" + String(pwmSetMax));
  }
}

// Устанавливаем мощность подаваемую на мотор
// 0 - мотор выключен
// 255 - максимальные обороты
void setPower(int MOTOR_power) {
  if (MOTOR_power < 0 || pwmOut < 0) { // Защита, на всякий случай
    MOTOR_power = 0;
    pwmOut = 0;
  }
  pwmOut = MOTOR_power;
  Dimmer = 255 - MOTOR_power;
}

// Плавный старт мотора
void SoftStart(int MOTOR_power) {
  addLog("SoftStart: " + String(MOTOR_power));
  for (int i = 0; i <= MOTOR_power; i++) {
    setPower(i);
    delay2(5);
  }
}

// Первый запуск мотора на MOTOR_POWER_MINIMAL_ORIGINAL сек, для калибровки мощности
void Motor_Calibration() {
  if (MOTOR_POWER_MINIMAL == MOTOR_POWER_MINIMAL_ORIGINAL) {
    addLog("Motor_Calibration MOTOR_direction " + String(MOTOR_direction) + " washing_speed1: " + String(washing_speed1) + " MOTOR_POWER_Time_calibration: " + String(MOTOR_POWER_Time_calibration));
    SetMinimalPOWER();
    Motor(MOTOR_direction, washing_speed1, MOTOR_POWER_Time_calibration);
  }
}

void ControlSpeed() {
  //if (pwmSpeed == pwmSet) {
    fl_add = true;
  //}

  // Если мотор не вращается при pwmOut = 255, выводим ошибку
  if (pwmOut == 255 && pwmSpeed == 0) {
    //error_ce (); 
  }

  if (pwmSpeed < pwmSetMin) { // увеличиваем обороты
    if (pwmOut < 255) {
      if (pwmSpeed == 0) { // Если вал еще не крутиться
        pwmOut = pwmOut + 10;
      } else {
        if (minimalPower == 0) minimalPower = pwmOut;
        if (fl_add) {
          pwmOut++;
        }
      }
      setPower(pwmOut);
    }
  }
  if (pwmSpeed > pwmSetMax) { // уменьшаем обороты
    if (pwmSpeed >= pwmSet * 2) {
      if (pwmOut > minimalPower && minimalPower > 0) {
        pwmOut = minimalPower;
        setPower(pwmOut);
      } else {
        if (pwmOut > 0) {
          pwmOut--;
          if (pwmOut < MOTOR_POWER_MINIMAL_ORIGINAL) pwmOut = MOTOR_POWER_MINIMAL_ORIGINAL;
          setPower(pwmOut);
        }
      }
    } else {
      if (pwmOut > 0) {
        pwmOut--;
        if (pwmOut < MOTOR_POWER_MINIMAL_ORIGINAL) pwmOut = MOTOR_POWER_MINIMAL_ORIGINAL;
        setPower(pwmOut);
      }
    }
  }
}

void Motor(boolean MOTOR_direction, int MOTOR_speed, unsigned long MOTOR_time) {
  MotorStop();
  MotorWaitStop(); // Ждем остановки, т.к. возможно направление вращения барабана сменилось
  delay2(1000);
  
  digitalWrite(PIN_MOTOR_OUT, LOW); //Принудительно отключим мотор
  delay2(100); // Пауза, что бы успело сработать
  digitalWrite(PIN_MOTOR_LR, MOTOR_direction); // Установим направление вращения
  delay2(1000); // Пауза, что бы реле успело сработать

  unsigned long MOTOR_time_l = MOTOR_time * 1000;
  pwmSet = MOTOR_speed;
  pwmSetMinMax();

  MotorStart(); // Запускаем мотор
  
  unsigned long millis_m = millis();
  while ((millis() - millis_m) <= MOTOR_time_l)  {

    // Если контролируем вибрацию
    if (fl_Vibro_Control) {
      if (fl_Vibro) {
        break;
      }
    }

    int obm = pwmSpeed * MOTOR_SPEED_rate;
    if (DEBUG_LEVEL >= 3) {
      addLog("MOTOR_direction: " + String(MOTOR_direction) + " pwmSet: " + String(pwmSet) + " pwmSpeed:" + String(pwmSpeed) + " (" + String(pwmSpeedSec) + ") pwmOut:" + String(pwmOut) + " Dimmer:" + String(Dimmer));
    }

    if (pwmSpeed == pwmSet) {
      add_array_power(pwmOut); // Если скорость вращения барабана = pwmSet, добавим значение мощности в массив, для расчета средней мощности
    }
  }

  MotorStop(); // Выключаем мотор
  MotorWaitStop(); // Ожидаем остановку мотора
}

// Запуск мотора
void MotorStart() {
  fl_add = false;
  pwmOut = MOTOR_POWER_MINIMAL; // Устанавливаем начальную скорость
  SoftStart(pwmOut);            // Плавно запускаем мотор
  MOTOR_ON = true;              // Включаем регулировку мотора, по таймеру _timer_MOTOR
  MOTOR_POWER_Time_START = millis();
}

// Остановка мотора
void MotorStop() {
  MOTOR_ON = false;
  pwmOut = 0;
  setPower(pwmOut);
  delay2(1000);
  MOTOR_POWER_Time_START = 0;
}

// Ожидание остановки мотора
void MotorWaitStop() {
  unsigned long millis_m = millis();
  while ((millis() - millis_m) <= MOTOR_STOP_Time_Wait)  {
    if (pwmSpeed == 0) {
      break;
    }
  }
}

void check_vibro() {
  if (fl_Vibro_Control) {
    if (digitalRead(PIN_VIBRO)) {
      fl_Vibro = true;
      addLog("Датчик вибрации!");
    }
  }  
}
