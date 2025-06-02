# Boya Climática - Sistema de Control y Actuación

Este proyecto simula una boya meteorológica inteligente que mide y controla condiciones climáticas en tiempo real utilizando una placa Arduino UNO. La lógica incluye control de temperatura de baterías, ajuste solar y control proporcional de iluminación, todo visualizado en una pantalla LCD.

---

##  ¿Qué hace este proyecto?

- Mide temperatura, humedad, calidad del aire, viento e iluminación.
- Controla automáticamente la temperatura del compartimento de baterías (calefacción/enfriamiento).
- Ajusta el ángulo de placas solares en función de la luz incidente.
- Controla un LED de iluminación mediante señal PWM proporcional.
- Muestra toda la información del sistema en una pantalla LCD 20x4.
- Estima energía consumida en función del comportamiento de los actuadores.

---

##  ¿Por qué es útil este proyecto?

Este sistema simulado permite comprender los fundamentos de un sistema embebido de control climático:
- Introduce al estudiante en el uso de sensores y actuadores reales.
- Simula condiciones ambientales de forma dinámica en entorno educativo.
- Aplica lógica de control simple (ON/OFF) y proporcional (PWM).
- Es una base para proyectos de eficiencia energética, domótica y sistemas IoT.

---

##  ¿Cómo comenzar a usar el proyecto?

1. Clona este repositorio o descarga los archivos ZIP.
2. Abre `sim_boya.ino` en la plataforma de simulación [Wokwi](https://wokwi.com).
3. Añade los componentes en Wokwi según el esquema (`esquema.png`).
4. Ejecuta la simulación y observa el comportamiento del sistema.
5. Modifica valores del potenciómetro o luz (LDR) para probar respuestas del sistema.

---

##  ¿Dónde puedo obtener ayuda?

- Puedes abrir una **issue** en este mismo repositorio.
- Contactar al autor a través de [sointelca.com](https://sointelca.com).
- Revisar los ejemplos de Wokwi para problemas específicos con la simulación.
- Revisar el código fuente comentado incluido en el `.ino`.

---

##  ¿Quién mantiene y contribuye?

Este proyecto fue desarrollado por:

**Gustavo Martín Ramírez**  
Ingeniero Técnico de Telecomunicación - desarrollador de soluciones educativas digitales.  

---

##  Referencias

- Cameron, N. (2019). *Arduino Applied*. Apress.
- Elegoo (2020). *UNO Super Starter Kit Tutorial*.
- Corona Ramírez, L.G. et al. (2014). *Sensores y actuadores con Arduino*.
- Torrente Artero, O. (2013). *Arduino. Curso práctico de formación*.
- Purdum, J. (2015). *Beginning C for Arduino*. Apress.

---

##  Fragmentos clave del código

```cpp
// Control de temperatura ON-OFF con zona muerta
if (celsius2 < Tempdeseada - DEAD_ZONE) {
    digitalWrite(LEDB, HIGH);
    digitalWrite(LEDBE, LOW);
} else if (celsius2 > Tempdeseada + DEAD_ZONE) {
    digitalWrite(LEDBE, HIGH);
    digitalWrite(LEDB, LOW);
} else {
    digitalWrite(LEDB, LOW);
    digitalWrite(LEDBE, LOW);
}
```

```cpp
// Control proporcional de iluminación
int nivelPWM = map(normalizedValue, 0, 100, 0, 255);
analogWrite(LEDP, nivelPWM);
```

```cpp
// Función que ajusta las placas solares usando sensores LDR y servos
void ajusteplacas()
{
  // Diferencia de luz entre los dos sensores LDR
  int difference = value - value2;

  // Si hay una diferencia significativa de luz, ajusta la orientación horizontal
  if (abs(difference) > 100) { // Umbral de sensibilidad
    if (difference > 0) {
      // Más luz en LDR 1, girar hacia ese lado
      int horizontalPos = map(difference, 0, 1023, 90, 180);
      servoHorizontal.write(horizontalPos); // Mover servo horizontal
    } else {
      // Más luz en LDR 2, girar hacia ese lado
      int horizontalPos = map(abs(difference), 0, 1023, 90, 0);
      servoHorizontal.write(horizontalPos); // Mover servo horizontal
    }
  }

  // Ajuste vertical basado en la mayor intensidad de luz detectada
  int maxLight = max(value, value2);
  int verticalPos = map(maxLight, 0, 1023, 0, 90); // Mapear a ángulo servo
  servoVertical.write(verticalPos); // Mover servo vertical
}
```
---

## 🗂️ Estructura del repositorio

```
boya-climatica/
├── sim_boya.ino                  # Código fuente Arduino
├── README.md                     # Información sobre el proyecto
├── esquema.png                   # Captura del circuito desde Wokwi
├── diagram.json                  # Diagrama de componentes y conexionado
├── capturas simulación           # Capturas de la información mostrada en el LCD
└── LICENSE                       # Licencia de uso
```
