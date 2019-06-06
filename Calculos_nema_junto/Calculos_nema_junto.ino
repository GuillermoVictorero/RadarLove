
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


void setup() {
  Serial.begin(9600);
  //SETUP NEMA
  pinMode(stepPin,OUTPUT);
  pinMode(dirPin,OUTPUT);
  digitalWrite(dirPin,LOW);

}
void calculations(){
  pos1y=p0;
  pos2y=p1;
  if(eastward){
    double desplX=-horDist;
    double desplY=pos2y-pos1y;
    double velocidadX=desplX/elapsedTime;
    double velocidadY=desplY/elapsedTime;
    double posFinalX=pos2x+velocidadX*2;
    double posFinalY=pos2y+velocidadY*2;//ojo con el tiempo requerido
    angDestino=PI-abs(atan((posFinalY/posFinalX)));      
    Serial.println(angDestino);
  }
  else{
    double desplX=horDist;
    double desplY=pos1y-pos2y;
    double velocidadX=desplX/elapsedTime;
    double velocidadY=desplY/elapsedTime;
    double posFinalX=pos1x+velocidadX*2;
    double posFinalY=pos1y+velocidadY*2;
    angDestino=abs(atan(posFinalY/posFinalX));
    Serial.println(angDestino);
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

void loop() {
  calculations();
  moverAngulo();
  disparo();
    while (true){
    Serial.println("Termino");
    delay(10000);
  }
}
