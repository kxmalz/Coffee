#include <DFRobot_HX711_I2C.h>
#include <LiquidCrystal.h>

DFRobot_HX711_I2C MyScale1(&Wire,/*addr=*/0x64);
DFRobot_HX711_I2C MyScale2(&Wire,/*addr=*/0x65);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
//DFRobot_HX711_I2C MyScale;
//scale
float Weight1 = 0;
float Weight2 = 0;
//pressure sensor
const float  OffSet = 0.483 ;
float V, P;
int licznik = 0;
int licznik1 = 0;



void setup() {
  //display
  // Wybór rodzaju wyświetlacza  - 16x2
  lcd.begin(16, 2); 
  //Przesłanie do wyświetlania łańcucha znaków hello, world!
  lcd.print("hello, world!");
  
  //scale
  Serial.println("Hello there!");
  Serial.begin(9600);
  while (!MyScale1.begin()) {
    Serial.println("The initialization of the chip is failed, please confirm whether the chip connection is correct");
    delay(1000);
  }
  while (!MyScale2.begin()) {
    Serial.println("The initialization of the chip is failed, please confirm whether the chip connection is correct");
    delay(1000);
  }
  /*
  //// Set the calibration weight when the weight sensor module is automatically calibrated (g)
  MyScale1.setCalWeight(100);
  // Set the trigger threshold (G) for automatic calibration of the weight sensor module. When only the weight of the object on the scale is greater than this value, the module will start the calibration process
  // This value cannot be greater than the calibration weight of the setCalWeight() setting
  MyScale1.setThreshold(30);
  // Obtain the calibration value. The accurate calibration value can be obtained after the calibration operation is completed
  Serial.print("the calibration value of the sensor is: ");
  Serial.println(MyScale1.getCalibration());
  MyScale1.setCalibration(MyScale1.getCalibration());
delay(1000);

    //// Set the calibration weight when the weight sensor module is automatically calibrated (g)
  MyScale2.setCalWeight(100);
  // Set the trigger threshold (G) for automatic calibration of the weight sensor module. When only the weight of the object on the scale is greater than this value, the module will start the calibration process
  // This value cannot be greater than the calibration weight of the setCalWeight() setting
  MyScale2.setThreshold(30);
  // Obtain the calibration value. The accurate calibration value can be obtained after the calibration operation is completed
  Serial.print("the calibration value of the sensor is: ");
  Serial.println(MyScale2.getCalibration());
  */
//  MyScale2.setCalibration(MyScale2.getCalibration());
  MyScale2.setCalibration(1763.81);
   
  delay(1000);
  

  //pressure sensor
        // open serial port, set the baud rate to 9600 bps
  Serial.println("Pressure and weight measurements");
  
}

void print_fixed(float x)
{
  char buffer[10];
  sprintf(buffer,"%5d.%02d",int(x),(int)((x-int(x))*100));
  Serial.print(buffer);
}

void loop() {
  if(licznik1){
    MyScale2.setCalibration(1763.81);
    }
    licznik1++;
  licznik ++;
  //scale
  Weight1 = MyScale1.readWeight(1);
  Weight2 = MyScale2.readWeight(1);

  
  
  //pressure
  V = analogRead(0) * 5.00 / 1024;     //Sensor output voltage
  P = (V - OffSet) * 237;             //Calculate water pressure
 //  Serial.print("Voltage:");
 // Serial.print(V, 3);
 // Serial.println("V");

  Serial.print("[");
  Serial.print(millis());
  Serial.print(",\t");
  Serial.print(Weight1, 1);
  Serial.print(",\t");
  Serial.print(Weight2, 1);
  Serial.print(",\t");
  Serial.print(P, 1);
  Serial.print("],");
  Serial.println();

  //display
  if(licznik == 25){
    licznik = 0;
  //Przejście kursora do pierwszej kolumny drugiego wiersza
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(Weight1, 1);
  lcd.print("g|: ");
  lcd.print(Weight2, 1);
  lcd.print("g");
  
  lcd.setCursor(0, 1);
  lcd.print(P, 1);
  lcd.print("kPa");
  //lcd.print(V,3);
  //lcd.print("V");
  }
}
