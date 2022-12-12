

float fm_left = 0;//pomiar z lewej grupy A0
float fm_right = 0; //pomiar z prawej grupy A1
int licznik = 0;

void setup() {
  Serial.begin(9600);//Uruchomienie komunikacji przez USART
  Serial.println("Flow measure");
}


void loop() {
  fm_left = analogRead(A0) * (5.0/1024.0);
  fm_right = analogRead(A1) * (5.0/1024.0);

  Serial.print(millis());
  Serial.print(",\t");
  Serial.print(fm_left);
  Serial.print(",\t");
  Serial.print(fm_left);

  Serial.println();
}
