#define LED 1
#define pumpdis 2
#define pumpsuc 3
#define LDR 15
#define waterlvl 12

const int LVLH = 0; //to log theoratical min and max values
const int LVLL = 0;
const int LVLI = 0; //ideal

const int LDRL = 0;

void setup() {
  
  Serial.begin(115200);
  Serial.println("Initialising Serial");
  
  pinMode(LED,OUTPUT);
  pinMode(pumpdis,OUTPUT);
  pinMode(pumpsuc,OUTPUT);
  pinMode(LDR,INPUT);
  pinMode(waterlvl,INPUT);
  
  //telesetup();//initalise telegram app setup
}

void refillwater(){
  while(analogRead(waterlvl)<LVLI){
    digitalWrite(pumpdis,100);
    delay(2000);
  }
}

int checktime(){
  //setupwifi();
  return 1800;
}

void led(){
  if(checktime()>1800 or analogRead(LDR)<LDRL){
    digitalWrite(LED,HIGH);
  }else{
    digitalWrite(LED,LOW);
  }
}

void checkanalogvalue(int analogpin){
  Serial.println(analogRead(analogpin));
  delay(500);
}

void loop(){
  Serial.println("Running Main Loop");
  checkanalogvalue(LDR);
  //refillwater();

  //led();
  
  //delay(10*1000);
}
