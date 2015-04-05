/* Projet Plantarium (BAC STI2D SIN - 2014-2015)- CALVEZ Tony, DIDOU Antoine et LAURENT Mathilde
Nous avons un capteur d'Humidité et de Température de types DHT22; un capteur de Luminosité sur Plantine TEMT6000 et un capteur de Niveau d'Eau.

Remerciement à:
-Monsieur Fidel pour le prêt de son Livre Arduino de Simon MONK (édition Pearson)
-Lepinkainen pour le code d'affichage DHT22: https://github.com/lepinkainen/DHT_LCD/blob/master/DHT_LCD.ino (domaine public)
-Eskimon pour éviter les interruptions de la pompe dû au delay: http://eskimon.fr/76-arduino-202-introduire-le-temps (domaine public)
*/
#include <SPI.h>
#include <Ethernet.h>
#include "DHT.h" //Appelle de la Librairie du DHT. CF: https://github.com/adafruit/DHT-sensor-library
#include <LiquidCrystal.h> //Appelle de la Librairie de l'Afficheur LCD
#define DHTPIN 22 //Entrée/Sortie ou est connecté le DHT22, ici, nous sommes sur le Digital 1
#define DHTTYPE DHT22   // Types de DHT: ici c'est le DHT22 - (AM2302)

DHT dht(DHTPIN, DHTTYPE); //Permet de réunir les informations pour l'initialisation du DHT22

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //L'écran LCD est connecté sur les Pins suivant. CF: http://www.arduino.cc/en/Tutorial/LiquidCrystal


int nbcharacteres=16;
int nblignes=2;
int alarmeDHT22=0;

int DELRougePin=21;
int DELVertPin=20;
int DELAlertePin=19;
int capteurniveaudeau=0; //Valeur du capteur de niveau d'eau.
int tensioncapteurniveaudeau=0; //Valeur de la tension en fonction de la valeur du niveau d'eau.
int alarmeniveaudeau=0; //Alarme du niveau d'eau.

int capteurluminosite=0;
int tensioncapteurluminosite=0;
int NeonPin=50;
int MatricePin=51;

int HumiditeDemande=65;
int HumiditeOnline;
long TempsArrosageCourt; //variable qui stocke la mesure du temps
long AllumPompeCourt=300000;
boolean PompeEtatCourt;
long TempsArrosageLong; //variable qui stocke la mesure du temps
long AllumPompeLong=900000;
boolean PompeEtatLong;
int PompePin=53;

int TemperatMini=18;
int TemperatMax=24;
int TemperatMiniOnline;
int TemperatMaxOnline;
int VentilateurPin=48;
int ThermoplongueurPin=49;


byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0F, 0xE7, 0x6B }; //MAC address found on the back of your ethernet shield.
IPAddress ip(192,168,1,50); // IP address dependent upon your network addresses.

//----- tableau de l'adresse de la passerelle --- 
byte passerelle[] = { 192, 168, 1, 1 }; // l'adresse du PC de connexion

//----- tableau du masque de sous réseau 
byte masque[] = { 255, 255, 255, 0 }; // idem masque sous-réseau du PC : 255.255.255.0
// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);



