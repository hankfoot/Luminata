# Luminata

## Updating WebApp
Updating the WebApp's content (data folder) requires writing to the SPIFFS partition. Setting up a server for updating files is possible, but not straightforward and messy. Instead, we'll just use the traditional method of using the [https://github.com/me-no-dev/arduino-esp32fs-plugin](ESP32 Sketch Data Upload) in Arudio, however, this isn't supported in Arduino 2.x yet, so we'll have to open the project in Arduino 1.8.13 and do it there.
