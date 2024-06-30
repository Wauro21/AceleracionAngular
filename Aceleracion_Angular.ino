///Importar libreria Talkie//////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include "Talkie.h"
#include "Vocab_US_Large.h"
#include "Vocab_Special.h"
Talkie voice;
void sayNumber(long n);
/// Variables //////////////////////////////////////////////////////////////////////////////////////////////////////////////
int encoder_pin = 2;                        //Pin 2, donde se conecta el encoder       
volatile byte pulses = 0;                   // Número de pulsos leidos por el Arduino en un segundo
unsigned long timeold = 0;                  // Tiempo 
static volatile unsigned long debounce = 0; // Tiempo del rebote.

int j=0;                                    // Inicia contador de datos guardados
int delta_t=100;                            // Diferencia de tiempo entre datos guardados
int datos=20;                               // Cantidad de datos guardados
float cuenta[20];                           //Inicia lista que guarda los pulsos  CHEQUEAR n=(datos)
float tiempo[20];                           //Inicia lista que cuarda los tiempos CHEQUEAR n=(datos)
float tiempo_v[19];                          //Inicia lista que contendra tiempos medios para la velocidad CHEQUEAR n=(datos-1)
float velocidad[19];                         //Inicia lista que contendra datos de velocidad   CHEQUEAR n=(datos-1)
float aceleracion[18];                       //Inicia lista que contendrá datos de aceleracion CHEQUEAR n=(datos-2)

float aceleracion_media=0;                  //Inicia Aceleracion Media
float suma = 0;                             //Inicia Suma promedio
int i=0;                                    //Inicia contador de datos

float aceleracion_lineal = 0;
float aceleracion_angular = 0;

int botonactivado=0;                        //Estado inicial del boton 
int boton=4;                                // Pin digital donde va el boton

unsigned int pulsesperturn = 100; // Número de muescas que tiene el disco del encoder.
const int wheel_diameter = 48.6;   // Diametro de polea[mm]




////  Configuración del Arduino /////////////////////////////////////////////////////////
void setup(){
Serial.begin(9600);                        // Configuración del puerto serie  
pinMode(encoder_pin, INPUT);               // Configuración del pin nº2
pinMode(boton,INPUT);                      // se define al boton como entrada digital
attachInterrupt(0, counter, RISING);       // Configuración de la interrupción 0, donde esta conectado. 
Serial.print("j ");                        //Imprime en serial j
Serial.print("Pulses ");                   // imprime en serial pulsos
Serial.println("millis");                //imprime en serial millis
voice.say(sp2_READY);
voice.say(sp4_TO);
voice.say(sp2_MEASURE);

}                   //Ready

////  Programa principal ///////////////////////////////////////////////////////////////////////
 void loop(){
   botonactivado = digitalRead(boton);
   if (millis() - timeold >= delta_t && j < datos && pulses > 2 ){  // Se actualiza cada segundo
      noInterrupts(); //Don't process interrupts during calculations // Desconectamos la interrupción para que no actué en esta parte del programa.
      timeold = millis(); // Almacenamos el tiempo actual.
      cuenta[j]=pulses;
      tiempo[j]=timeold/1000.0;
      Serial.print(j);
      Serial.print("       ");
      Serial.print(cuenta[j]);
      Serial.print("       ");
      Serial.println(tiempo[j]);
      interrupts(); // Restart the interrupt processing // Reiniciamos la interrupción
      j=j+1;
   }
   else if (j==datos ){
   Serial.println("Lista la captura de datos");
   j=j+1;
  //Serial.println("Velocidades : ");
  for (i = 0; i < (sizeof(cuenta)/sizeof(cuenta[0]))-1; i = i + 1) {
  velocidad[i]=(cuenta[i+1]-cuenta[i])/(tiempo[i+1]-tiempo[i]);
  //Serial.println(velocidad[i]);
  }

  for (i = 0; i < (sizeof(tiempo)/sizeof(tiempo[0]))-1; i = i + 1){
  tiempo_v[i]=((tiempo[i]+tiempo[i+1])/2);
  //Serial.print(tiempo_v[i]);
  }
  //Serial.println("aceleraciones : ");
  for (i = 0; i < (sizeof(velocidad)/sizeof(velocidad[0]))-1; i = i + 1){
  aceleracion[i]=(velocidad[i+1]-velocidad[i])/(tiempo_v[i+1]-tiempo_v[i]);
  //Serial.println(aceleracion[i]);
  }
  suma=0;
  for (i = 0; i < sizeof(aceleracion)/sizeof(aceleracion[0]); i= i + 1 ){
  suma=suma+aceleracion[i];
  //Serial.println(suma);
  }
  aceleracion_media= (suma/(sizeof(aceleracion)/sizeof(aceleracion[0])));
  
  Serial.println("Aceleración media Calculada");
  voice.say(sp4_MEASURED);//Calculated
   
   }
   else if (botonactivado==1){
   Serial.print("ACELERACION [PULSOS/SEGUNDO^2] : ");
   Serial.println(aceleracion_media);
   
   Serial.print("ACELERACION ANGULAR [RADIANES/SEGUNDO^2 : ");
   aceleracion_angular=aceleracion_media*((2*3.1416)/pulsesperturn);
   Serial.println(aceleracion_angular);

   aceleracion_angular=aceleracion_media*((2*3.1416*1000)/pulsesperturn);
   Serial.println(aceleracion_angular);
   sayNumber(aceleracion_angular);
   //voice.say(sp2_MILLI);
   delay(500);
   }
   }
