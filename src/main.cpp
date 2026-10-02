#define WDT_TIMEOUT 3  // Watchdog timeout in seconds
#define POT_PIN    35  
#define BUTTON_PIN 25  
#define LED    4       

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <esp_task_wdt.h> // ESP32 Task Watchdog Timer header


 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void setup() {
  Serial.begin(115200);
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW); // Ensure LED is off at startup
  Serial.println("\n--- ESP32 Watchdog Timer Demo ---");

  // Configure and initialize the Task Watchdog Timer (TWDT)
  /*esp_task_wdt_config_t twdt_config = {
      .timeout_ms = WDT_TIMEOUT * 1000, // Timeout in milliseconds (3000 ms)
      .idle_core_mask = (1 << portNUM_PROCESSORS) - 1, // Monitor idle tasks on both cores
      .trigger_panic = true            // Trigger hardware panic/reset on timeout
  };*/

  // Initialize WDT directly: 3-second timeout, panic enabled
  // It is directly done bcz the upper commented function is timed out in the libraries added

  // Parameter 1: Timeout in SECONDS (3 = 3 seconds)
  // Parameter 2: Panic flag (true = trigger hardware reset on timeout)
  esp_task_wdt_init(WDT_TIMEOUT, true);

  // Add the current task (loopTask running setup and loop) to the WDT watchdog list
  // Subscribe loopTask to watchdog
  // NULL here means the current task (loopTask) i.e the void loop() function is added to the watchdog list
  esp_task_wdt_add(NULL);

  Serial.println("System Initialized. Watchdog Timer Enabled.");
}


     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void loop() {
  // Read potentiometer value (0 to 4095 on ESP32 12-bit ADC)
  int potValue = analogRead(POT_PIN);
  potValue = map(potValue, 0, 4095, 0, 100); // Map to percentage (0-100)
  // Toggle LED state
  digitalWrite(LED, !digitalRead(LED));   // Read current LED state and write the opposite of it

  // Feed/Reset the ESP32 Task Watchdog Timer
  esp_task_wdt_reset();

  // FAULT INJECTION CHECK:
  if (digitalRead(BUTTON_PIN) == LOW) {
    Serial.println("FAULT INJECTED! Entering infinite blocking loop...");
    
    // Infinite loop: Stop calling esp_task_wdt_reset()
    while (true) {
      // The CPU is trapped here. The watchdog timer continues counting up.
      // After 3 seconds, the hardware WDT triggers an automatic hardware reset.
    }
  }

  // If no fault is injected, print the potentiometer value to the Serial Monitor
  Serial.print("Potentiometer Value: ");
  Serial.println(potValue);

  delay(1000);
}