//preba

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
    rxTime = pulseIn(SIG, HIGH,5000);//waits for the pin SIG to go HIGH, starts timing, then waits for the pin to go LOW and stops timing
    double distance = (double)rxTime * 34 / 2000.0; //convert the time to distance
    return distance;
}
void setup() {
  Serial.begin(9600);
  pinMode(stepPin,OUTPUT);
  pinMode(dirPin,OUTPUT);
  setSensors();
}    
void setSensors(){
  while(d0<20){
    d0 = readDistance(SIG0);  
  }
  while(d1<20){
    d1 = readDistance(SIG1);  
  }
  Serial.println("----");
  Serial.println(d0);
  Serial.println(d1);
  Serial.println("----");
  
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
    delayMicroseconds(20);
    //p0 = readDistance(SIG0);
    delayMicroseconds(20);
    p1 = readDistance(SIG1);
   
    if (p0<3){
      p0=d0;       
    }
    else{
      if(p1<3){
        p1=d1;
      }
    }
    Serial.println("----PO-P1");
    Serial.println(p0);
    Serial.println(p1);
    Serial.println("----");
  }while(true);//d0-p0 > 20 || d1-p1 > 20
  startTime=millis();
  if (d0-p0 > 20){
    eastward = true;
    do{
      p1 = readDistance(SIG1);    
    }while(d1-p1 > 20);
    endTime=millis();
  }
  else{
    eastward = false;
    do{
      p0 = readDistance(SIG0);    
    }while(d0-p0 > 20);
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
   while(true){
    Serial.println("Termino");  
   }
}
void loop(){
  detect();
  calculations();
  moveCanon();
}