////Fin de programa principal //////////////////////////////////////////////////////////////////////////////////
///////////////////////////Función que cuenta los pulsos buenos ///////////////////////////////////////////
 void counter(){
  if(  digitalRead (encoder_pin) && (micros()-debounce > 500) && digitalRead (encoder_pin) ) { 
// Vuelve a comprobar que el encoder envia una señal buena y luego comprueba que el tiempo es superior a 1000 microsegundos y vuelve a comprobar que la señal es correcta.
        debounce = micros(); // Almacena el tiempo para comprobar que no contamos el rebote que hay en la señal.
        pulses++;}  // Suma el pulso bueno que entra.
        else ; } 

/* Say any number between -999,999 and 999,999 */
void sayNumber(long n) {
    if (n < 0) {
        voice.say(sp2_MINUS);
        sayNumber(-n);
    } else if (n == 0) {
        voice.say(sp2_ZERO);
    } else {
        if (n >= 1000) {
            int thousands = n / 1000;
            sayNumber(thousands);
            voice.say(sp2_THOUSAND);
            n %= 1000;
            if ((n > 0) && (n < 100))
                voice.say(sp2_AND);
        }
        if (n >= 100) {
            int hundreds = n / 100;
            sayNumber(hundreds);
            voice.say(sp2_HUNDRED);
            n %= 100;
            if (n > 0)
                voice.say(sp2_AND);
        }
        if (n > 19) {
            int tens = n / 10;
            switch (tens) {
            case 2:
                voice.say(sp2_TWENTY);
                break;
            case 3:
                voice.say(sp2_THIR_);
                voice.say(sp2_T);
                break;
            case 4:
                voice.say(sp2_FOUR);
                voice.say(sp2_T);
                break;
            case 5:
                voice.say(sp2_FIF_);
                voice.say(sp2_T);
                break;
            case 6:
                voice.say(sp2_SIX);
                voice.say(sp2_T);
                break;
            case 7:
                voice.say(sp2_SEVEN);
                voice.say(sp2_T);
                break;
            case 8:
                voice.say(sp2_EIGHT);
                voice.say(sp2_T);
                break;
            case 9:
                voice.say(sp2_NINE);
                voice.say(sp2_T);
                break;
            }
            n %= 10;
        }
        switch (n) {
        case 1:
            voice.say(sp2_ONE);
            break;
        case 2:
            voice.say(sp2_TWO);
            break;
        case 3:
            voice.say(sp2_THREE);
            break;
        case 4:
            voice.say(sp2_FOUR);
            break;
        case 5:
            voice.say(sp2_FIVE);
            break;
        case 6:
            voice.say(sp2_SIX);
            break;
        case 7:
            voice.say(sp2_SEVEN);
            break;
        case 8:
            voice.say(sp2_EIGHT);
            break;
        case 9:
            voice.say(sp2_NINE);
            break;
        case 10:
            voice.say(sp2_TEN);
            break;
        case 11:
            voice.say(sp2_ELEVEN);
            break;
        case 12:
            voice.say(sp2_TWELVE);
            break;
        case 13:
            voice.say(sp2_THIR_);
            voice.say(sp2__TEEN);
            break;
        case 14:
            voice.say(sp2_FOUR);
            voice.say(sp2__TEEN);
            break;
        case 15:
            voice.say(sp2_FIF_);
            voice.say(sp2__TEEN);
            break;
        case 16:
            voice.say(sp2_SIX);
            voice.say(sp2__TEEN);
            break;
        case 17:
            voice.say(sp2_SEVEN);
            voice.say(sp2__TEEN);
            break;
        case 18:
            voice.say(sp2_EIGHT);
            voice.say(sp2__TEEN);
            break;
        case 19:
            voice.say(sp2_NINE);
            voice.say(sp2__TEEN);
            break;
        }
    }
}
