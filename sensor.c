/**
 * @file sensor.c
 * @brief DHT11 sensor driver implementation
 * @author Hypirae
 * @date 2025
 * @version 1.0.0
 * 
 * Professional DHT11 sensor driver with high-precision timing using DWT cycle counter.
 * Implements the complete DHT11 communication protocol with comprehensive error handling
 * and debug logging capabilities.
 * 
 * Key features:
 * - Microsecond-precision timing using DWT cycle counter
 * - Comprehensive error handling and validation
 * - Debug mode with detailed protocol analysis
 * - Temperature range validation and checksum verification
 * 
 * @see https://github.com/Hypirae/dht11
 */

#include "sensor.h"
#include <furi_hal.h>

bool dht11_sensor_read(DHT11App* app) {
    uint8_t data[5] = {0};
    uint32_t timeout = 0;
    
    // Flash blue LED to indicate sensor reading
    notification_message(app->notifications, &sequence_blink_start_blue);
    
    // DHT11 requires at least 1s between readings
    // Send start signal: pull low for at least 18ms
    furi_hal_gpio_init(DHT11_PIN, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(DHT11_PIN, false);
    furi_delay_ms(20); // 20ms low to ensure proper start signal
    
    // Critical: Disable interrupts during timing-sensitive communication
    FURI_CRITICAL_ENTER();
    
    // Pull high for 20-40us then release to input mode
    furi_hal_gpio_write(DHT11_PIN, true);
    furi_delay_us(30); // 30us high
    
    // Switch to input mode with pull-up
    furi_hal_gpio_init(DHT11_PIN, GpioModeInput, GpioPullUp, GpioSpeedLow);
    
    // DHT11 response sequence:
    // 1. DHT11 pulls low for 80us
    timeout = 0;
    while(furi_hal_gpio_read(DHT11_PIN) && timeout < 200) {
        furi_delay_us(1);
        timeout++;
    }
    if(timeout >= 200) {
        FURI_CRITICAL_EXIT();
        notification_message(app->notifications, &sequence_blink_stop);
        return false; // No response from DHT11
    }
    
    // 2. DHT11 pulls high for 80us
    timeout = 0;
    while(!furi_hal_gpio_read(DHT11_PIN) && timeout < 200) {
        furi_delay_us(1);
        timeout++;
    }
    if(timeout >= 200) {
        FURI_CRITICAL_EXIT();
        notification_message(app->notifications, &sequence_blink_stop);
        return false; // Invalid response
    }
    
    // 3. Wait for end of response high period
    timeout = 0;
    while(furi_hal_gpio_read(DHT11_PIN) && timeout < 200) {
        furi_delay_us(1);
        timeout++;
    }
    if(timeout >= 200) {
        FURI_CRITICAL_EXIT();
        notification_message(app->notifications, &sequence_blink_stop);
        return false; // Response too long
    }
    
    // Read 40 bits of data (5 bytes)
    for(int i = 0; i < 40; i++) {
        // Wait for bit start (low period ~50us)
        timeout = 0;
        while(!furi_hal_gpio_read(DHT11_PIN) && timeout < 200) {
            furi_delay_us(1);
            timeout++;
        }
        if(timeout >= 200) {
            FURI_CRITICAL_EXIT();
            notification_message(app->notifications, &sequence_blink_stop);
            return false;
        }
        
        // Measure high period using DWT cycle counter for microsecond precision
        // Enable DWT cycle counter if not already enabled
        if(!(DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk)) {
            DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
        }
        
        uint32_t start_cycles = DWT->CYCCNT;
        while(furi_hal_gpio_read(DHT11_PIN)) {
            uint32_t elapsed_cycles = DWT->CYCCNT - start_cycles;
            // Flipper Zero runs at 64MHz, so 64 cycles = 1μs
            uint32_t elapsed_us = elapsed_cycles / 64;
            if(elapsed_us > 200) break; // 200μs timeout
        }
        uint32_t end_cycles = DWT->CYCCNT;
        uint32_t pulse_duration_us = (end_cycles - start_cycles) / 64;
        
        // Bit value based on high period duration
        // For DHT11: Logic '1' is ~70us, Logic '0' is ~26-28us
        if(pulse_duration_us > 40) { // Logic '1' (~70us) with proper threshold
            uint8_t byte_idx = i / 8;
            uint8_t bit_idx = 7 - (i % 8);
            data[byte_idx] |= (1 << bit_idx);
        }
        // Logic '0' (~26-28us) - bit remains 0 (no action needed)
    }
    
    FURI_CRITICAL_EXIT();
    
    // Verify checksum
    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    if(checksum != data[4]) {
        // Turn off LED and return false for checksum error
        notification_message(app->notifications, &sequence_blink_stop);
        return false; // Checksum mismatch
    }
    
    // Convert data according to DHT11 format
    // DHT11 provides integer values only, fractional bytes should be 0
    app->humidity = (float)data[0];
    app->temperature = (float)data[2];
    
    // Handle negative temperatures (if MSB of data[2] is set)
    if(data[2] & 0x80) {
        app->temperature = -((float)(data[2] & 0x7F));
    }
    
    // Validate reasonable ranges
    if(app->humidity > 100.0f || app->temperature > 60.0f || app->temperature < -40.0f) {
        // Turn off LED and return false for range error
        notification_message(app->notifications, &sequence_blink_stop);
        return false; // Values out of reasonable range
    }
    
    // Turn off LED - successful read
    notification_message(app->notifications, &sequence_blink_stop);
    return true;
}

// DHT11 debug sensor reading function with detailed logging
bool dht11_sensor_debug_read(DHT11App* app) {
    uint8_t data[5] = {0};
    uint32_t timeout = 0;
    char temp_msg[128];
    
    // Clear debug log
    app->debug_log[0] = '\0';
    size_t log_pos = 0;
    
    // Start logging
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "=== DHT11 Debug Log ===\n");
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "Pin: C0 (GPIO 16)\n\n");
    
    // Flash blue LED to indicate sensor reading
    notification_message(app->notifications, &sequence_blink_start_blue);
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "1. LED: Blue flash started\n");
    
    // Initial pin state check
    snprintf(temp_msg, sizeof(temp_msg), "2. Initial pin state: %s\n", 
             furi_hal_gpio_read(DHT11_PIN) ? "HIGH" : "LOW");
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "%s", temp_msg);
    
    // Send start signal: pull low for at least 18ms
    furi_hal_gpio_init(DHT11_PIN, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(DHT11_PIN, false);
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "3. Start signal: Pin LOW for 20ms\n");
    furi_delay_ms(20);
    
    // Critical: Disable interrupts during timing-sensitive communication
    FURI_CRITICAL_ENTER();
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "4. Critical section: Interrupts disabled\n");
    
    // Pull high for 20-40us then release to input mode
    furi_hal_gpio_write(DHT11_PIN, true);
    furi_delay_us(30);
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "5. Release signal: Pin HIGH for 30us\n");
    
    // Switch to input mode with pull-up
    furi_hal_gpio_init(DHT11_PIN, GpioModeInput, GpioPullUp, GpioSpeedLow);
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "6. Input mode: Pull-up enabled\n");
    
    // DHT11 response sequence:
    // 1. DHT11 pulls low for 80us
    timeout = 0;
    while(furi_hal_gpio_read(DHT11_PIN) && timeout < 200) {
        furi_delay_us(1);
        timeout++;
    }
    snprintf(temp_msg, sizeof(temp_msg), "7. Wait for LOW: %lums timeout=%d\n", (unsigned long)timeout, timeout >= 200 ? 1 : 0);
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "%s", temp_msg);
    
    if(timeout >= 200) {
        FURI_CRITICAL_EXIT();
        notification_message(app->notifications, &sequence_blink_stop);
        log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "ERROR: No response from DHT11\n");
        log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "Check: VCC->3.3V, GND->GND, DATA->C0\n");
        return false;
    }
    
    // 2. DHT11 pulls high for 80us
    timeout = 0;
    while(!furi_hal_gpio_read(DHT11_PIN) && timeout < 200) {
        furi_delay_us(1);
        timeout++;
    }
    snprintf(temp_msg, sizeof(temp_msg), "8. Response LOW: %lums\n", (unsigned long)timeout);
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "%s", temp_msg);
    
    if(timeout >= 200) {
        FURI_CRITICAL_EXIT();
        notification_message(app->notifications, &sequence_blink_stop);
        log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "ERROR: Invalid response timing\n");
        return false;
    }
    
    // 3. Wait for end of response high period
    timeout = 0;
    while(furi_hal_gpio_read(DHT11_PIN) && timeout < 200) {
        furi_delay_us(1);
        timeout++;
    }
    snprintf(temp_msg, sizeof(temp_msg), "9. Response HIGH: %lums\n", (unsigned long)timeout);
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "%s", temp_msg);
    
    if(timeout >= 200) {
        FURI_CRITICAL_EXIT();
        notification_message(app->notifications, &sequence_blink_stop);
        log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "ERROR: Response too long\n");
        return false;
    }
    
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "10. Data transmission started\n");
    
    // Read 40 bits of data (5 bytes)
    uint8_t bits_read = 0;
    for(int i = 0; i < 40; i++) {
        // Wait for bit start (low period ~50us)
        timeout = 0;
        while(!furi_hal_gpio_read(DHT11_PIN) && timeout < 200) {
            furi_delay_us(1);
            timeout++;
        }
        if(timeout >= 200) {
            FURI_CRITICAL_EXIT();
            notification_message(app->notifications, &sequence_blink_stop);
            snprintf(temp_msg, sizeof(temp_msg), "ERROR: Bit %d start timeout\n", i);
            log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "%s", temp_msg);
            return false;
        }
        
        // Measure high period using DWT cycle counter for microsecond precision
        // Enable DWT cycle counter if not already enabled
        if(!(DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk)) {
            DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
        }
        
        uint32_t start_cycles = DWT->CYCCNT;
        while(furi_hal_gpio_read(DHT11_PIN)) {
            uint32_t elapsed_cycles = DWT->CYCCNT - start_cycles;
            // Flipper Zero runs at 64MHz, so 64 cycles = 1μs
            uint32_t elapsed_us = elapsed_cycles / 64;
            if(elapsed_us > 200) break; // 200μs timeout
        }
        uint32_t end_cycles = DWT->CYCCNT;
        uint32_t pulse_duration_us = (end_cycles - start_cycles) / 64; // Convert cycles to μs
        
        // Bit value based on high period duration
        // For DHT11: Logic '1' is ~70us, Logic '0' is ~26-28us  
        // Use cycle-based measurement for accurate timing
        bool bit_value = pulse_duration_us > 40;
        if(bit_value) {
            uint8_t byte_idx = i / 8;
            uint8_t bit_idx = 7 - (i % 8);
            data[byte_idx] |= (1 << bit_idx);
        }
        // Logic '0' (~26-28us) - bit remains 0 (no action needed)
        bits_read++;
        
        // Log ALL bits to understand timing patterns - log every bit for first 16
        if(i < 16) { // Log first 16 bits to see timing patterns
            snprintf(temp_msg, sizeof(temp_msg), "Bit %d: %lums = %d (th:40)\n", i, (unsigned long)pulse_duration_us, bit_value ? 1 : 0);
            log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "%s", temp_msg);
        } else if((i % 8) == 7) { // Every 8th bit after that
            snprintf(temp_msg, sizeof(temp_msg), "Bit %d: %lums = %d\n", i, (unsigned long)pulse_duration_us, bit_value ? 1 : 0);
            log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "%s", temp_msg);
        }
    }
    
    FURI_CRITICAL_EXIT();
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "11. Critical section: Interrupts enabled\n");
    
    snprintf(temp_msg, sizeof(temp_msg), "12. Bits read: %d/40\n", bits_read);
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "%s", temp_msg);
    
    // Add timing analysis
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "Timing Analysis:\n");
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "- Using DWT cycle counter (64MHz = 1us)\n");
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "- Current threshold: 40us\n");
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "- Expected: 0=26-28us, 1=70us\n");
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "- High precision cycle counting\n");
    
    // Show raw data
    snprintf(temp_msg, sizeof(temp_msg), "13. Raw data: %02X %02X %02X %02X %02X\n", 
             data[0], data[1], data[2], data[3], data[4]);
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "%s", temp_msg);
    
    // Verify checksum
    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    snprintf(temp_msg, sizeof(temp_msg), "14. Checksum calc: %02X, received: %02X\n", checksum, data[4]);
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "%s", temp_msg);
    
    if(checksum != data[4]) {
        notification_message(app->notifications, &sequence_blink_stop);
        log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "ERROR: Checksum mismatch\n");
        return false;
    }
    
    // Convert data
    app->humidity = (float)data[0];
    app->temperature = (float)data[2];
    
    snprintf(temp_msg, sizeof(temp_msg), "15. Humidity: %.1f%%\n", (double)app->humidity);
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "%s", temp_msg);
    snprintf(temp_msg, sizeof(temp_msg), "16. Temperature: %.1f°C\n", (double)app->temperature);
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "%s", temp_msg);
    
    // Validate ranges
    if(app->humidity > 100.0f || app->temperature > 60.0f || app->temperature < -40.0f) {
        notification_message(app->notifications, &sequence_blink_stop);
        log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "ERROR: Values out of range\n");
        return false;
    }
    
    notification_message(app->notifications, &sequence_blink_stop);
    log_pos += snprintf(app->debug_log + log_pos, sizeof(app->debug_log) - log_pos, "17. SUCCESS: Read completed\n");
    return true;
}
