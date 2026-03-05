#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

AnalogAudioStream out;
BluetoothA2DPSink a2dp_sink(out);

// ===== Pinos =====
const int btnPlay = 32;
const int btnNext = 33;
const int potPin  = 35;

// ===== Controle =====
bool isPlaying = true;
int lastVolume = -1;

unsigned long lastDebouncePlay = 0;
unsigned long lastDebounceNext = 0;
const unsigned long debounceDelay = 200;

void setup() {
  Serial.begin(115200);

  pinMode(btnPlay, INPUT_PULLUP);
  pinMode(btnNext, INPUT_PULLUP);

  a2dp_sink.set_mono_downmix(true);
  a2dp_sink.set_volume(80);
  a2dp_sink.start("AudioMonitorBT_ESP32");

  Serial.println("Monitor de Audio iniciado");
}

void loop() {

  // ===== BOTÃO PLAY / PAUSE =====
  if (digitalRead(btnPlay) == LOW) {
    if (millis() - lastDebouncePlay > debounceDelay) {
      if (isPlaying) {
        a2dp_sink.pause();
        Serial.println("Pause");
      } else {
        a2dp_sink.play();
        Serial.println("Play");
      }
      isPlaying = !isPlaying;
      lastDebouncePlay = millis();
    }
  }

  // ===== BOTÃO NEXT =====
  if (digitalRead(btnNext) == LOW) {
    if (millis() - lastDebounceNext > debounceDelay) {
      a2dp_sink.next();
      Serial.println("Next track");
      lastDebounceNext = millis();
    }
  }

  // ===== POTENCIÔMETRO (VOLUME) =====
  int potValue = analogRead(potPin);     // 0–4095
  int volume = map(potValue, 0, 4095, 0, 127);

  /* DEBUG ADC 
  Serial.println(analogRead(potPin));
  */

  // ===== ATUALIZA VOLUME =====
  if (abs(volume - lastVolume) > 2) {
    a2dp_sink.set_volume(volume);
    lastVolume = volume;
    Serial.print("Volume: ");
    Serial.println(volume);
  }

  delay(10);
}