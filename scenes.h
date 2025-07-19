/**
 * @file scenes.h
 * @brief Scene configuration and handlers
 * 
 * This file contains the scene handler function table and configuration
 * for Flipper's scene manager.
 */

#pragma once

#include <gui/scene_manager.h>

/**
 * @brief Scene handler function table
 * 
 * This table maps scene IDs to their corresponding handler functions
 * for use with Flipper's scene manager.
 */
extern const SceneManagerHandlers dht11_scene_handlers;

// Scene handler function declarations
void dht11_scene_main_menu_on_enter(void* context);
bool dht11_scene_main_menu_on_event(void* context, SceneManagerEvent event);
void dht11_scene_main_menu_on_exit(void* context);

void dht11_scene_read_sensor_on_enter(void* context);
bool dht11_scene_read_sensor_on_event(void* context, SceneManagerEvent event);
void dht11_scene_read_sensor_on_exit(void* context);

void dht11_scene_about_on_enter(void* context);
bool dht11_scene_about_on_event(void* context, SceneManagerEvent event);
void dht11_scene_about_on_exit(void* context);

void dht11_scene_debug_on_enter(void* context);
bool dht11_scene_debug_on_event(void* context, SceneManagerEvent event);
void dht11_scene_debug_on_exit(void* context);
