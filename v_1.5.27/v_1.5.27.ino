#include <Wire.h>
#include <timer-api.h>                            // подключаем библиотеку для работы с таймерами

#define PIN_BUTTON_INPUT1 22                      // Вход1 управления кнопок
#define PIN_BUTTON_INPUT2 24                      // Вход2 управления кнопок
#define PIN_MOTOR_OUT     4                       // выход на мотор
#define PIN_MOTOR_LR      11                      // реле реверса мотора
#define PIN_MOTOR_TURBO   12                      // режим ТУРБО мотора
#define PIN_BEEP          41                      // Выход на бипер
#define PIN_VIBRO         43                      // датчик вибрации
#define PIN_SET           0                       // установка оборотов в ручную (0-5 вольт  мин-макс, в сервисном режиме)
#define PIN_TEMP          1                       // датчик температуры
#define PIN_NULL          2                       // сигнал прохождения синусоиды через 0 (INT0)
#define PIN_TACHO         3                       // сигнал от таходатчика (INT1)
#define PIN_AC            5                       // управление питанием 220 v
#define PIN_TEN           6                       // управление ТЭН
#define PIN_BLOCK_DOOR    7                       // управление блокировкой двери
#define PIN_PUMP          8                       // управление насосом
#define PIN_WATER_VALVE1  9                       // управление клапаном подачи воды № 1
#define PIN_WATER_VALVE2  10                      // управление клапаном подачи воды № 2
#define PIN_CHECK_DOOR    A0                      // проверка блокировки двери (низкий уровень - дверь закрыта и заблокирована)
#define WATER_LEVEL0      25300                   // уровень воды для предварительной стирки
#define WATER_LEVEL1      25300                   // уровень воды для стирки
#define WATER_LEVEL2      25200                   // уровень воды для полоскания
#define WATER_LEVEL3      24500                   // уровень воды, при ее охлаждении, для программ стирки с температурой воды >= 60C
#define WATER_LEVEL4      23800                   // КРИТИЧЕСКИЙ уровень воды, нужно срочно отключать подачу воды, включать насос для слива, пищать и выводить ошибку
#define WATER_LEVEL5      28150                   // Уровень воды после слива 
#define WATER_LEVEL6      28200                   // Уровень воды после отжима
#define WATER_LEVEL7      26000                   // Минимальный уровень воды, когда можно включать ТЭН
#define WATER_LEVEL8      27000                   // Уровень воды, после слива, для появление ошибки насоса
#define WATER_LEVEL_delta 50                      // Дельта, на которую СМ переливает воду
#define WATER_LEVEL_TIME  90                      // Время, на которое открывается клапан забора воды, в сек., по умолчанию

#define temp_BIO          40                      // Температура БИО стирки
#define temp_PRE          20                      // Температура предварительной стирки

#define TIME_Washing_0    1                       // Время стирки в режиме "Слив"
#define TIME_Washing_1    24                      // Время стирки в режиме "Ручная стирка"
#define TIME_Washing_2    18                      // Время стирки в режиме "Шерсть"
#define TIME_Washing_3    35                      // Время стирки в режиме "Синтетика"
#define TIME_Washing_4    75                      // Время стирки в режиме "Хлопок"
#define TIME_Washing_5    90                      // Время стирки в режиме "Хлопок" - ЭКО
#define TIME_Washing_6    18                      // Время стирки в режиме "Деликатное"
#define TIME_Washing_7    11                      // Время стирки в режиме "Быстро 30"
#define TIME_Washing_8    0                       // Время стирки в режиме "Полскание+отжим"

#define PROGRAM_TIME_BIO  14                      // Время БИО стирки, в мин.
#define PROGRAM_TIME_PREWASHING 60                // Время предварительной стирки, в мин
#define PROGRAM_RINSING_TIME 240                  // Время одного цикла полоскания, в сек
#define PROGRAM_RINSING_WARM_TIME 600             // Время одного цикла полоскания в теплой воде, в сек
#define PROGRAM_RINSING_WARM_TEMP 20              // Температура теплого полоскания, градусах С
#define PROGRAM_PAUSE 10000                       // Пауза между этапами программ, в мсек

