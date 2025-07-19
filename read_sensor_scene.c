/**
 * @file read_sensor_scene.c
 * @brief Read sensor scene implementation
 */

#include "read_sensor_scene.h"
#include "sensor.h"
#include "scenes.h"
#include <locale/locale.h>

/**
 * @brief Button callback for the READ button
 * 
 * @param result Button type that was pressed
 * @param type Input event type
 * @param context Application context
 */
static void dht11_read_sensor_button_callback(GuiButtonType result, InputType type, void* context) {
    DHT11App* app = context;
    
    if(type == InputTypePress && result == GuiButtonTypeCenter) {
        // Send custom event to scene manager to trigger sensor read
        scene_manager_handle_custom_event(app->scene_manager, 1);
    }
}

/**
 * @brief Convert temperature to display format based on system locale
 * 
 * Uses Flipper's built-in locale service to determine measurement units.
 * Automatically converts between Celsius and Fahrenheit based on user preference.
 * 
 * @param temp_celsius Temperature in Celsius
 * @param buffer Output buffer for formatted string
 * @param buffer_size Size of output buffer
 */
static void format_temperature(float temp_celsius, char* buffer, size_t buffer_size) {
    LocaleMeasurementUnits units = locale_get_measurement_unit();
    
    if(units == LocaleMeasurementUnitsImperial) {
        // Convert to Fahrenheit using the official locale API
        float temp_fahrenheit = locale_celsius_to_fahrenheit(temp_celsius);
        snprintf(buffer, buffer_size, "%.1f°F", (double)temp_fahrenheit);
    } else {
        // Use Celsius (metric units)
        snprintf(buffer, buffer_size, "%.1f°C", (double)temp_celsius);
    }
}

/**
 * @brief Calculate Heat Index using NOAA formula
 * 
 * Calculates the Heat Index (apparent temperature) using the NOAA formula.
 * The Heat Index is calculated for temperatures >= 80°F (26.7°C) and humidity >= 40%.
 * For lower values, returns the air temperature.
 * 
 * Formula: HI = -42.379 + 2.04901523*T + 10.14333127*RH - 0.22475541*T*RH 
 *              - 6.83783e-3*T² - 5.481717e-2*RH² + 1.22874e-3*T²*RH 
 *              + 8.5282e-4*T*RH² - 1.99e-6*T²*RH²
 * 
 * @param temp_celsius Temperature in Celsius
 * @param humidity_percent Relative humidity percentage
 * @return Heat Index in Celsius
 */
static float calculate_heat_index(float temp_celsius, float humidity_percent) {
    // Convert to Fahrenheit for calculation (NOAA formula uses Fahrenheit)
    float temp_fahrenheit = locale_celsius_to_fahrenheit(temp_celsius);
    
    // Only calculate Heat Index for temperatures >= 80°F and humidity >= 40%
    if(temp_fahrenheit < 80.0f || humidity_percent < 40.0f) {
        return temp_celsius; // Return air temperature
    }
    
    float T = temp_fahrenheit;
    float RH = humidity_percent;
    
    // NOAA Heat Index formula coefficients
    float c1 = -42.379f;
    float c2 = 2.04901523f;
    float c3 = 10.14333127f;
    float c4 = -0.22475541f;
    float c5 = -6.83783e-3f;
    float c6 = -5.481717e-2f;
    float c7 = 1.22874e-3f;
    float c8 = 8.5282e-4f;
    float c9 = -1.99e-6f;
    
    // Calculate Heat Index in Fahrenheit
    float HI_F = c1 + (c2 * T) + (c3 * RH) + (c4 * T * RH) + 
                 (c5 * T * T) + (c6 * RH * RH) + (c7 * T * T * RH) + 
                 (c8 * T * RH * RH) + (c9 * T * T * RH * RH);
    
    // Convert back to Celsius
    float HI_C = (HI_F - 32.0f) * 5.0f / 9.0f;
    
    return HI_C;
}

/**
 * @brief Update the sensor widget with current readings
 * 
 * Updates the widget display based on the current sensor state and readings.
 * 
 * @param app Application context
 */
static void dht11_read_sensor_update_widget(DHT11App* app) {
    widget_reset(app->sensor_widget);
    
    // Title - moved up to prevent clipping
    widget_add_string_element(app->sensor_widget, 25, 5, AlignLeft, AlignTop, FontPrimary, "DHT11 Sensor");
    
    if(app->sensor_ok && (app->temperature != 0.0f || app->humidity != 0.0f)) {
        // Show actual sensor readings when successful - left column
        widget_add_string_element(app->sensor_widget, 10, 18, AlignLeft, AlignTop, FontSecondary, "Temperature:");
        char temp_str[32];
        format_temperature(app->temperature, temp_str, sizeof(temp_str));
        widget_add_string_element(app->sensor_widget, 10, 28, AlignLeft, AlignTop, FontSecondary, temp_str);
        
        widget_add_string_element(app->sensor_widget, 10, 38, AlignLeft, AlignTop, FontSecondary, "Humidity:");
        char hum_str[32];
        snprintf(hum_str, sizeof(hum_str), "%.1f%%", (double)app->humidity);
        widget_add_string_element(app->sensor_widget, 10, 48, AlignLeft, AlignTop, FontSecondary, hum_str);
        
        // Calculate and display Heat Index - right column, centered with temp/humidity
        float heat_index = calculate_heat_index(app->temperature, app->humidity);
        widget_add_string_element(app->sensor_widget, 75, 28, AlignLeft, AlignTop, FontSecondary, "Heat Index:");
        char hi_str[32];
        format_temperature(heat_index, hi_str, sizeof(hi_str));
        widget_add_string_element(app->sensor_widget, 75, 38, AlignLeft, AlignTop, FontSecondary, hi_str);
    } else if(!app->sensor_ok && (app->temperature != 0.0f || app->humidity != 0.0f)) {
        // Show error when sensor reading fails
        widget_add_string_element(app->sensor_widget, 35, 25, AlignLeft, AlignTop, FontSecondary, "Sensor Error!");
        widget_add_string_element(app->sensor_widget, 15, 35, AlignLeft, AlignTop, FontSecondary, "Check connections in");
        widget_add_string_element(app->sensor_widget, 30, 45, AlignLeft, AlignTop, FontSecondary, "About section");
    } else {
        // Initial state - no reading yet
        widget_add_string_element(app->sensor_widget, 25, 30, AlignLeft, AlignTop, FontSecondary, "Press OK to read");
        widget_add_string_element(app->sensor_widget, 30, 40, AlignLeft, AlignTop, FontSecondary, "sensor data");
    }
    
    // Button hint - no change needed, it's at the bottom
    widget_add_button_element(app->sensor_widget, GuiButtonTypeCenter, "READ", dht11_read_sensor_button_callback, app);
}

void dht11_scene_read_sensor_on_enter(void* context) {
    DHT11App* app = context;
    
    dht11_read_sensor_update_widget(app);
    view_dispatcher_switch_to_view(app->view_dispatcher, DHT11SceneReadSensor);
}

bool dht11_scene_read_sensor_on_event(void* context, SceneManagerEvent event) {
    DHT11App* app = context;
    bool consumed = false;
    
    if(event.type == SceneManagerEventTypeCustom) {
        // Handle custom events from widget button callback
        app->sensor_ok = dht11_sensor_read(app);
        dht11_read_sensor_update_widget(app);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_previous_scene(app->scene_manager);
        consumed = true;
    }
    
    return consumed;
}

void dht11_scene_read_sensor_on_exit(void* context) {
    DHT11App* app = context;
    widget_reset(app->sensor_widget);
}
