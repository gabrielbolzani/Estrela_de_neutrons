#include <LiquidCrystal.h>

#define encoderCLK A5
#define encoderDT A4
#define encoderSW A3

#define relayPin 6

#define doorSensorPin A2

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);



// Variáveis para o encoder
int lastCLK = HIGH;
int currentCLK;
int lastButtonState = HIGH;
int currentButtonState;

// Variáveis de tempo
int curingTime = 0; // em minutos
bool isCuring = false;
bool isPaused = false;
bool isComplete = false;
unsigned long startTime;
unsigned long pausedTime;

// Variáveis para o sensor de porta
int lastDoorState = HIGH;
int currentDoorState;

void setup() {
  // Inicializa o LCD
  lcd.begin(16, 2);
  lcd.print("   Estrela de");
  lcd.setCursor(0, 1);
  lcd.print(" Neutrons v1.0");

  // Inicializa o pino do relé
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  // Inicializa os pinos do encoder
  pinMode(encoderCLK, INPUT);
  pinMode(encoderDT, INPUT);
  pinMode(encoderSW, INPUT_PULLUP);

  // Inicializa o pino do sensor de porta
  pinMode(doorSensorPin, INPUT);

  // Inicializa a Serial para depuração
  Serial.begin(115200);
}

void loop() {
  // Lê o estado da porta
  currentDoorState = digitalRead(doorSensorPin);

  // Verifica o estado da porta e pausa/resume a cura conforme necessário
  handleDoorState();

  // Se não está curando e não está pausado, permite ajustes de tempo e inicia a cura
  if (!isCuring && !isPaused && !isComplete) {
    handleEncoder();
    handleButtonPress();
  } else if (isCuring) {
    // Se está curando, verifica se há interrupção pelo botão e atualiza o tempo restante
    handleCuringProcess();
  }

  // Salva o estado da porta
  lastDoorState = currentDoorState;
}

// Função para lidar com o estado da porta
void handleDoorState() {
  if (currentDoorState == HIGH) { // Porta está aberta
    if (isCuring) {
      pauseCuringProcess();
    }
  } else { // Porta está fechada
    if (isPaused && !isComplete) {
      resumeCuringProcess();
    }
    if (isComplete && lastDoorState == HIGH && currentDoorState == LOW) {
      resetAfterCompletion();
    }
  }
}

// Função para pausar o processo de cura
void pauseCuringProcess() {
  isPaused = true;
  pausedTime = millis();
  digitalWrite(relayPin, LOW); // Desliga a luz UV
  isCuring = false; // Pausa o processo de cura

  // Atualiza o LCD
  lcd.setCursor(0, 0);
  lcd.print(" Porta Aberta   ");
  Serial.println("Porta aberta, cura pausada");
}

// Função para resumir o processo de cura
void resumeCuringProcess() {
  isPaused = false;
  isCuring = true;
  startTime += millis() - pausedTime; // Ajusta o tempo de início
  digitalWrite(relayPin, HIGH); // Liga a luz UV
  lcd.clear(); // Limpa o LCD antes de retomar
  lcd.setCursor(0, 0);
  lcd.print("Tempo cura ");
  lcd.print(curingTime);
  lcd.print(":00   ");
  Serial.println("Porta fechada, cura retomada");
}

// Função para resetar após conclusão do processo de cura
void resetAfterCompletion() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   Estrela de");
  lcd.setCursor(0, 1);
  lcd.print(" Neutrons v1.0");
  curingTime = 0;
  isComplete = false;
  Serial.println("Porta fechada após conclusão, reset para tela inicial");
}

// Função para lidar com o encoder
void handleEncoder() {
  currentCLK = digitalRead(encoderCLK);
  if (currentCLK != lastCLK && currentCLK == LOW) {
    if (digitalRead(encoderDT) != currentCLK) {
      curingTime++; // Incrementa em 1 minuto
    } else {
      curingTime--; // Decrementa em 1 minuto
    }

    // Se o tempo de cura for menor que zero, retorna à tela inicial
    if (curingTime < 0) {
      resetToInitialScreen();
      return;
    }

    // Atualiza o LCD
    lcd.setCursor(0, 0);
    lcd.print("Tempo cura ");
    lcd.print(curingTime);
    lcd.print(":00   "); // Limpa caracteres extras
    lcd.setCursor(0, 1);
    lcd.print(" Restante: --:--");
    Serial.print("Tempo de cura ajustado para: ");
    Serial.print(curingTime);
    Serial.println(" minutos");
  }
  lastCLK = currentCLK;
}

