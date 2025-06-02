//inclusión de librerías usadas: LCD y sensor DHT
#include  <LiquidCrystal.h> //control del display
#include  <DHT.h> // lectura de datos del sensor DHT22
#include <Servo.h> // control de los servomotores

// Inicializar el LCD con los pines de interfaz
LiquidCrystal lcd(6,7,9,10,11,12);
#define DHTTYPE DHT22

// definiciones y configuración sensor DHT22
#define TIMEDHT 1000
#define DHTPIN 2

// Definiciones para los pines de los componentes
#define LED 8 // Pin del LED de señalización
#define LEDP 3 // Pin del LED para el control proporcional de iluminación
#define LEDB 4 // Pin del LED que simula la resistencia calefactora
#define LEDBE 5 //Pin del LED  que simula el enfriamiento del espacio de la batería
#define LDR 5 // Pin del sensor LDR
#define LDR2 6 // Pin del sensor LDR2
#define TempBAT 4 // Pin del sensor de temperatura de la batería
#define potPIN 3 // Pin del potenciómetro para simular la calidad del aire
#define WIND_SPEED_PIN A1 // Pin del potenciómetro para simular la velocidad del viento
#define WIND_DIRECTION_PIN A2 // Pin del potenciómetro para simular la dirección del viento
#define Tempdeseada 25 // Valor deseado de temperatura en grados Celsius
#define DEAD_ZONE 2 // Zona muerta en grados Celsius
#define SERVO_HORIZONTAL_PIN 0 // Pin del servomotor para el ajuste horizontal
#define SERVO_VERTICAL_PIN 1 // Pin del servomotor para el ajuste vertical

// Instancias de componentes
DHT dht(DHTPIN, DHTTYPE); // Crear una instancia del sensor DHT
Servo servoHorizontal; // Crear una instancia del servomotor horizontal
Servo servoVertical; // Crear una instancia del servomotor vertical

// declaración de variables
uint32_t timerDHT = TIMEDHT; // Temporizador para las lecturas del DHT
float humidity; // Almacena el valor de humedad leído del sensor DHT
float celsius, celsius2; // Almacena los valores de temperatura en grados Celsius
float fahrenheit; // Almacena el valor de temperatura en grados Fahrenheit
int value, value2, normalizedValue, normalizedValue2; // Variables para el valor del LDR y su valor normalizado
const float BETA = 3950; // coeficiente beta del termistor
int potValor;
String calidad;
float windSpeed = 0;
String windDirection = "";
String estadoCalefaccion= "";
String estadoEnfriamiento= "";
unsigned long energiaConsumida = 0;


