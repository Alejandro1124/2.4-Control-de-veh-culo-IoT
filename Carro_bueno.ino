// Definir pines y constantes
const int trigPin = 18, echoPin = 19;               // Pines para el sensor ultrasónico HC-SR04 (Trigger y Echo)
const int enable[] = {26, 25};                      // Pines Enable A y B para controlar la velocidad de los motores
const int inputs[] = {27, 14, 13, 12};              // Pines IN1, IN2, IN3, IN4 para controlar la dirección de los motores

// Matriz que define los movimientos de los motores {IN1, IN2, IN3, IN4}
// Cada fila representa un tipo de movimiento:
// {Avanzar adelante}, {Retroceder}, {Girar izquierda}, {Girar derecha}
const int movimientos[][4] = {
  {HIGH, LOW, HIGH, LOW},   // Avanzar hacia adelante
  {LOW, HIGH, LOW, HIGH},   // Retroceder
  {LOW, LOW, HIGH, LOW},    // Girar a la izquierda
  {LOW, HIGH, HIGH, LOW}    // Girar a la derecha (ambos motores giran en direcciones opuestas)
};

// Matriz que define las velocidades de los motores {Motor A, Motor B} para cada movimiento
// {Máxima velocidad}, {Media velocidad}, {Giro izquierda}, {Giro derecha}
const int velocidades[][2] = {
  {255, 255},   // Máxima velocidad (avanzar)
  {250, 250},   // Media velocidad (retroceder)
  {0, 250},     // Motor A detenido, Motor B en movimiento (giro izquierda)
  {250, 250}    // Ambos motores en movimiento para girar más rápido a la derecha
};

const int distanciaSegura = 30;  // Distancia mínima (en centímetros) antes de considerar un obstáculo

// Función para detener los motores
// "tiempo" es opcional; si se proporciona, los motores se detendrán durante ese tiempo
void detenerMotores(int tiempo = 0) {
  for (int i = 0; i < 2; i++) analogWrite(enable[i], 0);  // Apaga ambos motores
  delay(tiempo);  // Espera el tiempo especificado
}

// Función para mover los motores según el tipo de movimiento
// "tipo" es un índice que selecciona un movimiento específico desde la matriz "movimientos"
void moverMotores(int tipo) {
  // Asigna los valores de dirección a los pines IN1, IN2, IN3 e IN4 según el tipo de movimiento
  for (int i = 0; i < 4; i++) digitalWrite(inputs[i], movimientos[tipo][i]);
  
  // Configura la velocidad de los motores A y B de acuerdo a la matriz "velocidades"
  analogWrite(enable[0], velocidades[tipo][0]);  // Motor A
  analogWrite(enable[1], velocidades[tipo][1]);  // Motor B
}

// Función para medir la distancia usando el sensor ultrasónico HC-SR04
// Retorna la distancia calculada en centímetros
float medirDistancia() {
  // Enviar un pulso al pin Trigger para iniciar la medición
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2);  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);  
  digitalWrite(trigPin, LOW);

  // Leer la duración del pulso de respuesta desde el pin Echo
  long duration = pulseIn(echoPin, HIGH);
  
  // Convertir la duración del pulso en distancia (cm)
  return duration * 0.0343 / 2;
}

void setup() {
  Serial.begin(115200);  // Iniciar la comunicación serial para depuración
  
  // Configurar los pines del sensor ultrasónico
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Configurar los pines de los motores como salida
  for (int i = 0; i < 2; i++) pinMode(enable[i], OUTPUT);
  for (int i = 0; i < 4; i++) pinMode(inputs[i], OUTPUT);

  detenerMotores();  // Asegurarse de que los motores estén apagados al iniciar
}

void loop() {
  // Medir la distancia al obstáculo más cercano
  float distancia = medirDistancia();
  
  // Mostrar la distancia medida en el monitor serial
  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");

  // Si no hay obstáculos a la vista, avanzar
  if (distancia > distanciaSegura) {
    moverMotores(0);  // Movimiento de avanzar adelante
    Serial.println("Avanzando hacia adelante...");
  } else {
    // Si hay un obstáculo, detenerse y girar
    detenerMotores(100);  // Detenerse brevemente antes de girar
    Serial.println("Obstáculo detectado, girando a la derecha...");
    
    // Continuar girando a la derecha hasta que no haya obstáculos
    while (medirDistancia() <= distanciaSegura) {
      moverMotores(3);  // Movimiento de girar a la derecha
      delay(300);  // Esperar un poco entre cada intento de detección
    }
    
    Serial.println("Obstáculo evitado, avanzando...");
  }

  delay(50);  // Reducir el tiempo de espera para mejorar la respuesta de la detección
}
