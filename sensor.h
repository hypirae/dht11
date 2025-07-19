/**
 * @file sensor.h
 * @brief DHT11 sensor driver interface
 * 
 * This file contains the interface for reading data from the DHT11
 * temperature and humidity sensor.
 */

#pragma once

#include <furi_hal_gpio.h>
#include "app.h"

/** @brief GPIO pin connected to DHT11 data line */
#define DHT11_PIN &gpio_ext_pc0

/**
 * @brief Read temperature and humidity from DHT11 sensor
 * 
 * Performs a standard sensor reading operation and updates the app's
 * temperature and humidity values if successful.
 * 
 * @param app Pointer to the application instance
 * @return true if reading was successful, false otherwise
 */
bool dht11_sensor_read(DHT11App* app);

/**
 * @brief Read sensor with detailed debug logging
 * 
 * Performs a sensor reading operation with extensive debug logging
 * for troubleshooting purposes. Updates the app's debug_log buffer
 * with detailed timing and protocol information.
 * 
 * @param app Pointer to the application instance
 * @return true if reading was successful, false otherwise
 */
bool dht11_sensor_debug_read(DHT11App* app);
