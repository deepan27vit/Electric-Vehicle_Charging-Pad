//E-vehicle cahrger

#include <stdint.h>
#include <stdbool.h>
#include "pwm.h"
#include "adc.h"
#include "gpio.h"
#include "uart.h"
#include "temp_sensor.h"

#define PWM_FREQ_KHZ 85
#define CURRENT_THRESHOLD 25   // Amps
#define TEMP_LIMIT_C     60    // Celsius

void system_init(void) {
    pwm_init(PWM_FREQ_KHZ * 1000);
    adc_init();
    gpio_init();
    uart_init(115200);
    temp_sensor_init();
}

void safety_shutdown() {
    pwm_disable();
    gpio_write(RELAY_PIN, 0);
    uart_println("!! System Shutdown due to Fault !!");
}

int main(void) {
    system_init();
    gpio_write(RELAY_PIN, 1); // Enable H-bridge

    while (1) {
        float coil_current = adc_read_current();  // ACS712
        float temp = temp_sensor_read();          // LM35 or thermistor

        uart_printf("Current: %.2f A, Temp: %.2f C\n", coil_current, temp);

        if (coil_current > CURRENT_THRESHOLD || temp > TEMP_LIMIT_C) {
            safety_shutdown();
        }

        delay_ms(500);
    }

    return 0;
}
