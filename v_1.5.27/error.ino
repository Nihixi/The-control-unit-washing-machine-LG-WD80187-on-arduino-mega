//********************обработка ошибок*******************************
// Коды ошибок, соответсвуют руководству пользователя

// Ошибка - Переполнение водой
void error_fe () {
  indicator[0] = 22;  // 
  indicator[1] = 13;  // F
  indicator[2] = 12;  // E
  setInfoDisplay();
  drain_water(); // Включим откачку воды
  StopMashine();
}

// Ошибка - проблемы с подачей воды
void error_1e () {
  indicator[0] = 22;  // 
  indicator[1] = 1;   // 1
  indicator[2] = 12;  // E
  setInfoDisplay();
  StopMashine();
}

// Ошибка - проблемы с дренажем
void error_0e () {
  indicator[0] = 22;  // 
  indicator[1] = 0;   // 0
  indicator[2] = 12;  // E
  setInfoDisplay();
  StopMashine();
}

// Ошибка - проблемы в датчике уровня воды
void error_pe () {
  indicator[0] = 22;  // 
  indicator[1] = 16;  // P
  indicator[2] = 12;  // E
  setInfoDisplay();
  StopMashine();
}

// Ошибка - проблемы с мотором
void error_ce () {
  indicator[0] = 22;  // 
  indicator[1] = 11;  // C
  indicator[2] = 12;  // E
  setInfoDisplay();
  StopMashine();
}

// Ошибка - зависание программы
void error_int () {
  indicator[0] = 1;   // I
  indicator[1] = 19;  // n
  indicator[2] = 21;  // t
  setInfoDisplay();
}

// Ошибка - дверца не закрыта
void error_de () {
  indicator[0] = 22;  // 
  indicator[1] = 18;  // d
  indicator[2] = 12;  // E
  setInfoDisplay();
}

// Ошибка - не возмоно распределить белье
void error_ue () {
  indicator[0] = 22;  // 
  indicator[1] = 17;  // U
  indicator[2] = 12;  // E
  setInfoDisplay();
  StopMashine();
}