void setup() {
  Serial.begin(9600);
  lcd.begin(nbcharacteres, nblignes); //Initialisation de l'écran. 16 correspond au nombre de charactères sur l'écran et 2 le nombre de lignes
  dht.begin(); //Initialisation du DHT

    pinMode(PompePin, OUTPUT);
    //on initialise le temps
    TempsArrosageCourt = (millis()-AllumPompeCourt);
    TempsArrosageLong = (millis()-AllumPompeLong);

  pinMode(DELRougePin, OUTPUT); //Initialisation de la DEL Niveau d'Eau "Alerte" sur la carte arduino, ici, nous avons la LED sur Digital 21 et c'est une sortie.
  pinMode(DELVertPin, OUTPUT); //Initialisation de la DEL Niveau d'Eau "OK" sur la carte arduino, ici, nous avons la LED sur Digital 20 et c'est une sortie.
  pinMode(DELAlertePin, OUTPUT); //Initialisation de la DEL "Erreur DHT22" sur la carte arduino, ici, nous avons la LED sur Digital 19 et c'est une sortie.
  
  pinMode(VentilateurPin, OUTPUT); //Initialisation de la sortie pour la Ventilateur
  pinMode(ThermoplongueurPin, OUTPUT); //Initialisation de la sortie pour la THERMOPLONGEUR


  pinMode(NeonPin, OUTPUT);
  pinMode(MatricePin, OUTPUT);

  //On démarre le serveur en lui fournissant une adresse MAC et une IP
  Ethernet.begin(mac, ip);
  server.begin();
 
  //On affiche l'ip du serveur de manière à voir si tout va bien
  Serial.print("Adresse du serveur obtenue sans DHCP: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
/*Programme Luminosité DIDOU Antoine*/
  AgirPourLuminosite();
/*Programme Niveau D'eau - CALVEZ Tony*/
  VerifierNiveauDeau();
/*Programme DHT22 (Capteur d'Humidité et Température)- CALVEZ Tony + LAURENT Mathilde*/  
  float h = dht.readHumidity(); // h est la variable pour l'Humidité 
  float t = dht.readTemperature(); // t est la variable pour la Température

/*Envoie Internet*/
EthernetShield();

/*Pompe sans delay*/
  if (isnan(t) || isnan(h)) { //Vérifie si il correspond bien au domaine de mesure du DHT ou qu'il n'est pas endommagé.
    AffichageErreurDHT();
  } else { //Sinon si il correspond, démarrage de l'affichage
    AffichageValeurLCD();
    if(h>=HumiditeDemande){
          if((millis() - TempsArrosageCourt) > AllumPompeCourt)
        {
            PompeEtatCourt = !PompeEtatCourt; //on inverse l'état de la LED
            digitalWrite(PompePin, PompeEtatCourt); //on allume ou éteint
            if(PompeEtatCourt==LOW){delay(600000);}
            TempsArrosageCourt = millis(); //on stocke la nouvelle heure
        }
    }else{
          if((millis() - TempsArrosageLong) > AllumPompeLong)
          {
            PompeEtatLong = !PompeEtatLong; //on inverse l'état de la LED
            digitalWrite(PompePin, PompeEtatLong); //on allume ou éteint
            TempsArrosageLong = millis(); //on stocke la nouvelle heure
          }
        }
      }
    }















  void AffichageValeurLCD(){
    float h = dht.readHumidity(); // h est la variable pour l'Humidité 
    float t = dht.readTemperature(); // t est la variable pour la Température
    lcd.setCursor(0, 0); //Définition du début d'écriture, ici charactère:0 et ligne:0
    lcd.print("Humidite :"); //Affichage du texte: "Humidité"
    lcd.print(h); //Affichage de la variable "h" pour afficher l'Humidité
    lcd.print("%"); //Affichage du "%" a la fin.
    lcd.setCursor(0, 1); //Définition du début d'écriture, ici charactère:0 et ligne:1
    lcd.print("Temperat.:"); //Affichage du texte: "Température"
    lcd.print(t); //Affichage de la variable "t" pour afficher la Température
    lcd.print("C"); //Affichage de l'Unité de température, ici Degré Celsius donc "C"

    Serial.println(h);
    Serial.println(t);
      alarmeDHT22=0; //Pour le serveur
      digitalWrite(DELAlertePin, LOW); //Sinon le domaine de mesure du DHT22 correspond, la LED ne s'allume pas.

    }

void AffichageErreurDHT(){
    lcd.print("Erreur DHT22"); //Si il ne correspond pas, il marque "Erreur"
    alarmeDHT22=1; //Pour le serveur
    digitalWrite(DELAlertePin, HIGH); //Si le domaine de mesure du DHT22 ne correspond pas ou qu'il est endommagé, la LED s'allume.
    delay(1000);
    }


/*Mathilde LAURENT*/
void VerifierTemperature(){
float t = dht.readTemperature(); // t est la variable pour la Température
        if(t>=TemperatMini){
        TemperatParfaite();  
        }else if (t>=TemperatMax){
        TemperatTropHaute();
        }else if (t<=TemperatMini){
        TemperatTropBasse();
        }
}

void TemperatParfaite(){
      digitalWrite(VentilateurPin, LOW); //Si la temperature est supérieur à 18, le Pin 48 n'est pas alimenté donc le ventilateur est pas alimenté.
      digitalWrite(ThermoplongueurPin, LOW); //Si la temperature est supérieur à 18, le Pin 49 n'est pas alimenté donc le thermoplongueur est pas alimenté.
}

void TemperatTropHaute(){
      digitalWrite(VentilateurPin, HIGH); //Si la temperature est supérieur à 18, le Pin 48 est alimenté donc le ventilateur est alimenté.
      digitalWrite(ThermoplongueurPin, LOW); //Si la temperature est supérieur à 18, le Pin 49 est pas alimenté donc le thermoplongeur est pas alimenté.
    }

void TemperatTropBasse(){
      digitalWrite(VentilateurPin, LOW); //Si la temperature est supérieur à 18, le Pin 48 est pas alimenté donc le ventilateur est pas alimenté.
      digitalWrite(ThermoplongueurPin,HIGH); //Si la temperature est sufèrieur à 18, le Pin 49 est alimenté donc le thermoplongueur est alimenté.
    }

/*Tony CALVEZ*/
void AgirPourLuminosite(){
    int capteurluminosite = analogRead(A8); //Lire la valeur du capteur de niveau d'eau sur A8.
    float tensioncapteurluminosite = capteurluminosite * (5.0 / 1023.0); //Convertir en Tension en fonction du résultat du capteur.
    delay(100);
      if (tensioncapteurluminosite<1){
      digitalWrite(NeonPin, HIGH);
      digitalWrite(MatricePin, HIGH);
      Serial.println("Plantarium - Lampe Allumé"); //Affichage de la tension sur la console.
      }
      else{
      digitalWrite(NeonPin, LOW);
      digitalWrite(MatricePin, LOW);
      Serial.println("Plantarium - Lampe éteinte"); //Affichage de la tension sur la console.
    }
    }
      
void VerifierNiveauDeau(){
    int capteurniveaudeau = analogRead(A9); //Lire la valeur du capteur de niveau d'eau sur A9.
    float tensioncapteurniveaudeau = capteurniveaudeau * (5.0 / 1023.0); //Convertir en Tension en fonction du résultat du capteur.
    delay(100); //Fréquence actualisation.

    if(tensioncapteurniveaudeau>=4.5){
      /*Nous avons 5 Volts mais avec la resistance de l'eau, nous retirons 0,5 Volts*/
      pinMode(DELRougePin, OUTPUT); //Initialisation de la DEL Niveau d'Eau "Alerte" sur la carte arduino, ici, nous avons la LED sur Digital 21 et c'est une sortie.
      pinMode(DELVertPin, OUTPUT); //Initialisation de la DEL Niveau d'Eau "OK" sur la carte arduino, ici, nous avons la LED sur Digital 20 et c'est une sortie.
      digitalWrite(DELRougePin,LOW); //Sécurité mise a l'état bas.
      digitalWrite(DELVertPin, LOW);

      pinMode(DELRougePin, INPUT); //Initialisation de la DEL Niveau d'Eau "Alerte" sur la carte arduino, ici, nous avons la LED sur Digital 21 et c'est une sortie.
      pinMode(DELVertPin, OUTPUT); //Initialisation de la DEL Niveau d'Eau "OK" sur la carte arduino, ici, nous avons la LED sur Digital 20 et c'est une sortie.
      digitalWrite(DELRougePin, LOW); //Si la tension est supérieure à 2.5V, la LED ne s'allume pas.
      digitalWrite(DELVertPin, HIGH);
      delay (500);
      alarmeniveaudeau=0; //Rien ne se passe
      Serial.println("Niveau d'Eau Parfait"); //Affichage de la tension sur la console.
    }
    else if (tensioncapteurniveaudeau>=2.5){

      pinMode(DELRougePin, OUTPUT); //Initialisation de la DEL Niveau d'Eau "Alerte" sur la carte arduino, ici, nous avons la LED sur Digital 21 et c'est une sortie.
      pinMode(DELVertPin, OUTPUT); //Initialisation de la DEL Niveau d'Eau "OK" sur la carte arduino, ici, nous avons la LED sur Digital 20 et c'est une sortie.
      digitalWrite(DELRougePin,LOW); //Sécurité mise a l'état bas.
      digitalWrite(DELVertPin, LOW);

      pinMode(DELRougePin, OUTPUT); //Initialisation de la DEL Niveau d'Eau "Alerte" sur la carte arduino, ici, nous avons la LED sur Digital 21 et c'est une sortie.
      pinMode(DELVertPin, INPUT); //Initialisation de la DEL Niveau d'Eau "OK" sur la carte arduino, ici, nous avons la LED sur Digital 20 et c'est une sortie.
      digitalWrite(DELRougePin,HIGH); //Sinon la tension est inférieure à 2.5V, la LED s'allume.
      digitalWrite(DELVertPin, LOW);
      delay(500);

      pinMode(DELRougePin, INPUT); //Initialisation de la DEL Niveau d'Eau "Alerte" sur la carte arduino, ici, nous avons la LED sur Digital 21 et c'est une sortie.
      pinMode(DELVertPin, OUTPUT); //Initialisation de la DEL Niveau d'Eau "OK" sur la carte arduino, ici, nous avons la LED sur Digital 20 et c'est une sortie.
      digitalWrite(DELRougePin,LOW); //Sinon la tension est inférieure à 2.5V, la LED s'allume.
      digitalWrite(DELVertPin, HIGH);
      delay(500);
      Serial.println("Le Niveau commence à être juste"); //Affichage de la tension sur la console.

    }else{
      pinMode(DELRougePin, OUTPUT); //Initialisation de la DEL Niveau d'Eau "Alerte" sur la carte arduino, ici, nous avons la LED sur Digital 21 et c'est une sortie.
      pinMode(DELVertPin, OUTPUT); //Initialisation de la DEL Niveau d'Eau "OK" sur la carte arduino, ici, nous avons la LED sur Digital 20 et c'est une sortie.
      digitalWrite(DELRougePin,LOW); //Sécurité mise a l'état bas.
      digitalWrite(DELVertPin, LOW);

      pinMode(DELRougePin, OUTPUT); //Initialisation de la DEL Niveau d'Eau "Alerte" sur la carte arduino, ici, nous avons la LED sur Digital 21 et c'est une sortie.
      pinMode(DELVertPin, INPUT); //Initialisation de la DEL Niveau d'Eau "OK" sur la carte arduino, ici, nous avons la LED sur Digital 20 et c'est une sortie.
      digitalWrite(DELRougePin,HIGH); //Sinon la tension est inférieure à 2.5V, la LED s'allume.
      delay(500);
      Serial.println("Risque Manque d'Eau"); //Affichage de la tension sur la console.
      alarmeniveaudeau=1; //L'alarme est lancée    
    }
  }


  void EthernetShield() 
{
  
  EthernetClient client = server.available();

          //On répond à la requête avec nos données
          client.println("HTTP/1.1 200 OK");//Code 200 = Tout s'est bien déroulé
          client.println("Content-Type: application/xml");//On spécifie le MIME-TYPE des données qu'on envoit (XML)
          client.println("Connection: close");  
          client.println();
           
          client.println("<?xml version=\"1.0\"?>");//Entête du document XML
           
          //L'element racine du document XML
          client.println("<sensors>");

            //On crée un élement correspondant à chaque capteur
            client.print("<sensor name=\"sns0\">");

            //On crée un élément enfant contenant les valeurs de température
            client.print("<temperature>");
            client.print(dht.readTemperature());
            client.println("</temperature>");
             
            //On créé un élément enfant contenant les valeurs d'humidité
            client.print("<humidity>");
             client.print(dht.readHumidity());
            client.println("</humidity>");
            
            client.println("</sensor>");
           
          //On ferme l'élement racine
          client.println("</sensors>");
         
    //On laisse au navigateur client le temps de récuperer les données
    delay(1);
     
    //On déconnecte le client
    client.stop();
    Serial.println("client deconnecte");

}