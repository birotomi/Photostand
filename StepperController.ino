//Lábak inicializálása
#define stepperEnablePin 3
#define stepperStepPin 4
#define stepperDirectionPin 5

//Parancsok
const char Step[] = "step";
const char SerialInput::Ping[] = "ping";

void setup() {
    //Serial kommunikáció indítása
    Serial.begin(115200);
    Serial.flush();
    //200 bájt egy csomag mérete
    cmdString.reserve(200);
}

void loop() {
}

//Soros kommunikáció folyamatos figyelése, parancsok feldolgozása, végrehajtása
//A parancsokat '|' karakter zárja le, ha több parancs jön egyszerre, e karakter mentén 
//sorban feldolgozzuk őket
void serialEvent() {
  //Fogadott parancsok tárolója
  char cmdStringArray[50];
  //Tároló ürítése
  memset(cmdStringArray, 0, sizeof(cmdStringArray));
  int i = 0;
  while (Serial.available()){
    cmdStringArray[i] = (char)Serial.read();
    i++;
  }

  //Fogadott parancs(ok) feldolgozása
  char * pch;
  pch = strtok(cmdStringArray, "|");
  while (pch != NULL)
  {
    runCommand(cmdStringArray, true);
    pch = strtok(NULL, "|");
  }
}

//Egy karaktertömbről eldönti, hogy az adott parancsot tartalmazza-e
bool isCommand(char* pSerialCommand, const char* command){
  if (strlen(pSerialCommand) >= strlen(command)){
    pSerialCommand = strlwr(pSerialCommand);
    //command = strlwr(command);
    for (int i = 0; i < strlen(command); i++)
    {
      if (pSerialCommand[i] != command[i]) return false;
    }
  }
  else return false;
  return true;
}

void runCommand(char* pCommand, bool pIsSerial){
  Serial.println(commandResult);
  Serial.println("RunCommand");
  pCommand = strlwr(pCommand);
  //Bejövő parancsok feldolgozása
  if (Common::isCommand(pCommand, Step)){
    //Ha lépés parancs, megadjuk az irányt és a lépésszámot is
    motorStep(atoi(strcat(&pCommand[5], &pCommand[6])), &pCommand[4]);
    return;
  }
  else if (Common::isCommand(pCommand, SerialInput::Ping)){
    Ping(commandResult);
    return;
  }
  else{ Serial.println("Wrong command"); return; }
}

//Motort vezérli, meghatározott mezőnyi lépést tesz a meghatározott irányba
//Direction: f -> forward, b -> backward
void motorStep(int count, char* direction_){
  Serial.println("Step...");
  //Hány fogaskerék ugrása legyen egy lépés
  int howManySignalOneStep = 3;

  //Engedélyezzük a motor vezérlését
  digitalWrite(stepperEnablePin, LOW);
  
  //Beállítjuk az irányt
  Serial.println(direction_);
  if (direction_[0] == 'f'){
    Serial.println("Elore");
    digitalWrite(stepperDirectionPin, LOW);
  }
  else if (direction_[0] == 'b'){
    Serial.println("Hatra");
    digitalWrite(stepperDirectionPin, HIGH);
  }
  else{
    return;
  }

  //1 lépés "count" szor elvégezve
  for (int x = 0; x < count * howManySignalOneStep; x++) {
      speed(1000);
  }

  //Tiltjuk a motor vezérlését
  digitalWrite(stepperEnablePin, HIGH);

  //Elküldjük a számítógépnek, hogy befejeződött a léptetés
  strcpy(commandResult, SerialOutput::FinishSteps);
  return;
}

//Motorvezérléshez -> egy lépés. Paraméter a sebesség
void Step(int speed){
  digitalWrite(stepperStepPin, HIGH);
  delayMicroseconds(speed);
  digitalWrite(stepperStepPin, LOW);
  delayMicroseconds(speed);
}
