const double horDist=15;
double pos1x=horDist/2;
double pos2x=-horDist/2;
double pos1y;
double pos2y;
double p0=3;//valores aleatorios por ahora
double p1=5;//valores aleatorios por ahora
double elapsedTime=0.15;
bool eastward=true;//valores aleatorios por ahora


double angDestino;//conexion a movimiento motor

void setup() {
  Serial.begin(9600);
  
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
    if(pos1y>pos2y) angDestino =PI/2+(atan(posFinalY/posFinalX));
    else angDestino=PI/2+-(atan(posFinalY/posFinalX));      
    Serial.println(angDestino);
  }
  else{
    double desplX=horDist;
    double desplY=pos1y-pos2y;
    double velocidadX=desplX/elapsedTime;
    double velocidadY=desplY/elapsedTime;
    double posFinalX=pos1x+velocidadX*2;
    double posFinalY=pos1y+velocidadY*2;
    if(pos1y>pos2y) angDestino=atan(posFinalY/posFinalX);
    else angDestino=-(atan(posFinalY/posFinalX));
    Serial.println(angDestino);
  }
}
void loop() {
  // put your main code here, to run repeatedly:
  calculations();
  while (true){
    Serial.println("Termino");
    delay(10000);
  }
}
