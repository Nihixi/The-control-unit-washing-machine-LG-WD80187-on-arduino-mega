void WaterCooling() {
  water_pour(1, WATER_LEVEL3);
  for (int i = 0; i < 5; i = i + 1) {
    MOTOR_direction = !MOTOR_direction;
    Motor(MOTOR_direction, MOTOR_SPEED_STRAIGHTEN_MIN1, MOTOR_POWER_Time_STRAIGHTEN);
    delay2(PROGRAM_PAUSE);
  }
}

void Del_payten() {
  if (fl_Del_payten == 1) { // Удаление пятен
    getTotalTime(1, PROGRAM_TIME_PREWASHING + getTimeWaterOff() + getTimeWaterOn() + 15);
    water_pour(1, WATER_LEVEL0);
    getTotalTime(1, PROGRAM_TIME_PREWASHING + getTimeWaterOff() + 15);
    water_heating(temp_PRE, 15);
    getTotalTime(1, PROGRAM_TIME_PREWASHING + getTimeWaterOff());
    delay2(PROGRAM_PAUSE);
    washing(PROGRAM_TIME_PREWASHING * 60, temp_PRE, washing_speed1, washing_pause5);
    getTotalTime(1, getTimeWaterOff());
    delay2(PROGRAM_PAUSE);
    drain_water();
    getTotalTime(1, 0);
  }
}

void Bio() {
  if (fl_Bio == 1) { // БИО РЕЖИМ
    water_heating(temp_BIO, PROGRAM_TIME_BIO);
    delay2(PROGRAM_PAUSE);
    washing(PROGRAM_TIME_BIO * 60, temp_BIO, washing_speed1, washing_pause1);
  }
}

