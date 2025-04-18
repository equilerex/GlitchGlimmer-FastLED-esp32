#include <Arduino.h>
#include <TFT_eSPI.h>
#include "VerticalBarWidget.h"

// Helper functions that can be called from the main sketch
void checkHeapMemory() {
    Serial.printf("Free Heap: %d\n", ESP.getFreeHeap());
}

// You can add other helper functions here as needed
// These functions can be called from your main setup() or loop() in the .ino file