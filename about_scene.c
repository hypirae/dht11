/**
 * @file about_scene.c
 * @brief About scene implementation
 */

#include "about_scene.h"
#include "scenes.h"

void dht11_scene_about_on_enter(void* context) {
    DHT11App* app = context;
    
    if(!app->about_text) {
        app->about_text = malloc(2048);
        snprintf(app->about_text, 2048,
            "DHT11 Temperature & Humidity Sensor\n"
            "Version: 1.0\n\n"
            "PINOUT:\n"
            "VCC  -> 3.3V (Pin 9)\n"
            "DATA -> C0   (Pin 16)\n"
            "GND  -> GND  (Pin 8/11)\n\n"
            "SPECIFICATIONS:\n"
            "Temperature: 0-50°C (±2°C)\n"
            "Humidity: 20-90%% (±5%%)\n\n"
            "USAGE:\n"
            "1. Connect DHT11 sensor\n"
            "2. Go to 'Read Sensor'\n"
            "3. Press OK to read\n"
            "4. View temperature/humidity\n\n"
            "TROUBLESHOOTING:\n"
            "- Check connections\n"
            "- Ensure 3.3V power supply\n"
            "- Verify C0 pin wiring\n"
            "- Wait 1-2 seconds between reads\n"
            "- Use debug mode for details\n\n"
            "Built for Flipper Zero\n"
            "Educational purposes"
        );
    }
    
    text_box_set_text(app->about_text_box, app->about_text);
    text_box_set_font(app->about_text_box, TextBoxFontText);
    view_dispatcher_switch_to_view(app->view_dispatcher, DHT11SceneAbout);
}

bool dht11_scene_about_on_event(void* context, SceneManagerEvent event) {
    DHT11App* app = context;
    bool consumed = false;
    
    if(event.type == SceneManagerEventTypeBack) {
        scene_manager_previous_scene(app->scene_manager);
        consumed = true;
    }
    
    return consumed;
}

void dht11_scene_about_on_exit(void* context) {
    DHT11App* app = context;
    text_box_reset(app->about_text_box);
}
