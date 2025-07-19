/**
 * @file scenes.c
 * @brief Scene configuration implementation
 */

#include "scenes.h"
#include "app.h"

// Scene on_enter handlers
void (*const dht11_on_enter_handlers[])(void*) = {
    [DHT11SceneMainMenu] = dht11_scene_main_menu_on_enter,
    [DHT11SceneReadSensor] = dht11_scene_read_sensor_on_enter,
    [DHT11SceneAbout] = dht11_scene_about_on_enter,
    [DHT11SceneDebug] = dht11_scene_debug_on_enter,
};

// Scene on_event handlers
bool (*const dht11_on_event_handlers[])(void*, SceneManagerEvent) = {
    [DHT11SceneMainMenu] = dht11_scene_main_menu_on_event,
    [DHT11SceneReadSensor] = dht11_scene_read_sensor_on_event,
    [DHT11SceneAbout] = dht11_scene_about_on_event,
    [DHT11SceneDebug] = dht11_scene_debug_on_event,
};

// Scene on_exit handlers
void (*const dht11_on_exit_handlers[])(void*) = {
    [DHT11SceneMainMenu] = dht11_scene_main_menu_on_exit,
    [DHT11SceneReadSensor] = dht11_scene_read_sensor_on_exit,
    [DHT11SceneAbout] = dht11_scene_about_on_exit,
    [DHT11SceneDebug] = dht11_scene_debug_on_exit,
};

// Scene handler table for Flipper's scene manager
const SceneManagerHandlers dht11_scene_handlers = {
    .on_enter_handlers = dht11_on_enter_handlers,
    .on_event_handlers = dht11_on_event_handlers,
    .on_exit_handlers = dht11_on_exit_handlers,
    .scene_num = DHT11SceneCount,
};
