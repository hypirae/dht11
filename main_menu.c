/**
 * @file main_menu.c
 * @brief Main menu scene implementation
 */

#include "app.h"
#include "main_menu.h"
#include "scenes.h"

/**
 * @brief Main menu callback handler
 * 
 * Handles submenu item selection and navigates to the appropriate scene.
 * 
 * @param context Application context
 * @param index Selected menu item index
 */
static void dht11_main_menu_callback(void* context, uint32_t index) {
    DHT11App* app = context;
    
    switch(index) {
    case DHT11MainMenuIndexReadSensor:
        scene_manager_next_scene(app->scene_manager, DHT11SceneReadSensor);
        break;
    case DHT11MainMenuIndexAbout:
        scene_manager_next_scene(app->scene_manager, DHT11SceneAbout);
        break;
    case DHT11MainMenuIndexDebug:
        scene_manager_next_scene(app->scene_manager, DHT11SceneDebug);
        break;
    }
}

void dht11_scene_main_menu_on_enter(void* context) {
    DHT11App* app = context;
    
    submenu_reset(app->submenu);
    submenu_add_item(app->submenu, "Read Sensor", DHT11MainMenuIndexReadSensor, dht11_main_menu_callback, app);
    submenu_add_item(app->submenu, "About", DHT11MainMenuIndexAbout, dht11_main_menu_callback, app);
    submenu_add_item(app->submenu, "Debug", DHT11MainMenuIndexDebug, dht11_main_menu_callback, app);
    
    view_dispatcher_switch_to_view(app->view_dispatcher, DHT11SceneMainMenu);
}

bool dht11_scene_main_menu_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void dht11_scene_main_menu_on_exit(void* context) {
    DHT11App* app = context;
    submenu_reset(app->submenu);
}