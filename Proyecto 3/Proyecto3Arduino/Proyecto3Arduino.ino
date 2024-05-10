const int order = 9; //orden del filtro IIR

float b[order+1] = {0.97146805, -8.74321246, 34.97284984, -81.6033163, 122.40497445, -122.40497445, 81.6033163, -34.97284984, 8.74321246, -0.97146805};
float a[order+1] = {1.0, -8.94210655, 35.53852717, -82.39067533, 122.79294834, -122.00560458, 80.81595706, -34.41368447, 8.54838853, -0.94375017};

float x[order+1] = {0}; //buffer de entrada
float y[order+1] = {0}; //buffer de salida

const int ledPIN = 9;

const int numDatos = 100; //número de datos en el vector
int datos[numDatos]; //vector para almacenar los datos
int ultPos = 0;

void setup() {
  Serial.begin(9600);
  pinMode(ledPIN, OUTPUT);

  //leer los primeros datos del pin análogo
  for (int i = 0; i < numDatos; i++) {
    datos[i] = analogRead(A0);
  }
}

void loop() {
  datos[ultPos] = analogRead(A0);
  ultPos = (ultPos + 1) % numDatos;
  applyIIRFilter();
  int slopeChanges = calcularSlopeSignChanges();
  Serial.println(slopeChanges);

  if (slopeChanges > 48) {
    digitalWrite(ledPIN, HIGH); // Encender LED
  }
  else {
    digitalWrite(ledPIN, LOW); // Apagar LED
  }
}

void applyIIRFilter() {
  float xn = datos[ultPos]; //nuevo dato de entrada
  float yn = 0; //salida filtrada

  //para actualizar el buffer de entrada
  for (int i = order; i >= 1; i--) {
    x[i] = x[i-1];
  }
  x[0] = xn;

  //calcula salida filtrada
  for (int i = 0; i <= order; i++) {
    yn += b[i] * x[i];
  }

  for (int i = 1; i <= order; i++) {
    yn -= a[i] * y[i];
  }

  //para actualizar buffer de salida
  for (int i = order; i >= 1; i--) {
    y[i] = y[i-1];
  }
  y[0] = yn;

  //yn como la salida filtrada
  datos[ultPos] = static_cast<int>(yn); //reemplazar dato de entrada con salida filtrada
}

int calcularSlopeSignChanges() {
  int slopeChanges = 0;
  for (int i = 1; i < numDatos; i++) {
    int slope = datos[i] - datos[i - 1];
    if ((slope > 0 && datos[i] < 512) || (slope < 0 && datos[i] > 512)) {
      slopeChanges++;
    }
  }
  return slopeChanges;
}
