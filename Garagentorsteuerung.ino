//Garagentorsteuerung

//Delays zwischen den verschiedenen operationen
//Können angepasst werden
int clockspeed_major = 1000; //1 Sekunde; nach dem blinken
int clockspeed_minor = 100; //0.100 Sekunde; zwischen dem blinken

//Input Pins
int endschalter_oben_pin = 10;
int endschalter_unten_pin = 11;
int taster_pin = 12;
int sicherheits_schleife_pin = 13;
int externer_taster_pin = 14;//zb für ESP oder Funk
int notaus_pin = 15;

boolean notaus_enabled = false;//Notaus aktiv  ----> //Wenn notaus aktiv -> Pin 15 High -> Alles OK; Pin 15 Low -> Keine Bedienung wird ausgeführt

//Output Pins
int auf_pin = 2;
int zu_pin = 3;

//Variableln
int AUF = 1;
int ZU = 0;

//Letzter stand
int last_state = 0; //0 ist zu, 1 ist auf

void setup() {
  //Serielle Kommunikation beginnen
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  //Kurzer delay bis serial vollständig initialisiert ist
  delay(1500);
  
  //Start der initialisierung
  Serial.println("Setup starten...");
  Serial.println(">> Serielle Kommunikation gestartet - Baud Rate: 115200");
  Serial.println("Initialisiere Pins...");
  delay(1000);

  //Die verschiedenen PINs initialisieren
  pinMode(taster_pin, INPUT);
  Serial.println(".in.1");
  pinMode(sicherheits_schleife_pin, INPUT);
  Serial.print(".in.2");
  pinMode(endschalter_unten_pin, INPUT);
  Serial.print(".in.3");
  pinMode(endschalter_oben_pin, INPUT);
  Serial.print(".in.4");
  pinMode(externer_taster_pin, INPUT);
  Serial.print(".in.5");
  pinMode(notaus_pin, INPUT);
  Serial.print(".in.6");
  delay(1000);

  pinMode(auf_pin, OUTPUT);
  Serial.println(".out.1");
  pinMode(zu_pin, OUTPUT);
  Serial.print(".out.2");

  digitalWrite(auf_pin, LOW);
  digitalWrite(zu_pin, LOW);

  pinMode(LED_BUILTIN, OUTPUT);
  delay(1000);

  Serial.println(">> Alle pin/gpio Einstellungen wurden vorgenommen");

  //Hinweis texte
  if (!notaus_enabled) {
    Serial.println(">> Info: Die Notaus Funktion ist nicht aktiv!");
  }

  if (digitalRead(sicherheits_schleife_pin) == LOW) {
    Serial.println(">> Info: Die Sicherheitsschleife ist unterbrochen.");
  }

}

void handleCLICMD(String CMD) {
//Hier werden zukünftig die befehle behandelt
}

void loop() {

  //LED Blinken und clockspeed
  blink();
  
  //lesen der verschiedenen werte der Inputs und Outputs
  int taster = digitalRead(taster_pin);
  int externer_taster = digitalRead(externer_taster_pin);
  int sicherheits_schleife = digitalRead(sicherheits_schleife_pin);
  int endschalter_oben = digitalRead(endschalter_oben_pin);
  int endschalter_unten = digitalRead(endschalter_unten_pin);
  int notaus = digitalRead(notaus_pin);

  //Eingabe befehle; zzt unbenutzt
  String cli_input = Serial.readString();
  handleCLICMD(cli_input);

  //Wenn notaus aktiv ist dann kann hier ein beliebiger code eingefügt werden
  if (notaus_enabled) {
    if (notaus == LOW) {
      Serial.println(">> Notaus ist betätigt! Keine Bedienung möglich!");
      //Hier einfügen was sonst noch passieren soll z.B. fahrt stoppen
      return;
    }
  }

  //Sicherheitsschleife ist unterbrochen -> Fahrt stoppen
  if (sicherheits_schleife == LOW || sicherheits_schleife == -1) {
    digitalWrite(zu_pin, LOW);
    digitalWrite(auf_pin, LOW);
    Serial.println(">> Sicherheitsschleife unterbrochen.");
    return;
  }

  //Taster gedrückt
  if (taster == HIGH || externer_taster == HIGH) {
    Serial.println(">> Taster ist gedrückt.");
    
    //Tor fährt bereits -> Fahrt stoppen
    if (digitalRead(auf_pin) == HIGH || digitalRead(zu_pin) == HIGH) {
      digitalWrite(zu_pin, LOW);
      digitalWrite(auf_pin, LOW);
      Serial.println(">> Aktive Torfahrt wurde gestoppt.");
      return;
    }

    //Tor ist auf -> Zu fahren
    if (endschalter_oben == HIGH) {
      digitalWrite(auf_pin, LOW);
      if (digitalRead(auf_pin) == LOW) {
        digitalWrite(zu_pin, HIGH);
        last_state = ZU;//state = 0-> Zu
        Serial.println(">> Tor zu fahren.");
      }
      return;
    }

    //Tor ist zu -> Auf fahren
    if (endschalter_unten == HIGH) {
      digitalWrite(zu_pin, LOW);
      if (digitalRead(zu_pin) == LOW) {
        digitalWrite(auf_pin, HIGH);
        last_state = AUF;//state = 1 -> Auf
        Serial.println(">> Tor auf fahren.");
      }
      return;
    }

    //Torstellung unbekannt -> letzter stand zu -> AUF ausführen
    if (endschalter_unten == LOW && endschalter_oben == LOW) {
      Serial.println(">> Torstellung unbekannt.");
      if (last_state == ZU) {
        digitalWrite(zu_pin, LOW);
        if (digitalRead(zu_pin) == LOW) {
          digitalWrite(auf_pin, HIGH);
          last_state = AUF;
          Serial.println(">> Tor auf fahren (Unbekannte Stellung)");
        }
        return;
      }

      //Torstellung unbekannt -> letzter stand auf -> ZU ausführen
      if (last_state == AUF) {
        digitalWrite(auf_pin, LOW);
        if (digitalRead(auf_pin) == LOW) {
          digitalWrite(zu_pin, HIGH);
          last_state = ZU;//state = 0-> Zu
          Serial.println(">> Tor zu fahren (Unbekannte Stellung)");
        }
        return;
      }

    }//end(
  }
}

int counter = 0;
int blink_count = 3;

void blink() {
  digitalWrite(LED_BUILTIN, counter < 2 ? HIGH : LOW);
  delay(clockspeed_minor);
  digitalWrite(LED_BUILTIN, LOW);
  delay(clockspeed_minor);
  digitalWrite(LED_BUILTIN, counter < 2 ? HIGH : LOW);
  delay(clockspeed_minor);
  digitalWrite(LED_BUILTIN, LOW);
  if (counter > blink_count) {
    counter = 0;
  }
  delay(clockspeed_major);
  counter ++;
}
