#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

// ======================================================
// =============== CONFIGURAÇÃO GERAL ===================
// ======================================================

// --------- Pinos de Controle ---------
const int BTN_PLAY = 32;
const int BTN_NEXT = 33;
const int POT_PIN  = 35;

// --------- Parâmetros de Áudio ---------
#define SAMPLE_RATE     44100
#define BITS_PER_SAMPLE 16

// ===== ESCOLHA DO MODO DE SAÍDA =====

#define AUDIO_MONO     // ← Descomente para MONO
//#define AUDIO_STEREO    // ← Descomente para ESTÉREO


// ======================================================
// ================= OBJETOS ============================
// ======================================================

// DAC interno do ESP32 (GPIO25 ou GPIO26)
AnalogAudioStream analog_out;
BluetoothA2DPSink a2dp_sink(analog_out);


// ======================================================
// ================= CONTROLE ===========================
// ======================================================

bool isPlaying = true;
int lastVolume = -1;

unsigned long lastDebouncePlay = 0;
unsigned long lastDebounceNext = 0;
const unsigned long debounceDelay = 200;


// ======================================================
// ================= SETUP ==============================
// ======================================================

void setup() {

  Serial.begin(115200);

  pinMode(BTN_PLAY, INPUT_PULLUP);
  pinMode(BTN_NEXT, INPUT_PULLUP);

  // --------- Configuração do DAC Interno ---------
  auto config = analog_out.defaultConfig();
  config.sample_rate     = SAMPLE_RATE;
  config.bits_per_sample = BITS_PER_SAMPLE;

#ifdef AUDIO_MONO
  config.channels = 1;
  a2dp_sink.set_mono_downmix(true);
  Serial.println("Modo MONO (DAC interno)");
#endif

#ifdef AUDIO_STEREO
  config.channels = 2;
  a2dp_sink.set_mono_downmix(false);
  Serial.println("Modo ESTÉREO (DAC interno)");
#endif

  analog_out.begin(config);

  // --------- Configuração Bluetooth ---------
  a2dp_sink.set_volume(80);
  a2dp_sink.start("AudioMonitorBT_ESP32");

  Serial.println("Monitor de Audio via DAC interno iniciado");
}


// ======================================================
// ================= LOOP ===============================
// ======================================================

void loop() {

  // ===== BOTÃO PLAY / PAUSE =====
  if (digitalRead(BTN_PLAY) == LOW) {
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
  if (digitalRead(BTN_NEXT) == LOW) {
    if (millis() - lastDebounceNext > debounceDelay) {
      a2dp_sink.next();
      Serial.println("Next track");
      lastDebounceNext = millis();
    }
  }

  // ===== POTENCIÔMETRO (VOLUME) =====
  int potValue = analogRead(POT_PIN);     // 0–4095
  int volume = map(potValue, 0, 4095, 0, 127);

  /* ===== DEBUG ADC =====
  Serial.println(potValue);
  */

  // ===== ATUALIZA VOLUME (evita ruído excessivo) =====
  if (abs(volume - lastVolume) > 2) {
    a2dp_sink.set_volume(volume);
    lastVolume = volume;

    Serial.print("Volume: ");
    Serial.println(volume);
  }

  delay(10);
}