#define RINSING_TIME_MAX_F         10000          // Время отжима в мс, перого этапа, на скорости отжима
#define RINSING_TIME_MIN           15000          // Время отжима в мс, первого этапа на минимальной скорости, что бы убрать пену
#define RINSING_TIME_MID           10000          // Время отжима в мс, промежуточного этапа отжима, для набра скорости
#define RINSING_TIME_AFTER_WASHING 60000          // Время отжима в мс, после стирки
#define RINSING_TIME_AFTER_RINSING 60000          // Время отжима в мс, после полоскания
#define RINSING_TIME_END 60000                    // Время окончательного отжима
#define DEFAULT_PROG      8                       // Программа по умолчанию "Синтетика 30"
#define DEFAULT_RINSING_COUNT  4                  // Количество полосканий
#define DEFAULT_SPIN_COUNT  20                    // Количество попыток выхода на режим отжима
#define TIME_FOAM_REMOVAL_WATER 5                 // Время, на которое открывается клапан забора воды, для удаления пены в баке, в сек

#define ENC_A             42                      // Энкодер контакт A
#define ENC_B             44                      // Энкодер контакт B

#define WATER_WAIT       600000                   // Время, в течении которого ждем налив воды, в милисек.
#define WATER_DRAIN_WAIT 300000                   // Время, в течении которого ждем слив воды, в милисек.
#define WATER_WAIT_PRESOSTAT 60000                // Время, в течении которого ждем "нормальных данных" от пресостата
#define BUTTON_TIMER_WAIT 10                      // Задержка, для исключения дребезга контактов
#define BUTTON_TIMER_WAIT_CL 1000                 // Задержка, для включения/выключения режима "Блокировка от детей"

#define Reset() asm("JMP 0")                      // Макрос перезагрузки

const int MPU_addr = 0x68; // I2C address of the MPU-6050
double AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ, GyMid, AcMid, AcMax;

int encoder_value = 0;                            // Текущее значение энкодера, установленное в программе
int current_program = 0;                          // Текущая программа
int8_t encoderdata;                               // Полученные "сырое" значение (-1,0,1) из энкодера в процедуре read_encoder()
static int8_t enc_states[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0}; // Маска энкодера

volatile uint8_t tic, Dimmer;                     // Переменные регулятора скорости мотора
double pwmSet, pwmSpeed, pwmSpeedSec, pwmSpeedSecTmp, OldpwmSpeed, pwmOut;

int _timer_MOTOR  = _TIMER5;                      // Таймер для ШИМ мотора
int _timer_MAIN   = _TIMER4;                      // Основной таймер для работы различный процедур
int _timer_DISPLAY = _TIMER3;                     // Таймер вывод информации и опроса кнопок

int TimeLeft = 0;                                 // Осталось время до окнчания стирки
int TimeLeftOld = 0;                              // Осталось время до окнчания стирки, на прошлой итерации

int OLD_time_m = 999;                             // Оставшееся время
int Speed = 0;                                    // Скорость оборотов в минуту
int count_timer_motor = 0;                        // Счетчик таймера мотора
int count_timer_motor2 = 0;                       // Счетчик таймера мотора, для управления скоростью вращения
int countRetryWater = 6;                          // Количество попыток налива воды (цикл: налив до нужного уровня, вращение, измерение, если мало еще один цикл)
double count_timer_tacho = 0;                     // Счетчик таймера ТАХО датчика
double count_tacho = 0;                           // Счетчик импульсов от ТАХО датчика, за 0.1 сек
int count_water_gz = 0;                           // Счетчик импульсов от пресостата
double count_1000ms = 0;                          // Счетчик 1 сек
double sum_count_tacho = 0;                       // Сумма счетчика импульсов от ТАХО датчика, за 1 сек

int displayBuf[8];                                // Буфер для отображения информации на передней панели

boolean WORK = false;                             // Флаг работы (включеной) стиральной машины в любом режиме
boolean PAUSE = true;                             // Флаг режима PAUSE
boolean HeatingWaterWashing = false;              // Флаг нужно ли подогревать воду во время стрики, если вода остыла

boolean flag_pump = false;                        // Флаг работы насоса
boolean flag_water_valte1 = false;                // Флаг включения клапана 1
boolean flag_water_valte2 = false;                // Флаг включения клапана 2
boolean flag_ten = false;                         // Флаг включения ТЭНа
unsigned long time_stop_pump;                     // Время отключения насоса
unsigned long time_stop_water_valte1;             // Время отключения клапана 1
unsigned long time_stop_water_valte2;             // Время отключения клапана 2
unsigned long time_stop_ten;                      // Время отключения тена

bool fl_Vibro = false;                            // Флаг сработки датчика вибрации
bool fl_Vibro_Control = false;                    // Флаг контроля вибрации


