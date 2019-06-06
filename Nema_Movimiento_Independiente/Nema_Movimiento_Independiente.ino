#define stepPin 3
#define dirPin 2
const double tiempoRotMotor=0.157079633; //obtenido experimentalmente, revisar informe motor
const double radPorPaso=0.01*PI; 
const double angMotor= (PI/2); //se asume que la posición del motor es de 90° con respecto a un plano
const double angDestino= (PI/4); //esto debería ser lo que el sensor entrega

void setup() {
  Serial.begin(19200);

}


void loop() {
  // put your main code here, to run repeatedly:
  
  while (true){
    Serial.println("Termino");
    delay(1000);
  }
}
