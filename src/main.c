#include "mgos.h"
#include "mgos_gpio.h"
#include "mgos_modbus.h"
#include "mgos_timers.h"

static void print_buffer(struct mbuf buffer, uint8_t status) {
  char str[1024];
  int length = 0;
  for (int i = 0; i < buffer.len && i < sizeof(str) / 3; i++) {
    length += sprintf(str + length, "%.2x ", buffer.buf[i]);
  }
  if (status == RESP_SUCCESS) {
    LOG(LL_INFO, ("%f - VALID RESPONSE, Status: %d, Buffer: %.*s", mgos_uptime(), status, length, str));
  } else {
    LOG(LL_INFO, ("%f - Invalid response, Status: %d, Buffer: %.*s", mgos_uptime(), status, length, str));
  }
}

void mb_read_response_handler(uint8_t status, struct mb_request_info mb_ri, struct mbuf response, void* param) {
  print_buffer(response, status);
}

void button_handler(int pin, void* arg) {
  LOG(LL_INFO, ("Button Pressed, calling modbus"));
  mb_read_holding_registers(5, 100, 100, mb_read_response_handler, NULL);
}

void my_timer_cb(void* param) {
  // LOG(LL_INFO, ("TIMER STARTED - %f", mgos_uptime()));
  mb_read_holding_registers(5, 100, 100, mb_read_response_handler, NULL);
}

enum mgos_app_init_result mgos_app_init(void) {
  LOG(LL_INFO, ("Application init"));
  if (!mgos_modbus_connect()) {
    LOG(LL_INFO, ("Unable to connect MODBUS"));
  }
  mgos_gpio_set_button_handler(0, MGOS_GPIO_PULL_UP, MGOS_GPIO_INT_EDGE_NEG, 100, button_handler, NULL);
  mgos_set_timer(5000, MGOS_TIMER_REPEAT, my_timer_cb, NULL);
  return MGOS_APP_INIT_SUCCESS;
}