#define stepPin 3
#define dirPin 2
const double tiempoRotMotor=0.157079633; //obtenido experimentalmente, revisar informe motor
const double radPorPaso=0.01*PI; 
const double angMotor= (PI/2); //se asume que la posición del motor es de 90° con respecto a un plano
const double angDestino= (PI/4); //esto debería ser lo que el sensor entrega

void setup() {
  Serial.begin(19200);
  pinMode(stepPin,OUTPUT);
  pinMode(dirPin,OUTPUT);
  digitalWrite(dirPin,LOW);
}

void moverAngulo (){
  double angMover=angMotor-angDestino;
  int cantPasos=(abs(angMover)/radPorPaso)+1; //comprobado funciona bien
  if(angMover>0) digitalWrite(dirPin,LOW);
  else digitalWrite(dirPin,HIGH);
  for(int i=0;i<(cantPasos);i++){ //asumiendo que la división siempre da entera, por ser múltiplos enteros de pi, pregunta ¿verdaderamente es así? factor 5 o 6 
      digitalWrite(stepPin,HIGH);
      delayMicroseconds(2000);
      digitalWrite(stepPin,LOW);
      delayMicroseconds(2000);
  }
}
void loop() {
  // put your main code here, to run repeatedly:
  moverAngulo();
  while (true){
    Serial.println("Termino");
    delay(1000);
  }
}