// Função para lidar com o pressionamento do botão
void handleButtonPress() {
  currentButtonState = digitalRead(encoderSW);
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    if (currentDoorState == LOW) { // Inicia a cura apenas se a porta estiver fechada
      startCuringProcess();
    } else {
      // Atualiza o LCD para mostrar mensagem de porta aberta
      lcd.setCursor(0, 0);
      lcd.print(" Porta Aberta   ");
      lcd.setCursor(0, 1);
      lcd.print("Feche a porta  ");
      Serial.println("Tentativa de iniciar com porta aberta");
    }
  }
  lastButtonState = currentButtonState;
}

// Função para iniciar o processo de cura
void startCuringProcess() {
  isCuring = true;
  startTime = millis();
  digitalWrite(relayPin, HIGH); // Liga a luz UV

  // Atualiza o LCD
  lcd.setCursor(0, 0);
  lcd.print("Tempo cura ");
  lcd.print(curingTime);
  lcd.print(":00   ");
  Serial.println("Cura iniciada");
}

// Função para lidar com o processo de cura
void handleCuringProcess() {
  // Verifica interrupção pelo botão
  currentButtonState = digitalRead(encoderSW);
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    interruptCuringProcess();
  }
  lastButtonState = currentButtonState;

  // Calcula o tempo decorrido
  unsigned long elapsedMillis = millis() - startTime;
  int elapsedSeconds = elapsedMillis / 1000;

  // Calcula o tempo restante
  int remainingTime = curingTime * 60 - elapsedSeconds; // em segundos

  // Formata o tempo restante como mm:ss
  int remainingMinutes = remainingTime / 60;
  int remainingSeconds = remainingTime % 60;

  // Atualiza o LCD com o tempo restante
  lcd.setCursor(0, 1);
  lcd.print("Restante: ");
  lcd.print(remainingMinutes);
  lcd.print(":");
  if (remainingSeconds < 10) {
    lcd.print("0"); // Zero à esquerda para segundos de um dígito
  }
  lcd.print(remainingSeconds);
  lcd.print("   "); // Limpa caracteres extras

  Serial.print("Tempo restante: ");
  Serial.print(remainingMinutes);
  Serial.print(":");
  if (remainingSeconds < 10) {
    Serial.print("0");
  }
  Serial.println(remainingSeconds);

  // Verifica se o tempo de cura terminou
  if (remainingTime <= 0) {
    completeCuringProcess();
  }
}

// Função para interromper o processo de cura
void interruptCuringProcess() {
  isCuring = false;
  digitalWrite(relayPin, LOW); // Desliga a luz UV

  // Atualiza o LCD
  lcd.setCursor(0, 0);
  lcd.print("*****Parado*****");
  lcd.setCursor(0, 1);
  lcd.print("Tempo: --:--    ");
  Serial.println("Cura interrompida pelo usuário");
}

// Função para concluir o processo de cura
void completeCuringProcess() {
  isCuring = false;
  digitalWrite(relayPin, LOW); // Desliga a luz UV

  // Atualiza o LCD
  lcd.setCursor(0, 0);
  lcd.print(" Cura concluida ");
  lcd.setCursor(0, 1);
  lcd.print("  Total: ");
  lcd.print(curingTime);
  lcd.print(":00    ");
  isComplete = true;
  Serial.println("Cura concluída");
}

// Função para resetar para a tela inicial
void resetToInitialScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   Estrela de");
  lcd.setCursor(0, 1);
  lcd.print(" Neutrons v1.0");
  curingTime = 0;
  isComplete = false;
  isCuring = false;
  isPaused = false;
  Serial.println("Tempo de cura ajustado para menos de zero, reset para tela inicial");
}