void RunProgram() {
  digitalWrite(PIN_AC, HIGH);         // Включим 220V
  digitalWrite(PIN_BLOCK_DOOR, HIGH); // Заблокируем дверь
  
  delay2(5000); // ждем 5 сек, когда заблокируется дверь, пойдут нормальные данные от пресостата

  // Проверки
  //if (analogRead(PIN_CHECK_DOOR) != 0) {
    //int a0 = indicator[0];
    //int a1 = indicator[1];
    //int a2 = indicator[2];
    //error_de();
    // Ждем когда закроют дверцу
    //while (analogRead(PIN_CHECK_DOOR) != 0) {
    //  addLog(String(analogRead(PIN_CHECK_DOOR)));
    //  delay2(100);
    //}
    //indicator[0] = a0;  
    //indicator[1] = a1;  
    //indicator[2] = a2;  
    //setInfoDisplay();
  //}

  if (current_water_level == 0) {
    error_pe();
  }
  
  if (current_program != 0 && current_program != 18) {
    //Motor_Calibration();
  }

  
  switch (current_program) {
    case 0: // слив
      pump_on(60);
      delay2(70000);
      break;

    case 1: // Ручная стирка 30
      getTotalTime(1, 0);
      water_pour(2, WATER_LEVEL1);
      getTotalTime(2, 0);
      water_heating(30, 20);
      delay2(PROGRAM_PAUSE);
      getTotalTime(3, 0);
      washing(TIME_Washing_1 * 60, 30, washing_speed1, washing_pause2);
      delay2(PROGRAM_PAUSE);
      getTotalTime(4, 0);
      drain_water();
      delay2(PROGRAM_PAUSE);
      spin(1); 
      rinsing_spin();
      delay2(PROGRAM_PAUSE);
      break;

    case 2: // Ручная стирка ХВ
      getTotalTime(1, 0);
      water_pour(2, WATER_LEVEL1);
      getTotalTime(3, 0);
      washing(TIME_Washing_1 * 60, 0, washing_speed1, washing_pause2);
      delay2(PROGRAM_PAUSE);
      getTotalTime(4, 0);
      drain_water ();
      delay2(PROGRAM_PAUSE);
      spin(1); 
      rinsing_spin();
      delay2(PROGRAM_PAUSE);
      break;

    case 3: // Шерсть 40
      getTotalTime(1, 0);
      water_pour(2, WATER_LEVEL1);
      getTotalTime(2, 0);
      water_heating(40, 27);
      getTotalTime(3, 0);
      washing(TIME_Washing_2 * 60, 40, washing_speed0, washing_pause3);
      delay2(PROGRAM_PAUSE);
      getTotalTime(4, 0);
      drain_water();
      delay2(PROGRAM_PAUSE);
      spin(1); 
      rinsing_spin();
      delay2(PROGRAM_PAUSE);
      break;

    case 4: // Шерсть 30
      getTotalTime(1, 0);
      water_pour(2, WATER_LEVEL1);
      getTotalTime(2, 0);
      water_heating(30, 20);
      getTotalTime(3, 0);
      washing(TIME_Washing_2 * 60, 30, washing_speed0, washing_pause3);
      delay2(PROGRAM_PAUSE);
      getTotalTime(4, 0);
      drain_water();
      delay2(PROGRAM_PAUSE);
      spin(1); 
      rinsing_spin();
      delay2(PROGRAM_PAUSE);
      break;

    case 5: // Шерсть ХВ
      getTotalTime(1, 0);
      water_pour(2, WATER_LEVEL1);
      getTotalTime(3, 0);
      washing(TIME_Washing_2 * 60, 0, washing_speed0, washing_pause3);
      delay2(PROGRAM_PAUSE);
      getTotalTime(4, 0);
      drain_water();
      delay2(PROGRAM_PAUSE);
      spin(1); 
      rinsing_spin();
      delay2(PROGRAM_PAUSE);
      break;

    case 6: // Синтетика 60
      Del_payten();
      getTotalTime(1, 0);
      water_pour(2, WATER_LEVEL1);
      Bio();
      getTotalTime(2, 0);
      water_heating(60, 34);
      delay2(PROGRAM_PAUSE);
      getTotalTime(3, 0);
      washing(TIME_Washing_3 * 60, 60, washing_speed1, washing_pause1);
      delay2(PROGRAM_PAUSE);
      getTotalTime(4, 0);
      drain_water();
      delay2(PROGRAM_PAUSE);
      spin(1); 
      rinsing_spin();
      delay2(PROGRAM_PAUSE);
      break;

    case 7: // Синтетика 40
      Del_payten();
      getTotalTime(1, 0);
      water_pour(2, WATER_LEVEL1);
      getTotalTime(2, 0);
      water_heating(40, 27);
      delay2(PROGRAM_PAUSE);
      getTotalTime(3, 0);
      washing(TIME_Washing_3 * 60, 40, washing_speed1, washing_pause1);
      delay2(PROGRAM_PAUSE);
      getTotalTime(4, 0);
      drain_water();
      delay2(PROGRAM_PAUSE);
      spin(1); 
      rinsing_spin();
      delay2(PROGRAM_PAUSE);
      break;

    case 8: // Синтетика 30
      Del_payten();
      getTotalTime(1, 0);
      water_pour(2, WATER_LEVEL1);
      getTotalTime(2, 0);
      water_heating(30, 20);
      delay2(PROGRAM_PAUSE);
      getTotalTime(3, 0);
      washing(TIME_Washing_3 * 60, 30, washing_speed1, washing_pause1);
      delay2(PROGRAM_PAUSE);
      getTotalTime(4, 0);
      drain_water();
      delay2(PROGRAM_PAUSE);
      spin(1); 
      rinsing_spin();
      delay2(PROGRAM_PAUSE);
      break;

    case 9: // Хлопок 30
      Del_payten();
      getTotalTime(1, 0);
      water_pour(2, WATER_LEVEL1);
      getTotalTime(2, 0);
      water_heating(30, 20);
      delay2(PROGRAM_PAUSE);
      getTotalTime(3, 0);
      washing(TIME_Washing_4 * 60, 30, washing_speed2, washing_pause1);
      delay2(PROGRAM_PAUSE);
      getTotalTime(4, 0);
      drain_water();
      delay2(PROGRAM_PAUSE);
      spin(1); 
      rinsing_spin();
      delay2(PROGRAM_PAUSE);
      break;

    case 10: // Хлопок 40
      Del_payten();
      getTotalTime(1, 0);
      water_pour(2, WATER_LEVEL1);
      getTotalTime(2, 0);
      water_heating(40, 27);
      delay2(PROGRAM_PAUSE);
      getTotalTime(3, 0);
      washing(TIME_Washing_4 * 60, 40, washing_speed2, washing_pause1);
      delay2(PROGRAM_PAUSE);
      getTotalTime(4, 0);
      drain_water();
      delay2(PROGRAM_PAUSE);
      spin(1); 
      rinsing_spin();
      delay2(PROGRAM_PAUSE);
      break;

    case 11: // Хлопок 60
      Del_payten();
      getTotalTime(1, 0);
      water_pour(2, WATER_LEVEL1);
      getTotalTime(2, 0);
      Bio();
      water_heating(60, 34);
      delay2(PROGRAM_PAUSE);
      getTotalTime(3, 0);
      washing(TIME_Washing_4 * 60, 60, washing_speed2, washing_pause1);
      delay2(PROGRAM_PAUSE);
      getTotalTime(4, 0);
      drain_water();
      delay2(PROGRAM_PAUSE);
      spin(1); 
      rinsing_spin();
      delay2(PROGRAM_PAUSE);
      break;

    case 12: // Хлопок 95
      Del_payten();
      getTotalTime(1, 0);
      water_pour(2, WATER_LEVEL1);
      Bio();
      getTotalTime(2, 0);
      water_heating(90, 50);
      delay2(PROGRAM_PAUSE);
      getTotalTime(3, 0);
      washing(TIME_Washing_4 * 60, 90, washing_speed2, washing_pause1);
      delay2(PROGRAM_PAUSE);
      getTotalTime(4, 0);
      WaterCooling();
      drain_water();
      delay2(PROGRAM_PAUSE);
      spin(1); 
      rinsing_spin();
      delay2(PROGRAM_PAUSE);
      break;
       
    case 13: // Хлопок 60 ЭКОНОМ
      Del_payten();
      getTotalTime(1, 0);
      water_pour(2, WATER_LEVEL1);
      getTotalTime(2, 0);
      Bio();
      water_heating(50, 35);
      delay2(PROGRAM_PAUSE);
      getTotalTime(3, 0);
      washing(TIME_Washing_5 * 60, 50, washing_speed2, washing_pause1);
      delay2(PROGRAM_PAUSE);
      getTotalTime(4, 0);
      drain_water();
      delay2(PROGRAM_PAUSE);
      spin(1); 
      rinsing_spin();
      delay2(PROGRAM_PAUSE);
      break;

    case 14: // Деликатная ХВ
      getTotalTime(1, 0);
      water_pour(2, WATER_LEVEL2);
      delay2(PROGRAM_PAUSE);
      getTotalTime(3, 0);
      washing(TIME_Washing_6 * 60, 0, washing_speed0, washing_pause3);
      delay2(PROGRAM_PAUSE);
      getTotalTime(4, 0);
      drain_water();
      delay2(PROGRAM_PAUSE);
      spin(1); 
      rinsing_spin();
      delay2(PROGRAM_PAUSE);
      break;

    case 15: // Деликатная 30
      getTotalTime(1, 0);
      water_pour(2, WATER_LEVEL2);
      getTotalTime(2, 0);
      water_heating(30, 20);
      delay2(PROGRAM_PAUSE);
      getTotalTime(3, 0);
      washing(TIME_Washing_6 * 60, 30, washing_speed0, washing_pause3);
      delay2(PROGRAM_PAUSE);
      getTotalTime(4, 0);
      drain_water();
      delay2(PROGRAM_PAUSE);
      spin(1); 
      rinsing_spin();
      delay2(PROGRAM_PAUSE);
      break;

    case 16: // Деликатная 40
      getTotalTime(1, 0);
      water_pour(2, WATER_LEVEL2);
      getTotalTime(2, 0);
      water_heating(40, 27);
      delay2(PROGRAM_PAUSE);
      getTotalTime(3, 0);
      washing(TIME_Washing_6 * 60, 40, washing_speed0, washing_pause3);
      delay2(PROGRAM_PAUSE);
      getTotalTime(4, 0);
      drain_water();
      delay2(PROGRAM_PAUSE);
      spin(1); 
      rinsing_spin();
      delay2(PROGRAM_PAUSE);
      break;

    case 17: // Быстро 30
      TEN_STOP_temp = 30;
      getTotalTime(1, 0);
      water_pour(2, WATER_LEVEL1);
      ten_on(TIME_Washing_7 * 1000);
      delay2(PROGRAM_PAUSE);
      getTotalTime(3, 0);
      washing(TIME_Washing_7 * 60, 0, washing_speed2, washing_pause0);
      ten_off();
      getTotalTime(4, 0);
      delay2(PROGRAM_PAUSE);
      drain_water();
      delay2(PROGRAM_PAUSE);
      spin(1); 
      rinsing_spin();
      delay2(PROGRAM_PAUSE);
      break;

    case 18: // Полоскание + Отжим
      getTotalTime(4, 0);
      drain_water();
      delay2(PROGRAM_PAUSE);
      rinsing_spin();
      delay2(PROGRAM_PAUSE);
      break;

  }

  digitalWrite(PIN_BLOCK_DOOR, LOW); // Разблокируем дверь, сработает замок, как только остынет элемент, поэтому делаем это заранее

  setTime(1);
  pump_on(35);
  delay2(40000);
  digitalWrite(PIN_AC, LOW);         // Включим 220V в СМ
  delay2(10000);
  
  if (!PAUSE) {
    EndMashine();
  }
}
