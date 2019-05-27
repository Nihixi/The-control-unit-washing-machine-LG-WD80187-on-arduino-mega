//********************обработчики прерываний*******************************

// ***************************
// Прерывания таймеров
// ***************************

void timer_handle_interrupts(int timer) {

  // Таймер регулятора оборотов мотора
  if (timer == _timer_MOTOR && WORK == true && PAUSE == false) {

    // Защита, проверим таймаут работы мотора
    if (MOTOR_POWER_Time_START != 0 && pwmSpeed > 0 && MOTOR_ON) {
      if (millis() > MOTOR_POWER_Time_START + MOTOR_POWER_Time_WORK) {
        //setPower(0); // Если время вышло, выключим мотор
        //error_int(); // Покажем ошибку Int
        //PAUSE = true;
        //WORK = false;
      }
    }

    if (count_timer_motor == 1) { // запуск таймера каждые 40 микросекунд, а не каждые 20 микросекунд (timer_init_ISR_50KHz), таймера на 40 микросекунд нет
      tic++;  //счетчик
      if (Dimmer < tic ) digitalWrite(PIN_MOTOR_OUT, HIGH); //управляем выходом
      count_timer_motor = 0;
    } else {
      count_timer_motor++;
    }
  }

  // Главный таймер
  if (timer == _timer_MAIN) {
    unsigned long currentMillis = millis();

    // Получение информации от таходатчика
    pwmSpeed = count_tacho;
    count_tacho = 0;

    // Если мотор включен
    if (MOTOR_ON) {
      ControlSpeed(); 
    }

    // Если нужно, то контролируем вибрацию
    check_vibro();

    // Проверка на критический уровень воды в баке
    if (current_water_level < WATER_LEVEL4 && current_water_level != 0) {
     // water_valte_all_off (); // Выключим все клапана набора воды
     // error_fe ();  // Отобразим на экране ошибку и остановим все
    }

    // Проверка на отключение насоса
    if (flag_pump == true) { // Насос включен
      if (currentMillis > time_stop_pump) {
        pump_off ();
      }
    }
    
    // Проверка на отключение клапана 1 залива воды
    if (flag_water_valte1 == true) { // клапан 1 включен
      if (currentMillis > time_stop_water_valte1) {
        debug_water_valte1 = false;
        water_valte1_off ();
      }
    }
    
    // Проверка на отключение клапана 2 залива воды
    if (flag_water_valte2 == true) { // клапан 2 включен
      if (currentMillis > time_stop_water_valte2) {
        debug_water_valte2 = false;
        water_valte2_off ();
      }
    }

    // Проверка на отключение ТЭНа
    if (flag_ten == true) { // ТЭН включен
      if (currentMillis > time_stop_ten) {
        ten_off ();
      }
      if (TEN_current_temp > TEN_STOP_temp) {
        ten_off ();
      }
    }
    
    count_1000ms++;
    if (count_1000ms > 10) {
      // Вывод 1 раз в сек. CL на дисплей, если вкл. блокировка от детей
      // используется прямая запись в displayBuf, что бы сократить время и не использовать setInfoDisplay()
      if (WORK == true && mode_child_lock == 1) {
        fl_mode_child_lock = !fl_mode_child_lock;
        if (fl_mode_child_lock == 1) {
          indicator0 = displayBuf[0];
          indicator1 = displayBuf[1];
          indicator2 = displayBuf[2];
          displayBuf[0] = 22;
          displayBuf[1] = 11;
          displayBuf[2] = 15;
          bitWrite(displayBuf[3], 5, 0);
        } else {
          displayBuf[0] = indicator0;
          displayBuf[1] = indicator1;
          displayBuf[2] = indicator2;
          bitWrite(displayBuf[3], 5, 1);
        }
      }

      // Получим в current_water_level уровень воды, как частоту в Гц
      current_water_level = count_water_gz;
      count_water_gz = 0;

      // Получим количество импульсов с ТАХО за сек
      pwmSpeedSec = pwmSpeedSecTmp;
      pwmSpeedSecTmp = 0;
      
      // обнулим счетчик 1/10 сек
      count_1000ms = 0;
    } else {
      pwmSpeedSecTmp = pwmSpeedSecTmp + pwmSpeed;
    }
  }

  if (timer == _timer_DISPLAY) {
    viewDisplay();  // Отображение текущего состояния (один разряд, за одно срабатывание таймера)
  }
}




// ***************************
// Аппаратные прерывания
// ***************************

// Сработает по переднему фронту
void  detect_up() {
  tic = 0;           //обнулить счетчик
  timer_init_ISR_50KHz(_timer_MOTOR);   //запустить таймер
  attachInterrupt(0, detect_down, HIGH);  //перепрограммировать прерывание на другой обработчик
}

// Сработает по заднему фронту
void  detect_down()  {
  timer_stop_ISR(_timer_MOTOR); //остановить таймер
  digitalWrite(PIN_MOTOR_OUT, LOW); //логический ноль на выходы
  tic = 0;     //обнулить счетчик
  attachInterrupt(0, detect_up, LOW); //перепрограммировать прерывание на другой обработчик
}

// От таходатчика
void detect_tacho() {
  count_tacho++;
}

void detect_water () {
  count_water_gz++;
}
//*************************************************************************
