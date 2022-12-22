#include <DFRobot_HX711_I2C.h>

/*
 * Pin configuration
 * A0 - flow meter signal (left group - gray wire)
 * A3 - pressure sensor signal (yellow wire)
 * 
 * 5V - all red wires
 * GND - all black / brown wires
 * 
 */

DFRobot_HX711_I2C MyScale1( &Wire, /*addr=*/ 0x65);
//DFRobot_HX711_I2C MyScale2( &Wire, /*addr=*/ 0x64);

// looping control
const unsigned long print_window_micros = 200e3;
const unsigned long measurement_window_micros = 0.2e3; 

unsigned long last_print_micros = 0;
unsigned long last_measurement_micros = 0;
unsigned long measurements_since_print = 0;

//flow meter
const int high_analog_signal = 800;
const int low_analog_signal = 200;

unsigned int rising_edges_since_print = 0;
int flow_signal_reading = -1;
int previous_state = -1;
int current_state = -1;

//pressure sensor
const float voltage_offset = 0.483;
const float pressure_voltage_slope = 237.0;

int pressure_signal_reading = -1;
float current_pressure = -1;

//scales
float weight_1 = 0;
float weight_2 = 0;

void setup() {
    Serial.begin(9600);
    Serial.println("Device restarting");
    
    Serial.println("Initializing scales");    
    while (!MyScale1.begin()) 
    {
      Serial.println("Scales failed to intialize, wrong port number?");
      delay(1000);
    }
    Serial.println((
    "Scales calibration value: "  
    + String(MyScale1.getCalibration())
    + "\n"
    ));

    MyScale1.setCalibration(2037.38);

    /*
    while (!MyScale2.begin()) 
    {
        Serial.println("The initialization of the chip is failed, please confirm whether the chip connection is correct");
        delay(1000);        
    }
    //  MyScale2.setCalibration(MyScale2.getCalibration());
    MyScale2.setCalibration(1763.81);
    delay(500);
    */

    //pressure sensor
    Serial.println("READY FOR MEASUREMENT");
    Serial.println("");
    Serial.println("");
    Serial.println("Pressure, weight and flow measurements");

}

void loop() {

    while (micros() - last_print_micros < print_window_micros) // print loop
    { 

        if (micros() - last_measurement_micros >= measurement_window_micros) // measure loop
        { 

            last_measurement_micros = micros();
            measurements_since_print++;

            // flow meter
            previous_state = current_state;
            flow_signal_reading = analogRead(A0);
            
            if (flow_signal_reading > high_analog_signal) current_state = 1;
            if (flow_signal_reading < low_analog_signal) current_state = 0;

            if (current_state == 1 && previous_state == 0) {                
                rising_edges_since_print++;                
            }

            // pressure
            pressure_signal_reading = (5.0 / 1024) * analogRead(A3);
            current_pressure = (pressure_signal_reading - voltage_offset) * pressure_voltage_slope;
        }
    }

    // weight
    weight_1 = MyScale1.readWeight();

    // printing
    Serial.print(
        "[" 
        + String(millis())
        + ",\t"
        + String(weight_1,1)
        + ",\t"
        + String(0.0,1)
        + ",\t"
        + String(current_pressure,1)
        + ",\t"
        + String(rising_edges_since_print)
        + "],\n"
    );

    /*
    // Use for setting correct value
    Serial.println((
    "Scales calibration value: "  
    + String(MyScale1.getCalibration())
    + "\n"
    ));
    */

    last_print_micros = micros();
    measurements_since_print = 0;
    rising_edges_since_print = 0;
}
