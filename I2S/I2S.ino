#include "AudioTools.h"
#include "BluetoothA2DPSink.h"

// ======================================================
// ======== CONFIGURAÇÃO DE HARDWARE E ÁUDIO ===========
// ======================================================

// --------- Pinos I2S ---------
const int I2S_BCK  = 26;  // Bit Clock
const int I2S_WS   = 27;  // Word Select (LRCK)
const int I2S_DATA = 25;  // Data Out


// --------- Controles ---------
const int btnPlay = 32;
const int btnNext = 33;
const int potPin  = 35;


// --------- Configuração de Áudio ---------

#define SAMPLE_RATE     44100
#define BITS_PER_SAMPLE 16

// ===== ESCOLHA DO MODO DE SAÍDA =====

//#define AUDIO_MONO      // ← Descomente para MONO
#define AUDIO_STEREO    // ← Descomente para ESTÉREO


// ======================================================
// ================= OBJETOS ============================
// ======================================================

I2SStream i2s;
BluetoothA2DPSink a2dp_sink(i2s);

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

  pinMode(btnPlay, INPUT_PULLUP);
  pinMode(btnNext, INPUT_PULLUP);


  // --------- Configuração I2S ---------
  auto config = i2s.defaultConfig(TX_MODE);

  config.sample_rate     = SAMPLE_RATE;
  config.bits_per_sample = BITS_PER_SAMPLE;

#ifdef AUDIO_MONO
  config.channels = 1;
  a2dp_sink.set_mono_downmix(true);
  Serial.println("Modo MONO ativado");
#endif

#ifdef AUDIO_STEREO
  config.channels = 2;
  a2dp_sink.set_mono_downmix(false);
  Serial.println("Modo ESTÉREO ativado");
#endif

  // --------- Pinos I2S explícitos ---------
  config.pin_bck  = I2S_BCK;
  config.pin_ws   = I2S_WS;
  config.pin_data = I2S_DATA;

  i2s.begin(config);

  a2dp_sink.set_volume(80);
  a2dp_sink.start("AudioMonitorBT_ESP32");

  Serial.println("Monitor de Audio I2S iniciado");
}


// ======================================================
// ================= LOOP ===============================
// ======================================================

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

  // ===== POTENCIÔMETRO =====
  int potValue = analogRead(potPin);
  int volume = map(potValue, 0, 4095, 0, 127);

  if (abs(volume - lastVolume) > 2) {
    a2dp_sink.set_volume(volume);
    lastVolume = volume;
    Serial.print("Volume: ");
    Serial.println(volume);
  }

  delay(10);
}