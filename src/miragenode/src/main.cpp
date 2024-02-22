/* 
 * Main file for esp32 based LED control node
 * Author: Justin Martin, jcm-art
 * 
 * This file is the main entry point for the esp32 based LED control node. This 
 * node is responsible for generating lighting patterns on attached addressable
 * LEDs.
 * 
 */

#include <Arduino.h>
#include <ArduinoOTA.h>

#include <ArtnetWifi.h>
#include <FastLED.h> // Include the Arduino FastLED library


// LED Configuration Parameters
// Set LED_BUILTIN if it is not defined by Arduino framework
#ifndef LED_BUILTIN
    #define LED_BUILTIN 2
#endif
// Define Addressable LED parameters
#define ADDRESSABLE_PIN_1 13
#define NUM_FIXTURES 1
#define NUM_LEDS_PER_FIXTURE 15
CRGB leds[NUM_FIXTURES][NUM_LEDS_PER_FIXTURE];
// Define limits for LED output based on device capabilities
#define MAX_Current 900
#define CURRENT_PER_LED 60
#define MAX_BRIGHTNESS 255 // Max brightness required due to current limitations of prototype board

// Initialize LED pin list and state information
constexpr int addressable_pins[] = {ADDRESSABLE_PIN_1};
float output_current = 0.0;
int serial_counter = 0;
int effect_counter = 0;

// Define Serial Output Parameters
#define SERIAL_INTERVAL 300

// WiFi Configuration Parameters
const char* ssid = "TBD";
const char* password = "TBD";

// Artnet settings
ArtnetWifi artnet;
const int startUniverse = 0; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.
const char host[] = "192.168.86.37"; // CHANGE FOR YOUR SETUP your destination
const int numberOfChannels = NUM_FIXTURES * NUM_LEDS_PER_FIXTURE * 3;
// Check that all universes received
const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
bool universesReceived[maxUniverses];
bool sendFrame = 1;

// Function declarations
void connect_to_wifi();
void initialize_arduino_OTA();
void initialize_serial_comms();
void initialize_artnet();
void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data);
void initialize_addressable_LED_pins(); 
void set_led(int strip_id, int led_id, int r, int g, int b);
void check_max_current();
void addressable_off();
void vertical_traveling_led(int num_iterations, int delay_time_between_patterns, int delay_time_between_leds);
void run_next_pattern();

void setup() {
  /*
  * Setup function for esp32 based LED control node, run on boot.
  */

  // Initialize communications
  initialize_serial_comms();
  connect_to_wifi();
  initialize_arduino_OTA();

  // Initialize Artnet connection to host
  initialize_artnet();

  // Initialize built in LED digital pin (on board) as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  // Set pin initial conditions 
  initialize_addressable_LED_pins();

  // Perform lighting initialization test
  Serial.println("Running lighting initialization test");
  run_next_pattern();

}

void loop() {
  /*
  * Main loop function for esp32 based LED control node.
  */

  // Check OTA for update
  ArduinoOTA.handle();

  // Handle Artnet
  uint16_t read_value = artnet.read();
  Serial.print("Read value: ");
  Serial.println(read_value);

  // Perform Lighting Functions
  // run_next_pattern();
}

void initialize_serial_comms() {
  /*
  * Function to initialize serial communication for debugging.
  */

  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("Booting");
}

void connect_to_wifi() {
  Serial.println("Connecting to wifi");
  WiFi.mode(WIFI_STA);
  delay(1000);
  WiFi.begin(ssid, password);
  delay(1000);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
}

void initialize_arduino_OTA() {
  /*
  * Function to initialize Arduino OTA for remote flashing of the esp32.
  */

  Serial.println("Initializing Arduino OTA for remote flashing");
  // Initialize Arduino OTA for remote flashing
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void initialize_artnet() {
  /*
  * Function to initialize Artnet communication for receiving lighting data.
  */

  Serial.println("Initializing Artnet communication");
  // Initialize Artnet communication
  artnet.begin(host);
  artnet.setLength(3);
  artnet.setUniverse(startUniverse);

  // TODO(jcm-art): confirm memset function
  memset(universesReceived, 0, maxUniverses);
  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);

}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  sendFrame = 1;
  // set brightness of the whole strip
  if (universe == 15)
  {
    FastLED.setBrightness(data[0]);
    FastLED.show();
  }

  // range check
  if (universe < startUniverse)
  {
    return;
  }
  uint8_t index = universe - startUniverse;
  if (index >= maxUniverses)
  {
    return;
  }

  // Store which universe has got in
  universesReceived[index] = true;

  for (int i = 0 ; i < maxUniverses ; i++)
  {
    if (!universesReceived[i])
    {
      sendFrame = 0;
      break;
    }
  }

  // TODO(jcm-art): fix for multiple strips
  // read universe and put into the right part of the display buffer
  for (int i = 0; i < length / 3; i++)
  {
    int led = i + (index * 170);
    if (led < NUM_LEDS_PER_FIXTURE)
    {
      leds[0][led] = CRGB(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
    }
  }

  if (sendFrame)
  {
    FastLED.show();
    // Reset universeReceived to 0
    memset(universesReceived, 0, maxUniverses);
  }
}

