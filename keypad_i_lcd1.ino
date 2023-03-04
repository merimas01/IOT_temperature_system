#include "Keypad.h"
#include "stdio.h"
#include "string.h"
#include <LiquidCrystal.h>

const byte rows=4;
const byte cols=4;

char hexkeypad[rows][cols]={
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'},
};

byte rowspins[rows]={2,3,4,5};
byte colspins[cols]={6,7,8,9};

Keypad kpd=Keypad(makeKeymap(hexkeypad),rowspins,colspins,rows,cols);

Keypad customKeypad( makeKeymap(hexkeypad), rowspins, colspins, rows, cols);

byte i=0;

LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

char unos[10];  //unos temp
int brojac_unos=0;

char unos_tajmer[10];
int brojac_tajmer=0;

int sekunde=0; //odbrojavanje od trenutka paljenja pecnice
int minute=0;

bool postavljenaTemp=0;
bool postavljenTajmer=0;
bool ponovoPostavi=0;
bool pokreni=0;
bool promijeniTemp=0;
bool ugasenaPecnica=1;


char *pNext_tajmer;
long output_tajmer;

char *pNext_temp;
long output_temp;



int sensorPin=0; //za temperature sensor

//POMOC:
//temperatura se uvijek setuje, a tajmer ne mora
//dugme A- za setovanje tajmera
//dugme D- kada zavrsimo sa setovanjem temperature ili tajmera (done)
//dumge C- gasimo pecnicu
//dugme #- ponovo setujemo sve
//dugme *- obrisi unos
//tajmer se samo jednom postavlja
//ako tajmer nije setovan a kliknut je B, onda je nemoguce 
//ponovo setovati tajmer. Tajmer po potrebi se setuje samo 1
//i to prije nego sto se klikne B (start)
//validacija: temperatura mora biti veca od 100° a manja od 250°
//kada pisemo, nema opcije za brisanje (sva slova su zauzeta xD)


void setup()
{
 // Serial.begin(9600);
  lcd.begin(16,2);  //setujemo dimenzije lcda, 16 kolona i 2 reda
  pinMode(13,OUTPUT);
  
  for(i=0; i<8; i++)
    pinMode(i,OUTPUT);
}

void loop()
{
  char k=customKeypad.getKey();
  
  if(k!='C'){
    petlja();
  }
  else{
    ugasiPecnicu();
  }
  
 
} 
  

void ugasiPecnicu(){
    pokreni=0;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Pecnica ugasena.");
    postavljenaTemp=0;
    postavljenTajmer=0;
    output_tajmer=0;
    brojac_unos=0;
    brojac_tajmer=0;
    digitalWrite(13,LOW); //lampica se gasi  
        
    sekunde=0;
    ugasenaPecnica=1;
   
  
    while(ponovoPostavi==0){ 
      char key=kpd.getKey();
      if(key=='#')
         ponovoPostavi=1;
    }

   lcd.clear();
}



void postaviTemperaturu(){
  
    lcd.setCursor(0, 0);
    lcd.print("Unesi temp");
   
  
    char keypressed=kpd.getKey();
   
    if(keypressed>='0' && keypressed<='9'){
    Serial.print("Key is: ");
    Serial.println(keypressed);
    
    unos[brojac_unos]=keypressed;
    lcd.setCursor(brojac_unos, 1);
    lcd.print(unos[brojac_unos]);
    brojac_unos++;
    }
    else if(keypressed=='*') //za brisanje unosa
      {
        ObrisiUnos();
        lcd.clear();
        lcd.print("Unesi temp");
      }
    else if(keypressed=='D'){
    lcd.clear();
    delay(100);
    lcd.print("Temperatura: ");
    lcd.setCursor(0,1);
    lcd.print(output_temp);
   
    
    for(int i=0; i<brojac_unos; i++){
      lcd.setCursor(i,1);
      lcd.print(unos[i]);
    }
     
       
    //convert string u int
    output_temp = strtol (unos, &pNext_temp, 10);
      
    Serial.print("output temp: ");
    Serial.print(output_temp);
    
    //temperatura mora biti u rasponu od 100° do 250°
    if(output_temp>=100 && output_temp<=250)
       postavljenaTemp=1;
    else{
        ObrisiUnos();
        lcd.clear();
        output_temp=0;
      }
      
      pokreni=0;
  }
}


