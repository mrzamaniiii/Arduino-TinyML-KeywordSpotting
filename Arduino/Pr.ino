#include <Arduino.h>
#include <PDM.h>
#include <TensorFlowLite.h>
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "model.h"

#define MODEL_INPUT_SIZE 16000 
static int16_t audio_buffer_int[MODEL_INPUT_SIZE];
int audio_index = 0;

short sampleBuffer[256];
volatile int samplesRead = 0;

// Stable arena size for your model
constexpr int tensor_arena_size = 40 * 1024; 
uint8_t tensor_arena[tensor_arena_size];

tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;

const char* labels[] = {"Clap", "Snap", "Tap", "Background"};

void setup() {
  Serial.begin(115200);
  while (!Serial);
  delay(2000);

  Serial.println("--- Stable KWS System Start ---");

  // Fixed the type name here: MicroErrorReporter
  static tflite::MicroErrorReporter micro_error_reporter;
  const tflite::Model* model = tflite::GetModel(model_data);
  static tflite::AllOpsResolver resolver;

  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, tensor_arena_size, &micro_error_reporter);
  interpreter = &static_interpreter;

  if (interpreter->AllocateTensors() != kTfLiteOk) {
    Serial.println("Allocation Failed!");
    while (1);
  }

  input = interpreter->input(0);
  output = interpreter->output(0);

  PDM.onReceive(onPDMdata);
  if (!PDM.begin(1, 16000)) { 
    Serial.println("PDM Error!"); 
    while (1); 
  }
  PDM.setGain(127); 

  Serial.println("=== LISTENING STARTED ===");
}

void loop() {
  if (samplesRead > 0) {
    int readCount = samplesRead;
    samplesRead = 0;

    for (int i = 0; i < readCount; i++) {
      if (audio_index < MODEL_INPUT_SIZE) {
        audio_buffer_int[audio_index++] = sampleBuffer[i];
      }

      if (audio_index >= MODEL_INPUT_SIZE) {
        for (int j = 0; j < MODEL_INPUT_SIZE; j++) {
          input->data.f[j] = audio_buffer_int[j] / 32768.0f;
        }

        if (interpreter->Invoke() == kTfLiteOk) {
          float max_val = 0;
          int max_idx = -1;
          for (int k = 0; k < 4; k++) {
            if (output->data.f[k] > max_val) {
              max_val = output->data.f[k];
              max_idx = k;
            }
          }

          if (max_val > 0.60 && max_idx != 3) {
            Serial.print("RESULT -> ");
            Serial.print(labels[max_idx]);
            Serial.print(" ("); 
            Serial.print(max_val * 100); 
            Serial.println("%)");
          }
        }
        audio_index = 0; 
      }
    }
  }
}

void onPDMdata() {
  int bytesAvailable = PDM.available();
  PDM.read(sampleBuffer, bytesAvailable);
  samplesRead = bytesAvailable / 2;
}