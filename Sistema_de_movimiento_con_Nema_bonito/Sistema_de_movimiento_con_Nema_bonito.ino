//MOTOR va aquí
const int stepsPerRev=200;
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
double pos1x=0;
double pos1y=0;
bool pos1=false;
double pos2x=0;
double pos2y=0;
double d0;
double d1;
double p0;
double p1;
bool eastward;
double horDist = 15;
bool pos2=false;
unsigned long startTime;
unsigned long endTime;
double tiempoRequerido=1;//en segundos
double anguloDisparo; //en radianes
//AYUDA CALCULOS
double stepAngleRad=1.8*2*PI;
//DEV
unsigned long tiempoLectura;

float readDistance(SIG){
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
void setBoundaries(){
  int indice=0;
  for(int x=0; x<stepsPerRev/2;x++){
    boundariesPerAngle[indice++]=readDistance();
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(1000);
  }
}
void setup() {
  Serial.begin(9600);
  pinMode(stepPin,OUTPUT);
  pinMode(dirPin,OUTPUT);
  unsigned long lecturaBegin=millis();
  setBoundaries();
  unsigned long lecturaEnd=millis();
  tiempoLectura=lecturaEnd-lecturaBegin;
  Serial.print(tiempoLectura);
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
  d0 = readDistance(SIG0);
  d1 = readDistance(SIG1);
  
  
  /*while(!pos2){
    double distance;
    Serial.print("Pa un lado");
    // Set motor direction clockwise
    digitalWrite(dirPin,HIGH); 
    
    // Spin motor half a rotation slowly
    int indice=0;
    int anguloRad=0;
    for(int x = 0; x <(stepsPerRev/2); x++) {
      distance=readDistance();
      //if(abs(boundariesPerAngle[indice++]-distance)>20) registerDistance(distance,anguloRad);
      digitalWrite(stepPin,HIGH); 
      delayMicroseconds(1000); 
      digitalWrite(stepPin,LOW); 
      delayMicroseconds(1000);
      anguloRad+=stepAngleRad; 
    }
    Serial.print(indice);
    Serial.print("Pa'l otro");
    digitalWrite(dirPin,LOW);
    for(int x=0; x<(stepsPerRev/2);x++){
      distance=readDistance();
      //if (abs(boundariesPerAngle[indice--]-distance)>20) registerDistance(distance,anguloRad);
      digitalWrite(stepPin,HIGH);
      delayMicroseconds(1000);
      digitalWrite(stepPin,LOW);
      delayMicroseconds(1000);
      anguloRad-=stepAngleRad;
    }
  }*/
}

void detect(){
  do{
    d0 = readDistance(SIG0);
    d1 = readDistance(SIG1);    
  }while(d0-p0 > 10 || d1-p1 > 10)
  startTime=millis();
  if (d0-p0 > 10){
    eastward = true;
    do{
      d1 = readDistance(SIG1);    
    }while(d1-p1 > 10)
    endTime=millis();
  }
  else{
    eastward = false;
    do{
      d0 = readDistance(SIG0);    
    }while(d0-p0 > 10)
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
   Serial.println("TERMINO");
}
void loop(){
  setSensors();
  detect();
  calculations();
  moveCanon();
}
