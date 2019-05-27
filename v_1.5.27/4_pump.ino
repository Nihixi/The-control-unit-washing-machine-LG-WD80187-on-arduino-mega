//******************** работа с насосом *******************************

// Не обязательный параметр, время работы насоса в сек, по умолчанию 5 минут
void pump_on (unsigned long timer) {
  digitalWrite(PIN_PUMP, HIGH);
  time_stop_pump = millis() + timer * 1000;
  flag_pump = true;
}

void pump_off () {
  ten_off (); // Защита от "дурака"
  digitalWrite(PIN_PUMP, LOW);
  flag_pump = false;
}

// Слив воды, с контролем уровня в баке
void drain_water () {
  ten_off (); // Защита от "дурака"
  addLog("Слив воды (drain_water): " + String(WATER_DRAIN_WAIT / 1000));
  pump_on(WATER_DRAIN_WAIT / 1000);
  unsigned long time_Stop_water = millis() + WATER_DRAIN_WAIT;
  addLog("окончания: " + String(time_Stop_water));
  while (millis() < time_Stop_water) {
    addLog("Уровень воды: " + String(current_water_level) + " Hz");
    if (current_water_level > WATER_LEVEL5) {
      delay2(5000);
      pump_off ();
      break;
    }
    delay2(1000);
  }
  if (current_water_level < WATER_LEVEL8) {
    error_0e ();
  }
}
