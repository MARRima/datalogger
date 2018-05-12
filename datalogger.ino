#include <Sensirion.h>
#include "LowPower.h"
#include <SD.h>
#include <RTClib.h> // Incluye la libreria RTClib

const int CS_SD = 7;                 //Pin CS de la SD

const uint8_t dataPin  =  2;
const uint8_t clockPin =  3;

float temperature;
float humidity;
float dewpoint;
int tiempo[6];
char fecha[18];
bool garfagnana= true;
int nHour,nMin,nSec = 1;
int nDay,nMonth,nYear = 1;
String file,logN,ruta = "";
int opcion=0;

RTC_DS1307 RTC; // Crea el objeto RTC
DateTime now;//Obtiene la fecha y la hora del RTC
Sensirion tempSensor = Sensirion(dataPin, clockPin);
File f;

void setup()
{
  pinMode(6, INPUT);
  Serial.begin(9600);
  RTC.begin(); // Establece la velocidad de datos del RTC
//  RTC.adjust(DateTime(__DATE__, __TIME__));
  Serial.print(F("Iniciando SD ..."));
   if (!SD.begin(7))
  {
    Serial.println(F("Error al iniciar"));
    return;
  }
}

void loop()
{
if (digitalRead(6) == HIGH) {
  obtenerHora();
  generarRuta(); 
  guardarArchivo();
  delay(200);
  pausa();  
  } else {
    menu();
  }
}

void test(){
  
  now = RTC.now();// obtiene la fecha y la hora del RTC
  tempSensor.measure(&temperature, &humidity, &dewpoint);
//Imprimimos la fecha y lahora
  Serial.print(now.day());
  Serial.print(+ "/") ;
  Serial.print(now.month());
  Serial.print(+ "/") ;
  Serial.print(now.year()); 
  Serial.print( " ") ;
  Serial.print(now.hour());  
  Serial.print(+ ":") ;
  Serial.print(now.minute());
  Serial.print(":") ;
  Serial.print(now.second());
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" C, Humidity: ");
  Serial.println(humidity);

}
//actualiza fecha
void checkin()
{
    Serial.println("cadena aceptada");
    (Serial.readString()).toCharArray(&fecha[0],18) ;
    Serial.println(fecha);
    for(int i=0;i<5;i++)
    {
      tiempo[i]=10*(fecha[3*i]-'0')+fecha[3*i+1]-'0';
      Serial.println(tiempo[i]) ;
    }
/*
 * va esperar el siguiente formato xx-xx-xx-xx-xx, quiere decir Anno-mes-dia-hora-min dos cifras!!
*/
    RTC.adjust(DateTime(tiempo[0], tiempo[1],tiempo[2], tiempo[3], tiempo[4], 0));

}

void menu()
{
  switch(opcion)
  {
    
//Actualizar fecha"
    case 1:
    delay(200);
    if (Serial.available() > 0)
    {
      checkin();
      garfagnana=true;
      opcion=0;
      break;
    }else {opcion=1;
     if (garfagnana==false)Serial.println("formato xx-xx-xx-xx-xx, quiere decir Anno-mes-dia-hora-min dos cifras!!");
    garfagnana=true;
    break;}

//2-descargar datos
    
    case 2:
    leerymostrar();
    opcion=0;
    garfagnana=true;
    break;

//3-test
    
    case 3:
     obtenerHora();
     generarRuta();
     Serial.println();
     Serial.println(logN);
     Serial.println();
     opcion=0;
     garfagnana=true;
    break; 

//4-FormatearSD

    case 4:
        opcion=0;
        borrar();
        garfagnana=true;
         break; 
                 
//5-acabar"
       
    case 5:
      opcion=0;
    break; 
    
    //muestra todo el menu por defecto 
    default:
    if(garfagnana)
    {
      garfagnana=false;
      ordenes();
      
    }
    
    if(Serial.available()>0){
    opcion=Serial.parseInt();
    //Serial.print("la opcion es = ");
    //Serial.println(opcion);
    }
    break;
  }
}
void ordenes(){
  Serial.println("opciones disponibles");
  Serial.println("1-Actualizar fecha");
  Serial.println("2-descargar datos");
  Serial.println("3-test");
  Serial.println("4-FormatearSD");
  Serial.println("5-acabar");
  
}
/*
 * 
 * 
 * Funcion LEER LECTURAS
 * 
 */
 
void leerymostrar()
{
  File f;
  now = RTC.now();// obtiene la fecha y la hora del RTC
  for( int j=0;j<=30;j++){
  for(int i=0;i<=11;i++)
    {
      ruta = String(j+1)+ String(i+1)+String(now.year())+".txt";// fecha+extension
      if(f=SD.open(ruta))
      {
        while(f.available())Serial.write(f.read());
      }
    }
  }
    Serial.println("fin");
}

 /*********************************************
* Funcion acceder al RTC, obtenemos la fecha *
* a la que se producira el evento            *
*********************************************/

void obtenerHora(){
    //Serial.println("obteniendo hora");
    now = RTC.now();// obtiene la fecha y la hora del RTC
    tempSensor.measure(&temperature, &humidity, &dewpoint);
    nHour = now.hour();
    nMin = now.minute();
    nSec = now.second();
    nDay = now.day();
    nMonth = now.month();
    nYear = now.year();
  }
/***************************************************
*FUNCION RUTA: Creamos la ruta con formato:         *
*       file   -->/ano.mes.dia.txt                  *
*       LogN   -->ano-mes-dia-,hora:minutos:segundos*
*       ruta   -->/ano.mes/ano.mes.dia.txt          *
 ***************************************************/
void generarRuta(){
  //Serial.println("generando ruta");      
  ruta =  String(nDay)+ String(nMonth)+String(nYear)+".txt";// fecha+extension
  logN =  String(nHour) + ":" + String(nMin) + ":"+ String(nSec) +","+ String(temperature)+","+String(humidity);
  }
 /**********************************************
 * Guarda el archivo de los datos recuperados  * 
 * del reloj, creando una carpeta, y dentro de *
 * esa carpeta aloja el archivo de ese dia de  * 
 * los eventos sucedidos, un dia una hoja.     *
 **********************************************/
 
 void guardarArchivo(){
  File f;      
      Serial.println(ruta);
      Serial.println(logN);
       
    if( SD.exists(ruta))
  { 
     f = SD.open(ruta, FILE_WRITE);
       Serial.println("escrito ");              
       f.println(logN);
       f.close(); //cerramos el archivo 
       Serial.println("continuado cerrado");        
      }else{
         f = SD.open(ruta, FILE_WRITE); 
           Serial.println("escrito nuevo"); 
           f.println("hora,temperatura(C),humedad(%)"); 
           f.println(logN);
           f.close();
           Serial.println(" nuevo cerrado"); 
      }
  }
void pausa(){
   for(int i=0;i<8;i++)LowPower.powerDown (SLEEP_8S, ADC_OFF,BOD_OFF); 
}

//Funcion Borra SD
void borrar()
{
  File f;
  now = RTC.now();// obtiene la fecha y la hora del RTC
  for( int j=0;j<=30;j++)
 {
  for(int i=0;i<=11;i++)
    {
       ruta = String(j+1)+ String(i+1)+String(now.year())+".txt";// fecha+extension
      if(SD.exists(ruta)) SD.remove(ruta);       
     }
  }
  }