// Función para obtención de temperatura ambiente
void getTemperature()
{
  // Esperar un tiempo entre mediciones
  if ((millis() - timerDHT) > TIMEDHT) {
    // Update the timer
    timerDHT = millis();

    // Lectura de humedad y temperatura
    humidity = dht.readHumidity();
    celsius = dht.readTemperature();
    fahrenheit = dht.readTemperature(true);

    // Verificar si hay errores en la lectura
    if (isnan(humidity) || isnan(celsius) || isnan(fahrenheit)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
  }
}

//  Función para controlar la temperatura de la batería
void getTemperatureBat()
{
estadoCalefaccion = "OFF";
estadoEnfriamiento = "OFF";

int analogValue = analogRead(TempBAT);
celsius2 = 1 / (log(1 / (1023. / analogValue - 1)) / BETA + 1.0 / 298.15) - 273.15;

  // Lógica de control ON-OFF con zona muerta
  if (celsius2 < Tempdeseada - DEAD_ZONE) {
    // Activar calefacción
    digitalWrite(LEDB, HIGH);
    digitalWrite(LEDBE, LOW);
  } else if (celsius2 > Tempdeseada + DEAD_ZONE) {
    // Activar enfriamiento
    digitalWrite(LEDBE, HIGH);
    digitalWrite(LEDB, LOW);
  } else {
    // Dentro de la zona muerta, no hacer nada
    digitalWrite(LEDB, LOW);
    digitalWrite(LEDBE, LOW);
  }

   // Actualizar los estados como cadenas
  estadoCalefaccion = digitalRead(LEDB) == HIGH ? "ON" : "OFF";
  estadoEnfriamiento = digitalRead(LEDBE) == HIGH ? "ON" : "OFF";

  // simulación de consumo eléctrico

  if (digitalRead(LEDB) == HIGH || digitalRead(LEDBE) == HIGH) {
    energiaConsumida += 5; // Valor simulado
}
}

// Función para leer la intensidad de luz del LDR
void getLDR()
{
value=analogRead(LDR);
value2=analogRead(LDR2);
normalizedValue= map(value,0,1023,100,0);
normalizedValue2= map(value,0,1023,100,0);

}

// Función para controlar el LED de señalización
void onLED()
{
if (normalizedValue < 50) {
  digitalWrite(LED, HIGH);  // Enciende el LED
  delay(500);  // Espera 500 milisegundos
  digitalWrite(LED, LOW);  // Apaga el LED
  delay(500);  // Espera 500 milisegundos
} else {
  digitalWrite(LED, LOW);  // Asegura que el LED esté apagado si el valor normalizado es mayor o igual a 50
}  
  
  int nivelPWM = map(normalizedValue, 0, 100, 0, 255);
  analogWrite(LEDP, nivelPWM);

}

//función de medida de la calidad del aire, emulado por un potenciómetro
void calidadaire()
{
int potValue = analogRead(potPIN); // Leer el valor del potenciómetro
int airQuality = map(potValue, 0, 1023, 0, 100);

  // Determinar la calidad del aire
  if (airQuality < 20) {
    calidad="MB";
  } else if (airQuality < 40) {
    calidad="B";
  } else if (airQuality < 60) {
    calidad="R";
  } else if (airQuality < 80) {
    calidad="M";
  } else {
    calidad="MM";
  }
  delay(1000); // Esperar un segundo antes de la siguiente lectura
}

// Función para leer la velocidad y dirección del viento
void getWindData() {
  // Leer los valores de los potenciómetros
  int windSpeedValue = analogRead(WIND_SPEED_PIN);
  int windDirectionValue = analogRead(WIND_DIRECTION_PIN);

  // Convertir los valores leídos a velocidad y dirección
  windSpeed = map(windSpeedValue, 0, 1023, 0, 100); // Mapear a un rango de 0 a 100 para la velocidad
  // Convertir el valor leído a una dirección cardinal
  int direction = map(windDirectionValue, 0, 1023, 0, 360); // Mapear a un rango de 0 a 360 para la dirección

  // Determinar la dirección cardinal
  if (direction < 22.5 || direction >= 337.5) {
    windDirection = "N";
  } else if (direction < 67.5) {
    windDirection = "NE";
  } else if (direction < 112.5) {
    windDirection = "E";
  } else if (direction < 157.5) {
    windDirection = "SE";
  } else if (direction < 202.5) {
    windDirection = "S";
  } else if (direction < 247.5) {
    windDirection = "SO";
  } else if (direction < 292.5) {
    windDirection = "O";
  } else if (direction < 337.5) {
    windDirection = "NO";
  }
}

// Función que ajusta las placas solares según intensidad lumínica
void ajusteplacas()
{

  // Determinar la dirección de la luz solar
  int difference = value - value2;

  // Ajustar los servomotores basado en la diferencia de luz entre los dos sensores
  if (abs(difference) > 100) { // Umbral para considerar un cambio significativo en la luz
    if (difference > 0) {
      // Más luz en LDR 1, ajustar hacia LDR 1
      int horizontalPos = map(difference, 0, 1023, 90, 180);
      servoHorizontal.write(horizontalPos); // Ajustar el servomotor horizontal
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ajuste Horizontal:");
      lcd.setCursor(0, 1);
      lcd.print(horizontalPos);
      delay (2000);
      
    } else {
      // Más luz en LDR 2, ajustar hacia LDR 2
      int horizontalPos = map(abs(difference), 0, 1023, 90, 0);
      servoHorizontal.write(horizontalPos); // Ajustar el servomotor horizontal
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ajuste Horizontal:");
      lcd.setCursor(0, 1);
      lcd.print(horizontalPos);
      delay(2000);
      
    }
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sin ajuste");
    delay(2000);
  }

  // Ajustar el servomotor vertical basado en la intensidad de luz máxima
  int maxLight = max(value, value2);
  int verticalPos = map(maxLight, 0, 1023, 0, 90);
  servoVertical.write(verticalPos); // Ajustar el servomotor vertical
  // Mostrar el ajuste vertical después de un pequeño retraso
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ajuste Vertical:");
  lcd.setCursor(0, 1);
  lcd.print(verticalPos);

  delay(2000); // Esperar un segundo antes de la siguiente lectura
}

void energiaconsumida()
{
lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("Energia consumida");
  lcd.setCursor(0, 1);
  lcd.print("Valor: ");
  lcd.print(energiaConsumida);
  lcd.print(" Wh");
  delay(3000);
}

// función que simula el comienzo del ajuste de la placa solar
void printajuste()
{
  lcd.clear();
  lcd.print("Ajuste placa solar"); // Mensaje LCD de ajuste
  delay(500);
  lcd.clear();
  delay(500);
  lcd.print("Ajuste placa solar"); // Mensaje LCD de ajuste
  delay(500);
  lcd.clear();
  delay(500);
  lcd.print("Ajuste placa solar"); // Mensaje LCD de ajuste
  delay(500);
}

// función que muestra los valores de los sensors del compartimento de la batería
void printvalorbateria()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("<<Temp. Bateria>> ");
  lcd.setCursor(0, 1);
  lcd.print("TB:");
  lcd.print(celsius2);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("Calentamiento: ");
  lcd.print(estadoCalefaccion);
  lcd.setCursor(0, 2);
  lcd.print("Enfriamiento: ");
  lcd.print(estadoEnfriamiento);
  delay(2000);
}

