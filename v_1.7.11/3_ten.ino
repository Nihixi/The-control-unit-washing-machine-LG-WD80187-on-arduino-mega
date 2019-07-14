//******************** работа с ТЭНом *******************************

// расчёт температуры
int get_real_Temperatura() {
  // формула температуры t=k*(PIN_TEMP-a)^n+b  где  k=0.0001, a=1150, b=-5, n = 2 (парабола)
  long summ = 0;            // сумма для расчёта среднего
  const int i_Max = 1000;   // количество раз замера температуры
   
  for (int i = 1; i <= i_Max; i++) {              // Вычисление среднего значения температуры
    long aaa = ((analogRead(PIN_TEMP) - 1150));   // вычисление (PIN_TEMP-a)
    aaa = aaa * aaa;                              // возведение в квадрат
    aaa = aaa / 10000;                            // *0.0001
    aaa = aaa - 5;                                // -5
    if (aaa < 0) aaa = 0;
    summ = summ + aaa;                            // сумма i_Max температур
  }
  int ttt = summ / i_Max;                         // расчёт средней температуры
  ttt = ttt + 5;
  if (ttt > 99) ttt = 99;
  TEN_current_temp = ttt;
}

void ten_on (unsigned long timer) {
  if (current_water_level < WATER_LEVEL7) {
    // Включаем ТЭН, только если достаточно воды в баке
    digitalWrite(PIN_TEN, HIGH);
    time_stop_ten = millis() + timer * 1000;
    if (DEBUG_LEVEL >= 1) {
      addLog("Включаем ТЭН на " + String((time_stop_ten - millis()) / 1000) + " сек.");
    }
    flag_ten = true;
  } else {
    if (DEBUG_LEVEL >= 1) {
      addLog("Не можем включить ТЭН, т.к. в баке НЕ ДОСТАТОЧНО ВОДЫ!");
    }
  }
}

void ten_off () {
  digitalWrite(PIN_TEN, LOW);
  addLog("Выключаем ТЭН");
  flag_ten = false;
}

// Подогрев воды до температуры temp
// temp - необходимая темепература
// aboutTime - расчетное время, фактическое время нагрева маштабируется до aboutTime и не может быть его больше
void water_heating (int temp, int aboutTime) {
  addLog("Подпрограмма НАГРЕВ воды до температуры " + String(temp) + "С");
  ten_on(time_wait_heating / 1000); // Включим ТЭН на time_wait_heating

  unsigned long sumTimeM = 0;
  unsigned long time_Start_heating = millis();
  unsigned long time_Start_motor = millis() + MOTOR_POWER_Time_Ten; // Крутить барабаном во время подогрева, каждые MOTOR_POWER_Time_Ten мсек
  Washing_temperature = temp;
  TEN_STOP_temp = temp + 5; // Защита, автоматически отключит ТЭН при превышении температуры более чем на 5 градусов, обработка в таймере _timer_MAIN
  
  get_real_Temperatura();

  int Start_TEN_current_temp = TEN_current_temp;
  int m_c = 0;
  int t_m_s = 0;
  int t_m_s_r = 0;

  TimeLeftOld = 0;

  addLog("get_real_Temperatura: " + String(TEN_current_temp) + " Washing_temperature: " + String (Washing_temperature));
  while (TEN_current_temp < Washing_temperature) {
      if ((millis() - time_Start_heating) > time_wait_heating && TEN_current_temp < Washing_temperature) { // НЕТ нагрева в течение time_wait_heating миллисекунд
        addLog("Нагрева НЕТ за " + String(time_wait_heating) + "мсек продолжаем стирку без нагрева");
        break; // Просто выйдем и продолжим стирку в холодной воде
      }
      if (millis() > time_Start_motor) { // Пора покрутить барабаном
        MOTOR_direction = !MOTOR_direction;
        Motor(MOTOR_direction, currSpeedM, MOTOR_POWER_Time_Ten_ON);
        time_Start_motor = millis() + MOTOR_POWER_Time_Ten; // Крутить барабаном во время подогрева, каждые MOTOR_POWER_Time_Ten мсек
      }
      
      delay2(500);
      get_real_Temperatura();
      
      // Прикинем сколько еще осталось греть воду по времени и приведем это время к расчетному aboutTime (оставшееся время будет маштабированно до aboutTime)
      if (aboutTime != 0) {
        sumTimeM = ((millis() - time_Start_heating) / 1000 / 60); // Прошло времени в минутах
        addLog("sumTimeM " + String(sumTimeM));
        if (sumTimeM > 0) {
          if (TEN_current_temp > Start_TEN_current_temp) {
            m_c = sumTimeM / (TEN_current_temp - Start_TEN_current_temp); // нужно минут на нагрев 1 градуса
            if (m_c == 0) m_c = 1;
            t_m_s = m_c * (temp - TEN_current_temp); // Фактически, сколько еще греть в минутах
            t_m_s_r = map (sumTimeM, 0, sumTimeM + t_m_s, 0, aboutTime); // Расчетное время, приведенное к aboutTime
            if (t_m_s_r > t_m_s) t_m_s_r = t_m_s;
          } else {
            t_m_s_r = 0; 
          }
        } else {
          t_m_s_r = 0; 
        }
        t_m_s_r = aboutTime - t_m_s_r;

        if (TimeLeftOld == 0) TimeLeftOld = TimeLeft - aboutTime + t_m_s_r;
        if (TimeLeft - aboutTime + t_m_s_r < TimeLeftOld) {
          setTime(TimeLeft - aboutTime + t_m_s_r);
          TimeLeftOld = TimeLeft - aboutTime + t_m_s_r;
        }
      }

      if (DEBUG_LEVEL == 3) {
        addLog("get_real_Temperatura: " + String(TEN_current_temp) + ", Washing_temperature: " + String (Washing_temperature) + ", sumTimeM: " + String(sumTimeM) + ". Минут на нагрев 1 градуса: " + String(m_c) + ", Сколько еще греть в минутах: " + String(t_m_s) + ", Расчетное время, приведенное к aboutTime: " + String(t_m_s_r) + " aboutTime: " + String(aboutTime));
        //addLog("millis: " + String(millis()) + " time_Start_motor: " + String (time_Start_motor));
      }
  }
  ten_off();
}
