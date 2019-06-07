//VARIABLES DE LOS SENSORES
#define SIG0 A0
#define SIG1 A1
#include <Wire.h>
unsigned long rxTime;
unsigned long endTime;
unsigned long startTime;

//VARIABLES DE LOS CALCULOS
const double horDist=7.5;
double pos1x=horDist/2;
double pos2x=-horDist/2;
double d0;
double d1;
double pos1y;
double pos2y;
double p0=30;//valores aleatorios por ahora
double p1=50;//valores aleatorios por ahora
double elapsedTime=1;
bool westward=false;//valores aleatorios por ahora

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
#define servoPin 32 // servo is to be attached on pin 32
Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
const double tiempoDisparoTrigger=1.0;

void setup() {
  Serial.begin(9600);
  Serial.print("Setup");
  //SETUP NEMA
  pinMode(stepPin,OUTPUT);
  pinMode(dirPin,OUTPUT);
  digitalWrite(dirPin,LOW);
  //SETUP SERVO
  myservo.write(30);
  myservo.attach(servoPin);
  delay(1000);

}
float readDistance(int SIG){
    pinMode(SIG, OUTPUT);
    //Genarate a pulse 20uS pulse
    digitalWrite(SIG, HIGH);
    delayMicroseconds(20);
    digitalWrite(SIG, LOW);
    //set SIG as INPUT,start to read value from the module
    pinMode(SIG, INPUT);
    rxTime = pulseIn(SIG, HIGH,100000);//waits for the pin SIG to go HIGH, starts timing, then waits for the pin to go LOW and stops timing
    double distance = (double)rxTime * 34 / 2000.0; //convert the time to distance
    return distance;
}

void setSensors(){
  while(d0 < 20){
    d0 = readDistance(SIG0);
    delay(500);
    Serial.println("----d0");
    Serial.println(d0);
  }
  while(d1 < 20){
    d1 = readDistance(SIG1);
    delay(500);
    Serial.println("----d1");
    Serial.println(d1);
  }
}

void detect(){
  do{
    delayMicroseconds(5000);
    p0 = readDistance(SIG0);
    delayMicroseconds(5000);
    p1 = readDistance(SIG1);
   
    if (p0<20){
      p0=d0;       
    }
    else{
      if(p1<20){
        p1=d1;
      }
    }
    Serial.println("----PO-P1");
    Serial.println(p0);
    Serial.println(p1);
    Serial.println("----");
  }while(abs(d0-p0) < 20 || abs(d1-p1) < 20);
  startTime=millis();
  if (d0-p0 > 20){
    westward = true;
    do{
      p1 = readDistance(SIG1);    
    }while(d1-p1 > 20);
    endTime=millis();
  }
  else{
    westward = false;
    do{
      p0 = readDistance(SIG0);    
    }while(d0-p0 > 20);
    endTime=millis();
  }  
  elapsedTime = (endTime - startTime)/1000;
  Serial.println(elapsedTime);
}

void calculations(){
  pos1y=p0;
  pos2y=p1;
  Serial.println("Calculating");
  if(westward){
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
  Serial.print(angMover);
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
  setSensors();
  detect();
  calculations();
  moverAngulo();
  disparo();
    while (true){
    Serial.println("Termino");
    delay(10000);
  }
}
