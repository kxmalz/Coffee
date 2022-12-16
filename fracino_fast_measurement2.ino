/*
 * Podpięcia pinów:
 * Analog:
 * 4 - preassure
 * 1 - flow left group
 * 2 - flow right group
 * 
 */

#include <DFRobot_HX711_I2C.h>

DFRobot_HX711_I2C MyScale1(&Wire,/*addr=*/0x64);
DFRobot_HX711_I2C MyScale2(&Wire,/*addr=*/0x65);

//DFRobot_HX711_I2C MyScale;
//scale
float Weight1 = 0;
float Weight2 = 0;
//pressure sensor
const float  OffSet = 0.483 ;
float V, P;

//flow
float fm_left = 0;//pomiar z lewej grupy A0
float fm_right = 0; //pomiar z prawej grupy A1
unsigned long czas_printu = 0;
unsigned long czasStartu = 0;
unsigned long aktualnyCzas = 0;
unsigned long zapamietanyCzas = 0;
unsigned long roznicaCzasu = 0;
unsigned long licznik = 0;
unsigned long licznik_narastajacych = 0;

unsigned long czas_okna = 1000000UL;
unsigned long czas_pomiaru = 100UL;

int poprzedni_pomiar = -1;
int aktualny_pomiar = -1;

const int maximum = 100;
int preasures[maximum];

void setup() {
  Serial.begin(9600);
 /* while (!MyScale1.begin()) {
    Serial.println("The initialization of the chip is failed, please confirm whether the chip connection is correct");
    delay(1000);
  }
  while (!MyScale2.begin()) {
    Serial.println("The initialization of the chip is failed, please confirm whether the chip connection is correct");
    delay(1000);
  }
 
//  MyScale2.setCalibration(MyScale2.getCalibration());
  MyScale2.setCalibration(1763.81);
   
  delay(500);*/
  

  //pressure sensor
  Serial.println("Pressure, weight and flow measurements");
   
}

void loop() {

  

  //flow
  czasStartu = micros();

  int state = -1;
  int prev_state = -1;
  const int high = 800;
  const int low = 200;
  
  while(micros()-czasStartu < 1000000UL){ // print loop

    
    if (micros() - zapamietanyCzas >= 1000UL) { // measure loop

        zapamietanyCzas = micros();
        poprzedni_pomiar = aktualny_pomiar;
        aktualny_pomiar = analogRead(A0);


        if(aktualny_pomiar > high)
        {
          state = 1;
        }

        if(aktualny_pomiar < low)
        {
          state = 0;
        }
        
       preasures[0]= analogRead(A5);
       licznik ++;
       if (prev_state != -1 && state != -1 && state == 1 && prev_state == 0){
         licznik_narastajacych ++;
       }

       prev_state = state;

       
       
      }
  }

/*
Serial.println("Flow read");
Serial.print("Liczba narastajacych: ");
Serial.println(licznik_narastajacych);
Serial.print("Liczba ogolem: ");
Serial.println(licznik);
Serial.print("W czasie: ");
Serial.println(millis()-czasStartu);*/

/*
  //scale
//  Weight1 = MyScale1.readWeight(1);
 // Weight2 = MyScale2.readWeight(1);*/
  //pressure

  float V_sum = 0;
  for(int i = 0; i<licznik; i++){

    V_sum = V_sum + preasures[i];
    }
  V = (V_sum/licznik) * (5.00 / 1024);     //Sensor output voltage
  P = (V - OffSet) * 237;            //Calculate water pressure

czas_printu = micros();
  Serial.print("[");
  Serial.print(millis());
  Serial.print(",\t");
//  Serial.print(Weight1, 1);
  Serial.print(",\t");
//  Serial.print(Weight2, 1);
  Serial.print(",\t");
  Serial.print(P, 1);
  Serial.print(",\t");
  Serial.print(licznik_narastajacych, 1);
  Serial.print("],");
  Serial.println();
  Serial.println(micros()-czas_printu);


czasStartu = 0;
aktualnyCzas = 0;
zapamietanyCzas = 0;
roznicaCzasu = 0;
licznik = 0;
licznik_narastajacych = 0;

poprzedni_pomiar = -1;
aktualny_pomiar = -1;
Serial.println("new");
  
  /*fm_left = analogRead(A1) * (5.0/1024.0);
  fm_right = analogRead(A2) * (5.0/1024.0);


  
*/


  
}
