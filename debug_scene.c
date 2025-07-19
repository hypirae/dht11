/**
 * @file debug_scene.c
 * @brief Debug scene implementation
 */

#include "debug_scene.h"
#include "sensor.h"
#include "scenes.h"

void dht11_scene_debug_on_enter(void* context) {
    DHT11App* app = context;
    
    // Run debug sensor read and show results
    app->sensor_ok = dht11_sensor_debug_read(app);
    
    text_box_set_text(app->debug_text_box, app->debug_log);
    text_box_set_font(app->debug_text_box, TextBoxFontText);
    view_dispatcher_switch_to_view(app->view_dispatcher, DHT11SceneDebug);
}

bool dht11_scene_debug_on_event(void* context, SceneManagerEvent event) {
    DHT11App* app = context;
    bool consumed = false;
    
    if(event.type == SceneManagerEventTypeBack) {
        scene_manager_previous_scene(app->scene_manager);
        consumed = true;
    }
    
    return consumed;
}

void dht11_scene_debug_on_exit(void* context) {
    DHT11App* app = context;
    text_box_reset(app->debug_text_box);
}
