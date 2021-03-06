//VARIABLES DEL MANDO (LEDS Y BOTONES)
#define pinLed1 8
#define pinLed2 9
#define pinLed3 10

#define pinBotonIzq 11
#define pinBotonDer 12
#define pinBotonGo 13
int lecturaBotonIzq;
int lecturaBotonDer;
int lecturaBotonGo;

//VARIABLES DE LOS SENSORES
#define SIG1 A3
#define SIG0 A2
#include <Wire.h>
unsigned long rxTime;
unsigned long endTime;
unsigned long startTime;



//VARIABLES DE LOS CALCULOS
const double horDist=9.5;
double pos1x=horDist/2;
double pos2x=-horDist/2;
double d0;//DISTANCIA DE SENSOR 0
double d1;//DISTANCIA DE SENSOR 1
double pos1y;
double pos2y;
double p0;//POSICION GUARDADA
double p1;//POSICION GUARDADA
double elapsedTime;
const double tiempoDisparoTrigger=0.5;
bool westward=false;//valores aleatorios por ahora

//NEXO CALCULOS-NEMA
double angDestino;//conexion a movimiento motor

//VARIABLES DEL NEMA
#define stepPin 3
#define dirPin 2
#define enablePin 24
int cantPasos;
const double tiempoRotMotor=0.157079633; //obtenido experimentalmente, revisar informe motor
const double radPorPaso=0.01*PI; 
const double angMotor= (PI/2); //se asume que la posición del motor es de 90° con respecto a un plano

//VARIABLES DEL SERVO TRIGGER
#include <Servo.h>
#define servoPin 32 // servo is to be attached on pin 32
Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  //SETUP MANDO (LEDS Y BOTONES)
  pinMode(pinLed1,OUTPUT);
  pinMode(pinLed2,OUTPUT);
  pinMode(pinLed3,OUTPUT);

  pinMode(pinBotonIzq,INPUT);
  pinMode(pinBotonDer,INPUT);
  pinMode(pinBotonGo,INPUT);
  //SETUP DEBUGGING
  Serial.begin(9600);
  Serial.print("Setup");
  //SETUP NEMA
  pinMode(stepPin,OUTPUT);
  pinMode(dirPin,OUTPUT);
  pinMode(enablePin,OUTPUT);
  digitalWrite(dirPin,LOW);
  //SETUP SERVO
  myservo.write(30);
  myservo.attach(servoPin);
  delay(1000);

}
void startLeds(){
  digitalWrite(pinLed1,HIGH);
  digitalWrite(pinLed2,LOW);
  digitalWrite(pinLed3,LOW);
}
void manualCalibration(){
  digitalWrite(enablePin,LOW);
  lecturaBotonGo=LOW;
  while(lecturaBotonGo==LOW){
    lecturaBotonIzq=digitalRead(pinBotonIzq);
    lecturaBotonDer=digitalRead(pinBotonDer);
    if(lecturaBotonIzq==HIGH){
      digitalWrite(dirPin,HIGH);
      digitalWrite(stepPin,HIGH);
      delayMicroseconds(20000);
      digitalWrite(stepPin,LOW);
      delayMicroseconds(20000);
    }else if (lecturaBotonDer==HIGH){
      digitalWrite(dirPin,LOW);
      digitalWrite(stepPin,HIGH);
      delayMicroseconds(20000);
      digitalWrite(stepPin,LOW);
      delayMicroseconds(20000);
    } 
    lecturaBotonGo=digitalRead(pinBotonGo);
  }  
  digitalWrite(pinLed2,HIGH);
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
    float distance = (float)rxTime * 34 / 2000.0; //convert the time to distance
    return distance;
}

void setSensors(){
  digitalWrite(enablePin,HIGH);
  d0=0;
  d1=0;
  while(d0 < 50){
    d0 = readDistance(SIG0);
    delay(50);
    Serial.println("----d0");
    Serial.println(d0);
  }
  delayMicroseconds(49000);//42000
  while(d1 < 50){
    d1 = readDistance(SIG1);
    delay(50);
    Serial.println("----d1");
    Serial.println(d1);
  }
  digitalWrite(pinLed3,HIGH);
}

void detect(){
  do{
    delayMicroseconds(42000);//42000
    p0 = readDistance(SIG0);
    delayMicroseconds(42000);//42000
    p1 = readDistance(SIG1);
   
    if (p0<20){
      p0=d0;       
    }
    if(p1<20){
        p1=d1;
    }
    Serial.println("----PO-P1");
    Serial.println(p0);
    Serial.println(p1);
    Serial.println("----");
  }while(abs(d0-p0) < 20 && abs(d1-p1) < 20);  //abs(d0-p0) < 40 && abs(d1-p1) < 40
  startTime=millis();
  if (d0-p0 > 20){
    westward = true;
    do{
      p1 = readDistance(SIG1);
      Serial.println("P1 OTRO");
      Serial.println(p1);    
    }while(d1-p1 < 20 || p1==0);
    endTime=millis();
  }
  else{
    westward = false;
    do{
      p0 = readDistance(SIG0);
      Serial.println("P0 OTRO");
      Serial.println(p0);    
    }while(d0-p0 < 20 || p0==0);
    endTime=millis();
  }
  Serial.println(startTime);
  Serial.println(endTime);  
  Serial.println((double)(endTime-startTime)/1000.0);
  elapsedTime = (double)((endTime - startTime)/1000.0);
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
  digitalWrite(enablePin,LOW);
  double angMover=angMotor-angDestino;
  Serial.print(angMover);
  cantPasos=(abs(angMover)/radPorPaso)+1; //comprobado funciona bien
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
    for (pos = 30; pos <= 80; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 5ms for the servo to reach the position
  }
  myservo.write(30);
  delay(1000);//da tiempo para disparar
}
void devolverAngulo(){
  if(digitalRead(dirPin)==HIGH) digitalWrite(dirPin,LOW);
  else digitalWrite(dirPin,HIGH);
  for(int i=0;i<(cantPasos);i++){
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(2000);
  }
}

void loop() {
  setup();//fix, sin esto no funciona desde el primer reinicio
  startLeds();
  manualCalibration();
  setSensors();
  detect();
  calculations();
  moverAngulo();
  disparo();
  devolverAngulo();
  
  lecturaBotonGo=LOW;
  while (lecturaBotonGo==LOW){  
    lecturaBotonGo=digitalRead(pinBotonGo);
  }
}
