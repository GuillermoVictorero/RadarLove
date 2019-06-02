//preba

//MOTOR va aquí
const int stepsPerRev=200;
const int stepsPerRevTrig=32;
double boundariesPerAngle[stepsPerRev/2];
//DRIVER MOTOR va aqui
const int dirPin=2;
const int stepPin=3;
//SENSOR MOVIMIENTO va aqui
#include <Wire.h>
#define SIG0 A0
#define SIG1 A1
unsigned long rxTime;
//POSICIONES CARTESIANAS
#include <math.h>
#include <Stepper.h>
//const float stepsPerRev=32;
Stepper trigStepper (stepsPerRevTrig,9,11,10,12); 
double pos1x=0;
double pos1y=0;
bool pos1=false;
double pos2x=0;
double pos2y=0;
double d0=0;
double d1=0;
double p0;
double p1;
bool eastward;
double horDist = 15;
bool pos2=false;
unsigned long startTime;
unsigned long endTime;
double tiempoRequerido=1;//en segundos
double anguloDisparo; //en radianes
double anguloCanon=PI/2;
//retraso
unsigned int delayInt=20; 
//AYUDA CALCULOS
double stepAngleRad=1.8*2*PI;

float readDistance(int SIG){
  pinMode(SIG, OUTPUT);
    //Genarate a pulse 20uS pulse
    digitalWrite(SIG, HIGH);
    delayMicroseconds(20);
    digitalWrite(SIG, LOW);
    //set SIG as INPUT,start to read value from the module
    pinMode(SIG, INPUT);
    rxTime = pulseIn(SIG, HIGH);//waits for the pin SIG to go HIGH, starts timing, then waits for the pin to go LOW and stops timing
    double distance = (double)rxTime * 34 / 2000.0; //convert the time to distance
    return distance;
}
void setup() {
  Serial.begin(115200);
  trigStepper.setSpeed(900);
  pinMode(stepPin,OUTPUT);
  pinMode(dirPin,OUTPUT);
  setSensors();
}
void registerDistance(double distance,double anguloRad){
    double posx=distance*cos(anguloRad);
    double posy=distance*sin(anguloRad);
    if(!pos1){
      Serial.print("Registrando posicion 1");
      pos1x=posx;
      pos1y=posy;
      pos1=true;
      startTime=millis();
    }
    else{
      Serial.print("Registrando posicion 2");
      pos2x=posx;
      pos2y=posy;
      pos2=true;
      endTime=millis();
    }
  }    
void setSensors(){
  /*while(d0<20){
    d0 = readDistance(SIG0);
    delay(500);  
  }*/
  delay(500);
  while(d1<20){
    d1 = readDistance(SIG1);
    delay(500);  
  }
  Serial.println("----");
  Serial.println(d0);
  Serial.println(d1);
  Serial.println("----");
}

void detect(){
  do{
    delayMicroseconds(500);
    //p0 = readDistance(SIG0);
    delayMicroseconds(500);
    p1 = readDistance(SIG1);
   
    if(p0<6){
      p0=d0;       
    }
    if(p1<6){
      p1=d1;
    }
    Serial.println("----PO-P1");
    Serial.println(p0);
    Serial.println(p1);
    Serial.println("----");
  }while(d0-p0<20 && d1-p1<20);//d0-p0 > 20 || d1-p1 > 20
  delayMicroseconds(500);
  startTime=millis();
  if (d0-p0 > 20){
    eastward = true;
    do{
      delayMicroseconds(500);
      p1 = readDistance(SIG1);    
    }while(d1-p1 < 20);
    endTime=millis();
  }
  else{
    eastward = false;
    do{
      delayMicroseconds(500);
      p0 = readDistance(SIG0);    
    }while(d0-p0 < 20);
    endTime=millis();
  }  
}

void calculations(){
  double desplazamientoX=horDist;
  double desplazamientoY=p1-p0;
  pos2x = horDist/2;
  pos2y = p1;
  if(!eastward){
    desplazamientoX= -desplazamientoX;
    desplazamientoY= -desplazamientoY; 
    pos2x = -pos2x;
    pos2y = p0;
  }
  double elapsedTime=(double)(endTime-startTime)/1000;//tiempo en segundos
  double velocidadX=desplazamientoX/elapsedTime;
  double velocidadY=desplazamientoY/elapsedTime;
  //asumiendo ahora posicionInicial es la pos2
  double posFinalX=pos2x+velocidadX*tiempoRequerido;
  double posFinalY=pos2y+velocidadY*tiempoRequerido;
  anguloDisparo=atan(posFinalY/posFinalX);
}
void moveCanon(){
   double diferenciaPosicion=anguloCanon-anguloDisparo;
   if(diferenciaPosicion<0){
    digitalWrite(dirPin,HIGH);
      for(int i=0;i<diferenciaPosicion/stepsPerRev;i++){
      digitalWrite(stepPin,HIGH); 
      delayMicroseconds(1000); 
      digitalWrite(stepPin,LOW); 
      delayMicroseconds(1000);    
      }
   }
   else{
    digitalWrite(dirPin,LOW);
    for(int i=0;i<diferenciaPosicion/stepsPerRev;i++){
      digitalWrite(stepPin,HIGH);
      delayMicroseconds(1000);
      digitalWrite(stepPin,LOW);
      delayMicroseconds(1000);
    }
   }
   trigStepper.step(960);    
   trigStepper.step(-960);
   while(true){
    Serial.println("Termino");  
   }
}
void loop(){
  detect();
  calculations();
  moveCanon();
}
