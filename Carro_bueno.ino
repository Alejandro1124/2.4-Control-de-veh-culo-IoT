// Definir los pines del sensor HC-SR04
const int trigPin = 18;  // Pin de Trigger
const int echoPin = 19;  // Pin de Echo

// Definir los pines del carro
const int enable[2] = {26, 25}; // Pines Enable A y B
const int inputs[4] = {27, 14, 13, 12}; // Pines IN1, IN2, IN3, IN4

// Matriz de movimientos: {IN1, IN2, IN3, IN4}
const int movimientos[][4] = {
  {HIGH, LOW, HIGH, LOW},  // Avanzar adelante
  {LOW, HIGH, LOW, HIGH},  // Avanzar atrás
  {LOW, LOW, HIGH, LOW},   // Girar izquierda
  {LOW, HIGH, HIGH, LOW},  // Girar derecha (ambos motores en direcciones opuestas)
};

// Matriz de velocidades: {Motor A, Motor B}
const int velocidades[][2] = {
  {255, 255},  // Máxima velocidad (adelante)
  {250, 250},  // Media velocidad (atrás)
  {0, 250},    // Izquierda
  {250, 250},  // Derecha (ambos motores en movimiento)
};

// Distancia mínima para detectar un obstáculo
const int distanciaSegura = 30;  // En centímetros

// Función para detener los motores
void detenerMotores(int tiempo = 0) {
  for (int i = 0; i < 2; i++) analogWrite(enable[i], 0);
  delay(tiempo);
}

// Función genérica para mover los motores
void moverMotores(int tipo, int velocidad) {
  for (int i = 0; i < 4; i++) digitalWrite(inputs[i], movimientos[tipo][i]);
  analogWrite(enable[0], velocidades[tipo][0]);
  analogWrite(enable[1], velocidades[tipo][1]);
}

// Función para medir la distancia usando el sensor HC-SR04
float medirDistancia() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.0343 / 2;
  return distance;
}

void setup() {
  Serial.begin(115200);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  for (int i = 0; i < 2; i++) pinMode(enable[i], OUTPUT);
  for (int i = 0; i < 4; i++) pinMode(inputs[i], OUTPUT);

  detenerMotores();
}

void loop() {
  float distancia = medirDistancia();
  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");

  if (distancia > distanciaSegura) {
    moverMotores(0, 255);  // Avanzar adelante a máxima velocidad
    Serial.println("Avanzando hacia adelante...");
  } else {
    // Hay un obstáculo, girar hasta que no haya más obstáculos
    detenerMotores(100);  // Breve pausa antes de girar
    Serial.println("Obstáculo detectado, girando a la derecha...");
    
    // Continuar girando a la derecha hasta que no haya obstáculo
    while (medirDistancia() <= distanciaSegura) {
      moverMotores(3, 250);  // Girar a la derecha (ambos motores en direcciones opuestas)
      delay(300);  // Ciclo rápido de giro
    }
    
    Serial.println("Obstáculo evitado, avanzando...");
  }

  delay(50);  // Reducir el tiempo de espera para mejorar la detección
}