void initialize_addressable_LED_pins() {
  /*
  * Function to initialize addressable LED pins for use with FastLED library.
  */

  // Initialize addressable LED pins for use with FastLED library
  // Set the LED type and number of LEDs
  FastLED.addLeds<WS2812B, ADDRESSABLE_PIN_1, GRB>(leds[0], NUM_LEDS_PER_FIXTURE).setCorrection(TypicalLEDStrip);
  FastLED.clear();
}

void set_led(int strip_id, int led_id, int r, int g, int b){
  /*
  * Function to set the color of an individual LED in the LED array.
  */
  // Get prior value and update current
  int prior_val = leds[strip_id][led_id].r + leds[strip_id][led_id].g + leds[strip_id][led_id].b;
  output_current += (float((r + g + b) - prior_val))/(3*255) * CURRENT_PER_LED;

  // Print current value to serial bus
  // TODO(jcm-art): Implement dedicated logging function
  if (serial_counter % SERIAL_INTERVAL == 0){
    Serial.println("Current is " + String(output_current) + " vs. max of " + String(MAX_Current));
    serial_counter = 0;
  }
  serial_counter = serial_counter + 1;

  // Set new LED value
  leds[strip_id][led_id] = CRGB(r, g, b);


}

void check_max_current(){
  /*
  * Function to check if the current output is within the maximum allowable current.
  */

  // Check if current is over limit
  if (output_current > MAX_Current){
    float adjusted_current = 0.0;
    float current_ratio = MAX_Current / output_current;

    // Adjust all LED values to fit within current limit
    for (int i = 0; i< NUM_FIXTURES; i = i + 1) {
      for (int j = 0; j < NUM_LEDS_PER_FIXTURE; j = j + 1) {
        CRGB prior_color = leds[i][j];
        int adj_r = int(prior_color.r*current_ratio);
        int adj_g = int(prior_color.g*current_ratio);
        int adj_b = int(prior_color.b*current_ratio);
        leds[i][j] = CRGB(adj_r, adj_g, adj_b);
        adjusted_current+=(adj_r+adj_b+adj_g)/(3*255)*CURRENT_PER_LED;
      }
    }

    // Update output current
    output_current = adjusted_current;
  }

}

void addressable_off(){
  for (byte i = 0; i < NUM_FIXTURES ; i = i + 1) {
    for (byte j = 0; j < NUM_LEDS_PER_FIXTURE; j = j + 1) {
      set_led(i, j, 0, 0, 0);
    }
  }
  FastLED.show();
}

void vertical_traveling_led(int num_iterations, int delay_time_between_patterns, int delay_time_between_leds) {
  /*
  * Function to run a traveling LED animation on the addressable LED strips.
  */
  // Run traveling LED animation n times
  for (int n = 0; n < num_iterations; n = n + 1) {
    int counter = 0;
    // Iterate through all fixtures
    while (counter < NUM_LEDS_PER_FIXTURE) {
      // Set all LEDs equal to 0
      for (int i = 0; i< NUM_FIXTURES; i = i + 1){

        for (int j = 0; j < NUM_LEDS_PER_FIXTURE; j = j + 1) {
          set_led(i, j, 0, 0, 0);
        }

        // Set counter value to MAX_BRIGHTNESS
        set_led(i, counter, MAX_BRIGHTNESS, 0, 0);
      }
      FastLED.show();
      counter = counter + 1;
      delay(delay_time_between_leds);
    }
    delay(delay_time_between_patterns);
  }
}

void run_next_pattern() {
  // Turn on LED on board
  digitalWrite(LED_BUILTIN, HIGH);
  switch (effect_counter) {
    case 0:
      vertical_traveling_led(2, 10, 50);
      addressable_off();
      effect_counter+=1;
      break;
    case 1:
      delay(1000);
      addressable_off();
      effect_counter=0;
      break;
  }

  // Turn off LED on board
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}