void postaviTajmer(){
    
    char key=kpd.getKey();
  
    if(key>='0' && key<='9'){
   // Serial.print("Tajmer key is: ");
   // Serial.println(key);
    
    unos_tajmer[brojac_tajmer]=key;
    lcd.setCursor(brojac_tajmer, 1);
    lcd.print(unos_tajmer[brojac_tajmer]);
    brojac_tajmer++;
    }
    else if(key=='D'){
    lcd.clear();
    delay(100);
    lcd.print("Tajmer: ");
    
    for(int i=0; i<brojac_tajmer; i++){
      lcd.setCursor(i,1);
      lcd.print(unos_tajmer[i]);
    }
      
    //convert string u int
    output_tajmer = strtol (unos_tajmer, &pNext_tajmer, 10);
    
    Serial.print("output tajmer: ");
    Serial.print(output_tajmer);
    
   
    //tajmer moze biti postavljen na max sat vremena (60 min)
    if(output_tajmer>0 && output_tajmer<=60)
       postavljenTajmer=1;
      
      
      pokreni=0; //nije jos kliknuto B
  }
  
}


void ObrisiUnos(){
  while (brojac_unos != 0)
  { 
    unos[brojac_unos--] = 0;  //--brojac_unos
  }
  return;
}


void petlja(){
  
  while(postavljenaTemp==0){
       postaviTemperaturu();  
  }
  
  ObrisiUnos();
  
  char keypressed=kpd.getKey();
 
  //promijeni temp prije paljenja pecnice
 /* if(keypressed=='*'){
    postavljenaTemp=0;
    lcd.clear();
    delay(100);
  }*/
  
  /*
  if(keypressed=='A' && postavljenTajmer==0){
       lcd.clear();
       delay(100);
       lcd.setCursor(0,0);
       lcd.print("Set tajmer: ");
       delay(100);
    
       while(postavljenTajmer==0){
          postaviTajmer();
       } 
  }
  */
  brojac_tajmer=0;
  
  if(keypressed=='B'){
    
    //lampica se pali
    digitalWrite(13,HIGH);
    
    ugasenaPecnica=0;
    
    postavljenTajmer=1; //da se onemoguci postavljanje tajmera.
    
    pokreni=1;
    //sekunde+=1;
    
    lcd.clear();
    
    lcd.setCursor(0,0);
    lcd.print("Temperatura: ");   
    lcd.setCursor(13,0);
    lcd.print(output_temp);
  /*   
    lcd.setCursor(0,1);
    lcd.print("Tajmer(min): ");    
    lcd.setCursor(13,1);
    lcd.print(output_tajmer);
  */
    
  }
  
    if(keypressed=='C')
    {
       lcd.clear();
       ugasiPecnicu();
    }
   
  
  //ako nije ukljucen tajmer a pecnica je upaljena duze od 1h,
  //pecnica se automatski gasi
  if(pokreni==1 && output_tajmer==0){
    if(keypressed=='C')
      ugasiPecnicu();
   /* else{
       delay(1000);
       sekunde++;
    }
    if(sekunde>=50){
      ugasiPecnicu();
    }*/
      
  }
  
  //ako je setovan tajmer, odbrojava se na lcd
  //ne mozemo rucno ugasiti kad je tajmer postavljen
  
  if(output_tajmer>0 && pokreni==1)
  {
    //sekunde++; delay(1000);
    //Serial.print("sekunde");
    //Serial.println(sekunde);
    
    int sek=output_tajmer*60;
    timer(sek);
    
    /*
    if(sekunde==60)
    {
      output_tajmer--;
      
      lcd.setCursor(13,1);
      lcd.print(output_tajmer);
      sekunde=0;
     
      if(output_tajmer==0){
      ugasiPecnicu();
      }               
    } 
    */
    
  }
}

void timer(int &sek)
{
delay(1000);
  
  char key=kpd.getKey();
  if(key=='C'){
    ugasiPecnicu();
  }
  
sek--;
if(sek>0)
{
timer(sek);
//Serial.print(sek);
//lcd.setCursor(13,1);
//lcd.print(sek);
}
else
{
ugasiPecnicu();
}
}