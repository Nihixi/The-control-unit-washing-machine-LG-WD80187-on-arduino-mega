//******************** Стирка *******************************

void washing(unsigned long washing_time, int temp_MAIN, int washing_speed, int washing_pause) {
  unsigned long time_Start_washing = millis();
  unsigned long time_Stop_washing = time_Start_washing + washing_time * 1000;
  unsigned long sumTimeM = 0;

  int temp_MAIN_min = temp_MAIN - (temp_MAIN * washing_temp_delta / 100);

  fl_Vibro_Control = false; // Отключим контроль вибрации

  addLog("Подпрограмма СТИРКИ на " + String((time_Stop_washing - time_Start_washing) / 1000) + " сек, при температуре: " + String(temp_MAIN));

  addLog("Start СТИРКИ");
  while (millis() < time_Stop_washing) {

    if (HeatingWaterWashing) {
      // Проверим температуру воды и подогреем, если это необходимо
      get_real_Temperatura();
      if (TEN_current_temp < temp_MAIN_min) {
        if (DEBUG_LEVEL >= 2) {
          addLog("TEN_current_temp: " + String(TEN_current_temp) + " temp_MAIN_min: " + String(temp_MAIN_min));
          addLog("Включаем подогрев до температуры : " + String(temp_BIO) + " С");
        }
        MOTOR_POWER_Time_Ten = washing_pause;
        water_heating(temp_MAIN);
        delay2(washing_pause);
      }
    }

    MOTOR_direction = !MOTOR_direction;
    Motor(MOTOR_direction, washing_speed, MOTOR_POWER_Time_washing);
    delay2(washing_pause);
    sumTimeM = floor((millis() - time_Start_washing) / 1000 / 60); // Прошло времени в минутах
    setTime(TimeLeft - sumTimeM);
  }
  addLog("End СТИРКИ");
}
