/**
 * @file app.h
 * @brief DHT11 Temperature and Humidity Sensor Application
 * 
 * This file contains the main application structure and scene definitions
 * for the DHT11 sensor application on Flipper Zero.
 */

#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_box.h>
#include <gui/modules/widget.h>
#include <input/input.h>
#include <notification/notification_messages.h>

/**
 * @brief Application scene enumeration
 * 
 * Defines all available scenes in the DHT11 application.
 * The count must be last for proper scene manager initialization.
 */
typedef enum {
    DHT11SceneMainMenu,     /**< Main menu scene */
    DHT11SceneReadSensor,   /**< Sensor reading scene */
    DHT11SceneAbout,        /**< About/help scene */
    DHT11SceneDebug,        /**< Debug output scene */
    DHT11SceneCount,        /**< Total number of scenes */
} DHT11Scene;

/**
 * @brief Main menu items enumeration
 * 
 * Defines the available menu items in the main menu.
 */
typedef enum {
    DHT11MainMenuIndexReadSensor,   /**< Read sensor menu item */
    DHT11MainMenuIndexAbout,        /**< About menu item */
    DHT11MainMenuIndexDebug,        /**< Debug menu item */
} DHT11MainMenuIndex;

/**
 * @brief Main application structure
 * 
 * Contains all GUI components, sensor data, and application state.
 */
typedef struct {
    Gui* gui;                           /**< GUI instance */
    ViewDispatcher* view_dispatcher;    /**< View dispatcher for scene management */
    SceneManager* scene_manager;        /**< Flipper's scene manager */
    
    // GUI Views
    Submenu* submenu;                   /**< Main menu submenu */
    Widget* sensor_widget;              /**< Sensor reading widget */
    TextBox* about_text_box;            /**< About screen text box */
    TextBox* debug_text_box;            /**< Debug output text box */
    
    NotificationApp* notifications;     /**< Notification service */
    
    // Sensor data
    float temperature;                  /**< Last temperature reading in Celsius */
    float humidity;                     /**< Last humidity reading in percentage */
    bool sensor_ok;                     /**< Flag indicating last sensor read status */
    char debug_log[2048];              /**< Buffer for debug output */
    char* about_text;                   /**< About screen text content */
} DHT11App;

// Function declarations  
DHT11App* app_alloc(void);
void app_free(DHT11App* app);