void setup()  {
  
  pinMode(PIN_BUTTON_INPUT1, INPUT);
  pinMode(PIN_BUTTON_INPUT2, INPUT);

  pinMode(PIN_VIBRO, INPUT); // Датчик вибрации
  digitalWrite(PIN_VIBRO, HIGH);

  pinMode(PIN_TEMP, INPUT);   // инициализирую реле  СМ

  pinMode(PIN_AC, OUTPUT);   // инициализирую реле  СМ
  digitalWrite(PIN_AC, LOW); // Отключаю питания СМ

  pinMode(PIN_BEEP, OUTPUT);   // инициализирую пищалку
  
  pinMode(PIN_TEN, OUTPUT);   // инициализирую реле ТЭН
  digitalWrite(PIN_TEN, LOW);

  pinMode(PIN_BLOCK_DOOR, OUTPUT);   // блокировка двери
  digitalWrite(PIN_BLOCK_DOOR, LOW);

  pinMode(PIN_NULL, INPUT); // Переход через 0
  digitalWrite(PIN_NULL, HIGH);

  pinMode(PIN_CHECK_DOOR, INPUT); // Проверка на закрытие дверцы
  digitalWrite(PIN_CHECK_DOOR, HIGH);

  pinMode(PIN_TACHO, INPUT); // Таходатчик
  digitalWrite(PIN_TACHO, HIGH);

  pinMode(PIN_MOTOR_OUT, OUTPUT); // Выход на мотор
  digitalWrite(PIN_MOTOR_OUT, LOW);

  pinMode(PIN_MOTOR_LR, OUTPUT); // Выход реле реверса мотора
  digitalWrite(PIN_MOTOR_LR, LOW);

  pinMode(PIN_WATER_VALVE1, OUTPUT);    // инициализирую пины клапанов налива 1, 2 и насоса слива
  digitalWrite(PIN_WATER_VALVE1, LOW);
  pinMode(PIN_WATER_VALVE2, OUTPUT);
  digitalWrite(PIN_WATER_VALVE2, LOW);
  pinMode(PIN_PUMP, OUTPUT);
  digitalWrite(PIN_PUMP, LOW);

  pinMode(PIN_VIBRO, INPUT); // инициализация пина с датчиком вибрации
  digitalWrite(PIN_VIBRO, HIGH);

  // Энкодер
  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);
  digitalWrite(ENC_A, HIGH);
  digitalWrite(ENC_B, HIGH);

  tone(PIN_BEEP, 3000, 50);

  timer_init_ISR_10Hz(_timer_MAIN);   //запустить главный таймер 1sec/10 = 100ms
  timer_init_ISR_500Hz(_timer_DISPLAY);

  display_init();

  Wire.begin();
  Wire.setClock(400000UL); // Set I2C frequency to 400kHz

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  
  Serial.begin(115200);
}
