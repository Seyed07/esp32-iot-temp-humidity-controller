#include "DHTesp.h" 
#include <Ticker.h>
#include <BluetoothSerial.h>
#include <ESP32Servo.h> 

// تعیین کردن اسم بلوتوث برد esp32
String device_name = "ESP32-BT-Slave";
DHTesp dht;
BluetoothSerial SerialBT;

Servo myservo; // سروو موتور برای چرخش زاویه دمنده
Servo myservo2; // سروو موتور برای فن 

int servopin = 13; // چرخش زاویه
int servopin2 = 12; // فن
char r; // کارکتر دریافتی 
int x; // عدد دریافتی برای ست پوینت

//*************باز طراحی کتابخونه DHT11 **********************/
TaskHandle_t tempTaskHandle = NULL;
Ticker tempTicker;
ComfortState cf;
bool tasksEnabled = false;
int dhtPin = 17;
float currentTemperature = 0; 
int setpoint = 25; 

// Function declarations
void tempTask(void *pvParameters);
bool getTemperature();
void triggerGetTemp();
bool initTemp();
//*************************************************************/

void setup() {
  Serial.begin(115200);
  SerialBT.begin(device_name); //روشن کردن بلوتوث
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  SerialBT.begin(115200);
  SerialBT.println("DHT ESP32 example with tasks");
  pinMode(14, OUTPUT); // چراغ: فن
  pinMode(27, OUTPUT); // چراغ : هیتر
  pinMode(25, OUTPUT); // چراغ : setpoint

  initTemp();
  tasksEnabled = true;

  myservo.attach(servopin); // فعال کردن سروو موتور برای چرخش زاویه 
  myservo2.attach(servopin2); // فعال کردن سروو موتور برای فن
}
//*************************************************************/

void loop() 
{
  if (!tasksEnabled)
  {
    delay(2000);
    tasksEnabled = true;
    if (tempTaskHandle != NULL) 
    {
      vTaskResume(tempTaskHandle);
    }
  }


  if (Serial.available())
  {
    SerialBT.write(Serial.read());
  }

//***************************تعیین setpoint*****************************/

  if (SerialBT.available()) 
  {
    r = SerialBT.read();
    Serial.write(r);
    if (isdigit(r)) 
    {
      x = (x * 10) + (r - '0'); // تبدیل کارکتر به متغییر اینتجر
    } 
    else if (r == '\n') 
    {
      setpoint = x; // ست کردن ست پوینت
      x = 25; // درصورت اینکه ست پونتی ست نشد ، 25 درجه ست میشود 
      SerialBT.printf("Setpoint updated to: %d\n", setpoint);
    }
  }

//***************** *****چرخش زایه ایی دریچه ******************/

  if (r == 'A') {
    for (int posDegrees = 0; posDegrees <= 120; posDegrees++) {
      myservo.write(posDegrees);
      Serial.println(posDegrees);
      delay(20);
    }
  }

  if (r == 'C') {
    for (int posDegrees = 120; posDegrees >= 0; posDegrees--) {
      myservo.write(posDegrees);
      Serial.println(posDegrees);
      delay(20);
    }
  }

//*****************************کنترل setpoint***************************/


  if (currentTemperature >= setpoint - 5 && currentTemperature <= setpoint + 5){
      digitalWrite(25, HIGH); // setpoint
      if(currentTemperature > setpoint - 5 && currentTemperature < setpoint ){ 
          digitalWrite(27, HIGH); //heater
          digitalWrite(14, LOW); // fan
    }
      if(currentTemperature < setpoint + 5 && currentTemperature > setpoint){
          digitalWrite(14, HIGH); // fan
          digitalWrite(27, LOW); //heater
          
        for (int posDegrees = 0; posDegrees <=  90; posDegrees++) {
        myservo2.write(posDegrees % 180); 
        delay(5);
        }
      } 
  }
  if(currentTemperature > setpoint+5 or currentTemperature < setpoint - 5) 
  {
    digitalWrite(25, LOW); //setpoint
    if(currentTemperature > setpoint+5) 
      {
       digitalWrite(14, HIGH); //fan 
       digitalWrite(27, LOW); // heater
        for (int posDegrees = 0; posDegrees <= 90; posDegrees++) {
        myservo2.write(posDegrees % 180); 
        delay(5);
        }
      }
    if(currentTemperature<setpoint-5)
      {
      digitalWrite(27, HIGH); // heater  
      digitalWrite(14, LOW); // fan
      }
  }


//***************************پایان حلقه ********************************/  
}


bool initTemp() {
  dht.setup(dhtPin, DHTesp::DHT11);
  Serial.println("DHT initiated");

  xTaskCreatePinnedToCore(
    tempTask,
    "tempTask",
    4000,
    NULL,
    5,
    &tempTaskHandle,
    1);

  if (tempTaskHandle == NULL) {
    Serial.println("Failed to start task for temperature update");
    return false;
  } else {
    tempTicker.attach(1.3, triggerGetTemp);
  }
  return true;
}

void triggerGetTemp() {
  if (tempTaskHandle != NULL) {
    xTaskResumeFromISR(tempTaskHandle);
  }
}

void tempTask(void *pvParameters) {
  Serial.println("tempTask loop started");
  while (1) {
    if (tasksEnabled) {
      getTemperature();
    }
    vTaskSuspend(NULL);
  }
}

bool getTemperature() {
  TempAndHumidity newValues = dht.getTempAndHumidity();
  if (dht.getStatus() != 0) {
    Serial.println("DHT11 error status: " + String(dht.getStatusString()));
    return false;
  }

  currentTemperature = newValues.temperature*5.5; // Store the current temperature

  SerialBT.print(" Temperature:" + String(currentTemperature));
  SerialBT.print("\n");
  return true;
}
