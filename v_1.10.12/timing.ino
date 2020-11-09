//********************Расчет времени*******************************

// Расчет оставшегося времени
// currentStep - текущий шаг
// timeAdd - добавляемое время
void  getTotalTime(int currentStep, int timeAdd) {
  int timing = 0;
  if (WORK) {
    switch (currentStep) {
    case 0: // Предварительный расчет
      timing = getTimeDel_payten()+ getTimeWaterOn () + getTimeBIO() + getTimeTenOn () + getTimeWashing () + getTimeRisingSpin () + getTimeEasy ();
      break;
    case 1: // Предварительная стирка
      timing = getTimeWaterOn () + getTimeBIO() + getTimeTenOn () + getTimeWashing () + getTimeRisingSpin () + getTimeEasy ();
      break;
    case 2: // Набор воды
      timing = getTimeBIO() + getTimeTenOn () + getTimeWashing () + getTimeRisingSpin () + getTimeEasy ();
      break;
    case 3: // Нагрев воды + биостирка
      timing = getTimeWashing () + getTimeRisingSpin () + getTimeEasy ();
      break;
    case 4: // Отжим
      timing = getTimeRisingSpin () + getTimeEasy ();
      break;
    case 5: // Легкая глажка
      timing = getTimeEasy ();
      break;
    }
    TimeLeft = timing + timeAdd;
    setTime(TimeLeft);
  }
}

// Био стирка
int getTimeBIO() {
  if (fl_Bio == 1) {
    return PROGRAM_TIME_BIO;
  } else {
    return 0;
  }
}

// Предварительная стирка, доп. учитывает подогрев для предварительной стирки, слив и налив воды для основной стирки
int getTimeDel_payten() {
  if (fl_Del_payten == 1) {
    return PROGRAM_TIME_PREWASHING + getTimeWaterOff () + getTimeWaterOn () + 15;
  } else {
    return 0;
  }
}

// Примерное время набора воды, в мин
int getTimeWaterOn () {
  if (current_program == 0) {
    return 0;
  } else {
    return 2;
  }
}

// Примерное время слива воды, в мин
int getTimeWaterOff () {
  if (current_program == 0) {
    return 0;
  } else {
    return 1;
  }
}

// Примерное время нагрева воды, в мин, в зависимости от выбранной программы (необходимой температуры)
int getTimeTenOn () {
  // Нагрев до 30 градусов
  if (current_program == 1 || current_program == 4 || current_program == 8 || current_program == 9 || current_program == 15) return 20; 
  
  // Нагрев до 40 градусов
  if (current_program == 3 || current_program == 7 || current_program == 10 || current_program == 16) return 27; 
  
  // Нагрев до 60 градусов
  if (current_program == 6 || current_program == 11) return 34;
  
  // Нагрев до 60 градусов "Эконом", наливаем чуть меньше воды, греем на самом деле до 50 градусов, стираем на 15 мин дольше, тем самым чуть-чуть экономим на всем
  if (current_program == 13) return 35; 
  
  // Нагрев до 90-95 градусов
  if (current_program == 12) return 50; 

  // Без нагрева и БЫСТРО 30 (нагрев осуществляется все время, пока идет стирка)
  return 0;
}

// Примерное время стирки, в мин., в зависимости от программы и дополнительных настроек
int getTimeWashing () {
  int time_ = 0;
  
  switch (current_program) {
    case 0: // слив
      time_ = TIME_Washing_0;
      break;
    case 1: // Ручная стирка 30
      time_ = TIME_Washing_1;
      break;
    case 2: // Ручная стирка ХВ
      time_ = TIME_Washing_1;
      break;
    case 3: // Шерсть 40
      time_ = TIME_Washing_2;
      break;
    case 4: // Шерсть 30
      time_ = TIME_Washing_2;
      break;
    case 5: // Шерсть ХВ
      time_ = TIME_Washing_2;
      break;
    case 6: // Синтетика 60
      time_ = TIME_Washing_3;
      break;
    case 7: // Синтетика 40
      time_ = TIME_Washing_3;
      break;
    case 8: // Синтетика 30
      time_ = TIME_Washing_3;
      break;
    case 9: // Хлопок 30
      time_ = TIME_Washing_4;
      break;
    case 10: // Хлопок 40
      time_ = TIME_Washing_4;
      break;
    case 11: // Хлопок 60
      time_ = TIME_Washing_4;
      break;
    case 12: // Хлопок 95
      time_ = TIME_Washing_4;
      break;
    case 13: // 60 ЭКО
      time_ = TIME_Washing_5;
      break;
    case 14: // Деликатное ХВ
      time_ = TIME_Washing_6;
      break;
    case 15: // Деликатное 30
      time_ = TIME_Washing_6;
      break;
    case 16: // Деликатное 40
      time_ = TIME_Washing_6;
      break;
    case 17: // Быстро 30
      time_ = TIME_Washing_7;
      break;
    case 18: // Полоскание + отжим
      time_ = TIME_Washing_8;
      break;
  }

  return time_;
}

// Примерное время отжима, в мин., в зависимости от дополнительных настроек
int getTimeRisingSpin () {
  int time_ = 0;
  if (fl_0 == 0 && current_program != 0 && current_program != 18) {
    time_ = DEFAULT_RINSING_COUNT * ( 2 + 4 + 1); // 2 - налив, слив, 4 - время полоскания, 1 - время отжима
  }
  if (fl_0 == 0 && current_program == 18) {
    time_ = DEFAULT_RINSING_COUNT * ( 2 + 4 + 1); // 2 - налив, слив, 4 - время полоскания, 1 - время отжима
  }
  if (fl_PoloskanirPlus == 1) {
    time_ = time_ +  2 + 4 + 1 + 10; // 2 - налив, слив, 4 - время полоскания, 1 - время отжима, 10 - время подогрема воды
  }
  return time_;
}


// время цикла, "Легкая глажка"
int getTimeEasy () {
  if (fl_Easy == 1) {
    return 1;
  } else {
    return 0;
  }
}
