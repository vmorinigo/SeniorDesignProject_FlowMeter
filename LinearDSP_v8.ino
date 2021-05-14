/* -----------------------------------------------  LINEAR  -------------------------------------------------------------- */
#define LIN_OUT 1 // use the lin output function
#define FHT_N 256 // set sample size

#include <FHT.h> // include the library

const unsigned long READ_PERIOD = 8;
int signalPin = A5;  // input pin for the flow meter
int finalValue = 0;// variable to store the value without DC offset
int max_fht_lin;
int max_index;
//float delayTime = 8 ;
float max_freq;
float f_max = 500 / (float)READ_PERIOD;
int i = 0;
unsigned long lastRead = 0;

//  Max Analog Discovery frequency =  64 Hz 

void setup() {
  Serial.begin(115200);
}

void loop() {
  /* that is, if the difference between the current time and last time we grabbed the ADC value is bigger than the READ_PERIOD.with the following parameters to achieve desired sampling:*/
    /* ---------------------------------- ADC ------------------------------ */
//    Serial.println("--------------------- START --------------------------------");
    if(millis() >= lastRead + READ_PERIOD){
        lastRead += READ_PERIOD;
        finalValue = analogRead(signalPin) - 485; // read the value from the analog channel and subtract DC offset of 2.37V
        fht_input[i] = finalValue * 64; // save values into bins
        i++;
        if (i == FHT_N)
        {
          Harley_Transform();
          Max_Value();
          i = 0;
          max_fht_lin = 0;
          max_index = 0;
          max_freq = 0; 
        }
    }
}

void Harley_Transform(){
  fht_window(); // window the data for better frequency response
  fht_reorder(); // reorder the data before doing the fht
  fht_run(); // process the data in the fht 
  fht_mag_lin(); // take the output of the fht
}

void Max_Value(){
  for (int i = 0; i < 102; i++) {
    if (fht_lin_out[i] > max_fht_lin) {
      max_fht_lin = fht_lin_out[i];
      max_index = i;
    }
  }
  max_freq =  ( ( (float)max_index / 128.0) * f_max);
  Serial.print("Frequency = ");
  Serial.println(max_freq);
  /* --------------------------------------------- WARNINGS ---------------------------------------------- */
  // Low is less than 5Hz, High flow more than 50Hz
  if(max_freq < 5.0)
  {
    Serial.println("too low");
  }else if (max_freq > 50.0){
    Serial.println("too high");
  }else{
    Serial.println("good flow");
    }
  Serial.flush();
}