bool MOTOR_ON = false;                            // Мотор вкл/выкл
bool MOTOR_direction = false;                     // направление вращения барабана (прямое)
bool fl_add = false;
bool fl_del_p = true;                             // Флаг удаления пены после стирки
int MOTOR_POWER_MINIMAL_ORIGINAL = 100;           // Минимальное значение мощности, что бы пустой барабан начал крутиться
int MOTOR_POWER_MINIMAL = MOTOR_POWER_MINIMAL_ORIGINAL;
int MOTOR_POWER_Array[100];                       // Массив, в котором хранится значение мощности, при необходимом вращении барабана, с нужной скоростью
int MOTOR_POWER_Array_pointer = 0;                // Указатель на текущий элемент массива MOTOR_POWER_Array
unsigned long MOTOR_POWER_Time_Ten = 60000;       // Крутить барабаном во время подогрева воды будем 1 раз в минуту, в мс
unsigned long MOTOR_STOP_Time_Wait = 60000;       // Максимальное время ожидания остановки вращения барабана, в мс
int MOTOR_POWER_Time_calibration = 30;            // Крутить барабаном MOTOR_POWER_Time_calibration сек, для калибровки мощности
int MOTOR_POWER_Time_Esy_ON = 15;                 // Крутить барабаном MOTOR_POWER_Time_Esy_ON сек, во время "легкой глажки"
int MOTOR_POWER_Time_Ten_ON = 15;                 // Крутить барабаном MOTOR_POWER_Time_Ten_ON сек, во время подогрева воды
int MOTOR_POWER_Time_Water_ON = 20;               // Крутить барабаном MOTOR_POWER_Time_Water_ON сек, во время налива воды
int MOTOR_POWER_Time_washing = 20;                // Крутить барабаном MOTOR_POWER_Time_washing сек, во время стирки
int MOTOR_POWER_Time_rinsing = 20;                // Крутить барабаном MOTOR_POWER_Time_rinsing сек, во время полоскания
int MOTOR_POWER_Time_STRAIGHTEN = 15;             // Крутить барабаном MOTOR_POWER_Time_STRAIGHTEN сек, во время "расправления" его в барабане
unsigned long  MOTOR_POWER_Time_START = 0;        // Время, когда мотор был включен
unsigned long  MOTOR_POWER_Time_WORK = 180000;    // Максимальное время, которое мотор может работать, контролируется прерыванием, по умолчанию 3 мин.
int MOTOR_SPEED_Water_ON = 5;                     // Крутить барабаном со скоростью MOTOR_SPEED_Water_ON, во время налива воды
int MOTOR_SPEED_rate = 6;                         // Коэффицент пересчета обротов мотора, в обороты/мин
int MOTOR_SPEED_STRAIGHTEN_MIN1 = 4;              // Минимальная скорость при "расправления" белья
int MOTOR_SPEED_STRAIGHTEN_MAX1 = 6;              // Максимальная скорость при "расправления" белья
int MOTOR_SPEED_STRAIGHTEN_MIN2 = 5;              // Минимальная скорость при выходе на отжим
int MOTOR_SPEED_STRAIGHTEN_MAX2 = 10;             // Максимальная скорость при выходе на отжим (примерно 55 об/мин)
int MOTOR_SPEED_STRAIGHTEN_UNBALANCE = 10;        // Cкорость определения дисбаланса (примерно 55 об/мин)
int MOTOR_SPEED_STRAIGHTEN_MAX3 = 20;             // Скорость при выходе на отжим (примерно 100 об/мин)
int MOTOR_SPEED_STRAIGHTEN_MAX4 = 25;             // Скорость при выходе на отжим (примерно 110 об/мин)
int MOTOR_SPEED_STRAIGHTEN_MAX4_1 = 35;           // Скорость отжима, при снижении оборотов (примерно 200 об/мин)
int MOTOR_SPEED_STRAIGHTEN_MAX5 = 40;             // Скорость при выходе на отжим (примерно 220 об/мин)
int MOTOR_SPEED_STRAIGHTEN_MAX6 = 70;             // Скорость при выходе на отжим (примерно 400 об/мин)
int MOTOR_SPEED_STRAIGHTEN_MAX7 = 90;             // Скорость при выходе на отжим (примерно 500 об/мин)
int MOTOR_SPEED_STRAIGHTEN_MAX8 = 110;            // Скорость при выходе на отжим (примерно 600 об/мин)
int MOTOR_SPEED_STRAIGHTEN_MAX9 = 1000;           // Максимальная скорость, задана с большим запасом
int MOTOR_SPEED_STRAIGHTEN_TIMER = 2000;          // Задержка, используется при увеличении скорости, при выходе на отжим, в мс
int MOTOR_WAIT_STRAIGHTEN_TIMER = 30000;          // Задержка, используется для определения дисбаланса
int MOTOR_WAIT_STRAIGHTEN_TIMER2 = 10000;         // Задержка, используется для определения дисбаланса
double pwmSetMin = 0;                             // Минимальная заданная скорость (расчитывается автоматически)
double pwmSetMax = 0;                             // Максимальная заданая скорость (расчитывается автоматически)
unsigned long millis_m_s = 0;                     // Тех. переменная используется в процеруде отжима
int minimalPower = 0;

