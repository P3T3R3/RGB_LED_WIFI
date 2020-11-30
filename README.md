# RGB_LED_WIFI
Version 1.5.0
Main changes:
-improved stability a lot
-fully asynchronous
-changed webclient color wheel to hsl sliders
-some code refactor

Full changelog:
-changed Adafruit Neopixel library to NeoPixelBus (C) by Makuna
-new library allowed to change method of communication with leds to Neo800kbs that doesn't use interrupts, which has caused watchdog reset frequently
-cleaned and optimized code
-credentials and sensitive information saved in credentials.h
