/**
 * @file dht11.c
 * @brief DHT11 sensor application main entry point
 * @author Hypirae
 * @date 2025
 * @version 1.0.0
 * 
 * This is the main entry point for the DHT11 temperature and humidity
 * sensor application for Flipper Zero. It provides professional-grade
 * sensor readings with Heat Index calculation and comprehensive debugging.
 * 
 * Features:
 * - Real-time DHT11 sensor readings
 * - Heat Index calculation using NOAA formula
 * - Temperature unit localization (°C/°F)
 * - Comprehensive debug mode with microsecond timing analysis
 * - Professional UI with proper error handling
 * 
 * @see https://github.com/Hypirae/dht11
 */

#include "app.h"
#include "sensor.h"
#include "scenes.h"

/**
 * @brief Navigation event callback
 * 
 * Handles back navigation events from the view dispatcher.
 * 
 * @param context Application context
 * @return true if the event was handled
 */
static bool dht11_navigation_event_callback(void* context) {
    DHT11App* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

/**
 * @brief Custom event callback
 * 
 * Handles custom events from the view dispatcher.
 * 
 * @param context Application context
 * @param custom_event Custom event value
 * @return true if the event was handled
 */
static bool dht11_custom_event_callback(void* context, uint32_t custom_event) {
    DHT11App* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

/**
 * @brief Main application entry point
 * 
 * Initializes the DHT11 application, sets up the GPIO pin for the sensor,
 * and starts the scene manager with the main menu.
 * 
 * @param p Unused parameter (required by Flipper application interface)
 * @return Always returns 0
 */
int32_t dht11_app(void* p) {
    UNUSED(p);
    
    // Allocate application structure
    DHT11App* app = malloc(sizeof(DHT11App));
    
    // Initialize records
    app->gui = furi_record_open(RECORD_GUI);
    app->notifications = furi_record_open(RECORD_NOTIFICATION);
    
    // Initialize view dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, dht11_navigation_event_callback);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, dht11_custom_event_callback);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    
    // Initialize scene manager
    app->scene_manager = scene_manager_alloc(&dht11_scene_handlers, app);
    
    // Initialize GUI components
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(app->view_dispatcher, DHT11SceneMainMenu, submenu_get_view(app->submenu));
    
    app->sensor_widget = widget_alloc();
    view_dispatcher_add_view(app->view_dispatcher, DHT11SceneReadSensor, widget_get_view(app->sensor_widget));
    
    app->about_text_box = text_box_alloc();
    view_dispatcher_add_view(app->view_dispatcher, DHT11SceneAbout, text_box_get_view(app->about_text_box));
    
    app->debug_text_box = text_box_alloc();
    view_dispatcher_add_view(app->view_dispatcher, DHT11SceneDebug, text_box_get_view(app->debug_text_box));
    
    // Initialize sensor data
    app->temperature = 0.0f;
    app->humidity = 0.0f;
    app->sensor_ok = false;
    app->about_text = NULL;
    
    // Initialize and enable GPIO pin C0 for DHT11 with pull-up resistor
    furi_hal_gpio_init(DHT11_PIN, GpioModeInput, GpioPullUp, GpioSpeedLow);
    
    // Start with main menu scene
    scene_manager_next_scene(app->scene_manager, DHT11SceneMainMenu);
    
    // Run view dispatcher
    view_dispatcher_run(app->view_dispatcher);
    
    // Cleanup
    furi_assert(app);
    
    // Remove views from dispatcher
    view_dispatcher_remove_view(app->view_dispatcher, DHT11SceneMainMenu);
    view_dispatcher_remove_view(app->view_dispatcher, DHT11SceneReadSensor);
    view_dispatcher_remove_view(app->view_dispatcher, DHT11SceneAbout);
    view_dispatcher_remove_view(app->view_dispatcher, DHT11SceneDebug);
    
    // Free GUI components
    submenu_free(app->submenu);
    widget_free(app->sensor_widget);
    text_box_free(app->about_text_box);
    text_box_free(app->debug_text_box);
    
    // Free scene manager
    scene_manager_free(app->scene_manager);
    
    // Free view dispatcher
    view_dispatcher_free(app->view_dispatcher);
    
    // Free about text if allocated
    if(app->about_text) {
        free(app->about_text);
    }
    
    // Close records
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    
    free(app);
    
    return 0;
}