int washing_speed0 = 4;                           // 0 скорость для стирки
int washing_speed1 = 5;                           // 1 скорость для стирки
int washing_speed2 = 6;                           // 2 скорость для стирки
int rinsing_speed = 5;                            // скорость для полоскания
int washing_pause0 = 5000;                        // Пауза между вращениями барабана, во время программы БЫСТРО 30, в режиме "Легкая глажка", в милисек
int washing_pause1 = 8000;                        // Пауза между вращениями барабана, во время обычной стирки, в милисек
int washing_pause2 = 60000;                       // Пауза между вращениями барабана, во время ручной стирки, в милисек
int washing_pause3 = 30000;                       // Пауза между вращениями барабана, во время программы ШЕСТЬ, в милисек
int washing_pause4 = 20000;                       // Пауза между вращениями барабана, во время программы ДЕЛИКАТНАЯ, в милисек
int washing_pause5 = 65000;                       // Пауза между вращениями барабана, во время предварительной стирки, в милисек
int rinsing_pause = 10000;                        // Пауза между вращениями барабана, во время полоскания, в милисек
int washing_temp_delta = 10;                      // дельта в %, от рабочей температуры, при которой включаеся ТЭН для подогрева воды, во время стирки

int mode_spin = 800;                              // скорость для отжима, по умолчанию

int mode_child_lock = 0;                          // блокировка от детей, по умолчанию

int RINSE_SPIN_SUM = 4;                           // Кол-во циклов полоскания+отжим, минимум 4

int TEN_current_temp = 10;                        // Текущая температура воды в баке, переопределяется в get_real_Temperatura()
int TEN_STOP_temp = 90;                           // Предельная температуры воды, при которой ТЭН отключается автоматически. Нагрев контролируется в процедуре water_heating, это доп. контроль для отключения ТЭНА в таймере;

int current_water_level;                          // Текущий уровень водцы в баке, измеряется в kHz
int Washing_temperature = 30;                     // Заданная температура стирки, зависит от выбранной программы

unsigned long time_wait_heating = 3600000;        // Время включения ТЭНА и время ожидания нагрева воды, в миллисекундах

int DEBUG_LEVEL = 3;                              // Включать в лог отладочную информацию, чем больше значение, тем больше отладочной информации (0 - 3), 0 - не влючать отладочную информацию

// Кнопки
boolean BUTTON_ON = false;                        // Вкл
boolean fl_BUTTON_ON = false;
boolean BUTTON_SPIN = false;                      // Отжим
boolean fl_BUTTON_SPIN = false;
boolean BUTTON_TIMER = false;                     // Таймер отсрочки
boolean fl_BUTTON_TIMER = false;
boolean BUTTON_PREWASHING = false;                // Предварительная стирка, легкая глажка
boolean BUTTON_PREWASHING2 = false;
boolean fl_BUTTON_PREWASHING = false;
boolean BUTTON_BIOWASHING = false;                // BIO стирка, полоскание+
boolean BUTTON_BIOWASHING2 = false;
boolean fl_BUTTON_BIOWASHING = false;
boolean BUTTON_START_PAUSE = false;               // Старт/пауза
boolean fl_BUTTON_START_PAUSE = false;
boolean BUTTON_CL = false;                        // блокировка от детей
boolean fl_BUTTON_CL = false;
boolean fl_BUTTON_CL_DONE = false;

// Время первого срабатывания кнопки или 0 (используется для подавления дребезга контактов)
unsigned long Last_Time_BUTTON_ON = 0;
unsigned long Last_Time_BUTTON_SPIN = 0;
unsigned long Last_Time_BUTTON_TIMER = 0;
unsigned long Last_Time_BUTTON_PREWASHING = 0;
unsigned long Last_Time_BUTTON_PREWASHING2 = 0;
unsigned long Last_Time_BUTTON_BIOWASHING = 0;
unsigned long Last_Time_BUTTON_BIOWASHING2 = 0;
unsigned long Last_Time_BUTTON_START_PAUSE = 0;
unsigned long Last_Time_BUTTON_CL = 0;

