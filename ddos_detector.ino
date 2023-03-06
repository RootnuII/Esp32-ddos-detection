#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#define THRESHOLD 100  // Set the threshold for what is considered normal traffic

AsyncWebServer server(80);  // Create a web server on port 80

void setup() {
  Serial.begin(115200);

  WiFi.begin("SSID", "password");  // Connect to Wi-Fi network

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }

  Serial.println("Connected to Wi-Fi");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello, world!");
  });

  server.begin();  // Start the server
}

void loop() {
  static int counter = 0;
  static int sum = 0;
  static float mean = 0;
  static float variance = 0;
  static float std_dev = 0;

  int request_size = server.client()->available();

  if (request_size > 0) {
    Serial.println("Incoming request detected");

    counter++;
    sum += request_size;
    mean = (float) sum / counter;

    variance += pow(request_size - mean, 2) / counter;
    std_dev = sqrt(variance);

    Serial.printf("Request size: %d\n", request_size);
    Serial.printf("Mean: %.2f\n", mean);
    Serial.printf("Standard deviation: %.2f\n", std_dev);

    if (counter >= THRESHOLD && request_size > (mean + (2 * std_dev))) {
      Serial.println("DDoS attack detected!");
      server.client()->stop();
      counter = 0;
      sum = 0;
      mean = 0;
      variance = 0;
      std_dev = 0;
      delay(1000);
    }
  }
}
