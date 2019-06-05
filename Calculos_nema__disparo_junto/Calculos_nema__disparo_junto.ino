
//VARIABLES DE LOS CALCULOS
const double horDist=7.5;
double pos1x=horDist/2;
double pos2x=-horDist/2;
double pos1y;
double pos2y;
double p0=100;//valores aleatorios por ahora
double p1=3;//valores aleatorios por ahora
double elapsedTime=1;
bool eastward=true;//valores aleatorios por ahora

//NEXO CALCULOS-NEMA
double angDestino;//conexion a movimiento motor

//VARIABLES DEL NEMA
#define stepPin 3
#define dirPin 2
const double tiempoRotMotor=0.157079633; //obtenido experimentalmente, revisar informe motor
const double radPorPaso=0.01*PI; 
const double angMotor= (PI/2); //se asume que la posición del motor es de 90° con respecto a un plano

//VARIABLES DEL SERVO TRIGGER
#include <Servo.h>
#define servoPin 9 // servo is to be attached on pin 9
Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
const double tiempoDisparoTrigger=1.0;

void setup() {
  Serial.begin(9600);
  //SETUP NEMA
  pinMode(stepPin,OUTPUT);
  pinMode(dirPin,OUTPUT);
  digitalWrite(dirPin,LOW);
  //SETUP SERVO
  myservo.write(30);
  myservo.attach(servoPin);
  delay(1000);

}
void calculations(){
  pos1y=p0;
  pos2y=p1;
  if(eastward){
    double desplX=-horDist;
    double desplY=pos2y-pos1y;
    double velocidadX=desplX/elapsedTime;
    double velocidadY=desplY/elapsedTime;
    double tiempoRequerido=tiempoRotMotor+tiempoDisparoTrigger;
    double posFinalX=pos2x+velocidadX*tiempoRequerido;
    double posFinalY=pos2y+velocidadY*tiempoRequerido;//ojo con el tiempo requerido
    angDestino=PI-abs(atan((posFinalY/posFinalX)));      
  }
  else{
    double desplX=horDist;
    double desplY=pos1y-pos2y;
    double velocidadX=desplX/elapsedTime;
    double velocidadY=desplY/elapsedTime;
    double tiempoRequerido=tiempoRotMotor+tiempoDisparoTrigger;
    double posFinalX=pos1x+velocidadX*tiempoRequerido;
    double posFinalY=pos1y+velocidadY*tiempoRequerido;
    angDestino=abs(atan(posFinalY/posFinalX));
  }
}

void moverAngulo (){
  double angMover=angMotor-angDestino;
  int cantPasos=(abs(angMover)/radPorPaso)+1; //comprobado funciona bien
  if(angMover>0) digitalWrite(dirPin,LOW);
  else digitalWrite(dirPin,HIGH);
  for(int i=0;i<(cantPasos);i++){
      digitalWrite(stepPin,HIGH);
      delayMicroseconds(2000);
      digitalWrite(stepPin,LOW);
      delayMicroseconds(2000);
  }
}
void disparo(){
    for (pos = 30; pos <= 120; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 5ms for the servo to reach the position
  }
}
void loop() {
  calculations();
  moverAngulo();
  disparo();
    while (true){
    Serial.println("Termino");
    delay(10000);
  }
}
