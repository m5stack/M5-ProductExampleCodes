#include <M5Stack.h>
#include "uart_frame.h"


typedef struct _JpegFrame_t {
  uint8_t *buf;
  uint32_t size;
} JpegFrame_t;


static QueueHandle_t jpeg_fream_queue = NULL;

void frame_recv_callback(int cmd, const uint8_t*buf, int len) {
  JpegFrame_t frame;
  frame.buf = (uint8_t *)malloc(sizeof(uint8_t) * len);
  memcpy(frame.buf, buf, len);
  frame.size = len;
  
  if (xQueueSend(jpeg_fream_queue, &frame, 0) != pdTRUE) {
    free(frame.buf);
  }
}

// the setup routine runs once when M5Stack starts up
void setup(){
  // Initialize the M5Stack object
  M5.begin(true, false, false, false);
  Serial.begin(1500000);
  uart_frame_init(36, 26, 1500000);
  jpeg_fream_queue = xQueueCreate(2, sizeof(JpegFrame_t));
}

// the loop routine runs over and over again forever

void loop() {
  JpegFrame_t frame;
  int time_last = 0;
  time_last = millis();
  xQueueReceive(jpeg_fream_queue, &frame, portMAX_DELAY);
  M5.Lcd.drawJpg(frame.buf, frame.size, 0, 0);
  Serial.printf("--%ld--size--%d--\r\n", millis() - time_last, frame.size);
  free(frame.buf);
}