int prog[19];                 // Массив программ, для отображения в эенкодере
int fl_Bio = 0;               // флаг Биостирки
int fl_PoloskanirPlus = 0;    // Флаг Подоскание+
int fl_Del_payten = 0;        // Флаг Удаления пятен (предварительная стирка)
int fl_Easy = 0;              // Флаг Легкая глажка
int fl_800 = 0;               // Флаг 800 оборотов
int fl_600 = 0;               // Флаг 600 оборотов
int fl_500 = 0;               // Флаг 500 оборотов
int fl_400 = 0;               // Флаг 400 оборотов
int fl_0 = 0;                 // Флаг Без отжима
int fl_Wait = 0;              // Флаг Отложенного отжима
int fl_StartStop = 0;         // Флаг индикатора start/pause
int fl_BlockB = 0;            // Флаг индикатора Блокировки
int fl_Dots = 0;              // Флаг разделителя
int fl_mode_child_lock = 0;   // Флаг отображения блокировки от детей
char indicator[4] = "   ";    // Символы для индикатора, если 3 символа, то массив должен быть на 4 элемента, т.к. последним элеменом должен быть пустой символ.
int indicator0;
int indicator1;
int indicator2;

// Сервисные переменные
boolean debug_flag_pump = false;
boolean debug_water_valte1 = false;
boolean debug_water_valte2 = false;
boolean debug_ten = false;
boolean debug_motor_manual = false;
boolean debug_motor_reverce = false;
int Serial_val = 0;
int currSpeedM = 5;
int pwmSet1 = 0;

// Function prototypes
void water_heating (int temp = 30, int aboutTime = 0);
void ten_on (unsigned long timer = 300); // Не обязательный параметр, время работы тена в сек, по умолчанию 5 минуты
void water_valte1_on (unsigned long timer = 30);
void water_valte2_on (unsigned long timer = 30);
void water_valte_all_on (unsigned long timer = 30);
void Motor(boolean MOTOR_direction, int MOTOR_speed, unsigned long MOTOR_time);
void setPower(int MOTOR_power);
void pump_on (unsigned long timer = 300);
void washing(unsigned long washing_time, int temp_MAIN, int washing_speed, int washing_pause);
void addLog(String txt);
void setTime (int time_m);
void  getTotalTime(int currentStep, int timeAdd);


void loop() {

  if (BUTTON_START_PAUSE) {
    addLog("BUTTON_START_PAUSE");
    BUTTON_START_PAUSE = false;
    PAUSE = !PAUSE;
    getTotalTime(0, 0);
    setInfoDisplay();
    tone(PIN_BEEP, 3000, 50);

    if (!PAUSE) {
      // Вкл. блокировку отдетей принудительно
      mode_child_lock = 1;
      setInfoDisplay();
      RunProgram();
    } else {
      mode_child_lock = 0;
      setInfoDisplay();
    }
  }

  if (BUTTON_ON) {
    addLog("BUTTON_ON");
    BUTTON_ON = false;
    WORK = !WORK;
    tone(PIN_BEEP, 3000, 50);
    if (WORK) {
      On_Mashine();
    } else {
      Off_Mashine();
      Off_Display();
    }
  }

  if (BUTTON_PREWASHING) {
    addLog("BUTTON_PREWASHING");
    BUTTON_PREWASHING = false;
    if (current_program != 0 && current_program != 18) {
      Event_Buttom_PREWASHING ();
      getTotalTime(0, 0);
      setInfoDisplay();
      tone(PIN_BEEP, 3000, 50);
    }
  }

  if (BUTTON_TIMER) {
    addLog("BUTTON_TIMER");
    BUTTON_TIMER = false;
  }

  if (BUTTON_SPIN) {
    BUTTON_SPIN = false;
    if (current_program != 0) {
      Event_Buttom_Spin();
      getTotalTime(0, 0);
      setInfoDisplay();
      tone(PIN_BEEP, 3000, 50);
    }
  }

  if (BUTTON_BIOWASHING) {
    addLog("BUTTON_BIOWASHING");
    BUTTON_BIOWASHING = false;
    if (current_program != 0 && current_program != 18) {
      Event_Buttom_Bio ();
      getTotalTime(0, 0);
      setInfoDisplay();
      tone(PIN_BEEP, 3000, 50);
    }
  }

  if (BUTTON_CL) {
    addLog("BUTTON_CL");
    BUTTON_CL = false;
    fl_BUTTON_CL_DONE = true;
    mode_child_lock = !mode_child_lock;
    if (!mode_child_lock) setInfoDisplay();
    tone(PIN_BEEP, 3000, 50);
  }

  Service();

}