// función que envía a LCD el valor de las variables
void printresults()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("<<<<< BOYA XXX >>>>>");
  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print(celsius);
  lcd.print("C H:");
  lcd.print(humidity);
  lcd.print("%");
  lcd.setCursor(0, 2);
  lcd.print("I:");
  lcd.print(normalizedValue);
  lcd.print("Cd ");
  lcd.print("C.air:");
  lcd.print(calidad);
  lcd.setCursor(0, 3);
  lcd.print("Vv:");
  lcd.print(windSpeed);
  lcd.print("Km/h");
  lcd.print(" Dv:");
  lcd.print(windDirection);
  delay(5000);
  //lcd.clear();  
}

void setup() 
{
  lcd.begin(20,4);
  lcd.setCursor(0, 0);

  // simulamos una carga del sistema operativo en el arduino 
  lcd.print(">> inicializando <<");
  delay(200);
  lcd.setCursor(0, 2);
  // proceso de carga
  for (int i = 0; i < 19; i++) {
    lcd.print(">");
    delay(200); // Pequeña pausa para simular la carga
  }
  delay(2000);  
  dht.begin();
  servoHorizontal.attach(SERVO_HORIZONTAL_PIN); // Conectar el servomotor horizontal
  servoVertical.attach(SERVO_VERTICAL_PIN); // Conectar el servomotor vertical

// configuración de puertos
  pinMode(LDR, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(LEDBE, OUTPUT);
}

void loop() 
{
  getTemperature(); //llamada a la función para obtener temperatura ambiente
  getLDR();//llamada a la función para obtener intensidad lumínica
  onLED();//llamada a la función para encender led de señalización
  calidadaire();//llamada a la función que mide la calidad del aire, simulado por un potenciómetro
  getWindData(); // Llamar a la función para obtener los datos del viento
  printresults();//imprime resultado de variables a monitor
  getTemperatureBat();//llamada a la función para gestionar carga batería
  printvalorbateria();//llamada a la función que muestra los parámetros de la batería
  printajuste();//Llamada a la función que indica que comienza el ajuste de la placa solar
  ajusteplacas();//llamada a la función que ajusta vertical y horizontalmente la placa solar
  energiaconsumida();//llamada a la función que muestra la energía consumida en el lcd
  delay(3000);
}