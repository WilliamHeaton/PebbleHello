#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "resource_ids.auto.h"


#define MY_UUID { 0x76, 0xDC, 0xD7, 0x76, 0x36, 0xA9, 0x4E, 0x87, 0x9D, 0x56, 0x64, 0xB7, 0xC0, 0x88, 0xC9, 0xA0 }
PBL_APP_INFO(MY_UUID,
             "Hello", "William Heaton",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;
BmpContainer background_container;
TextLayer textLayer;
AppTimerHandle timer_handle;

const uint32_t helloTimer = 1;
const uint32_t itisTimer  = 2;
const uint32_t timeTimer  = 3;

bool showingTime = false;

void hello(AppContextRef ctx){
    showingTime = false;
    text_layer_set_text(&textLayer, "hello");
    timer_handle = app_timer_send_event(ctx, 4000, itisTimer);
}
void itis(AppContextRef ctx){
    showingTime = false;
    text_layer_set_text(&textLayer, "it is");
    timer_handle = app_timer_send_event(ctx, 1000, timeTimer);
}
void thetime(AppContextRef ctx){
    showingTime = true;
    static char timeText[] = "00:00";
    PblTm currentTime;
    get_time(&currentTime);
    
    if(clock_is_24h_style())
        string_format_time(timeText, sizeof(timeText), "%H:%M", &currentTime);
    else
        string_format_time(timeText, sizeof(timeText), "%I:%M", &currentTime);

    text_layer_set_text(&textLayer, timeText);
}


void handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t cookie) {
    (void)ctx;
    (void)handle;

    if(cookie == helloTimer){
        hello(ctx);
    }else if(cookie == itisTimer){
        itis(ctx);
    }else if(cookie == timeTimer){
        thetime(ctx);
    }

}
void handle_second_tick(AppContextRef ctx, PebbleTickEvent *t) {
    (void)t;
    (void)ctx;
    
    int sec = t->tick_time->tm_sec;
    
    if( showingTime && ( sec == 0 || sec == 30 ) )
        hello(ctx);
}

void handle_deinit(AppContextRef ctx) {
  (void)ctx;
  bmp_deinit_container(&background_container);
}

void handle_init(AppContextRef ctx) {
    (void)ctx;
    
    window_init(&window, "Hello");
    window_stack_push(&window, true /* Animated */);
    
    resource_init_current_app(&HELLO_IMAGE_RESOURCES);
    bmp_init_container(RESOURCE_ID_IMAGE_HELLO, &background_container);
    layer_add_child(&window.layer, &background_container.layer.layer);
    
//    text_layer_init(&textLayer, GRect(7, 46, 130, 76));
    text_layer_init(&textLayer, GRect(7, 56, 130, 50));
    text_layer_set_text_color(&textLayer, GColorBlack);
    text_layer_set_background_color(&textLayer, GColorClear);
    text_layer_set_font(&textLayer, fonts_get_system_font(FONT_KEY_GOTHAM_42_LIGHT));
    text_layer_set_text_alignment(&textLayer, GTextAlignmentCenter);
    layer_add_child(&window.layer, &textLayer.layer);
    
    hello(ctx);
}


void pbl_main(void *params) {
    PebbleAppHandlers handlers = {
        .init_handler = &handle_init,
        .deinit_handler = &handle_deinit,
        .timer_handler = &handle_timer,
        .tick_info = {
            .tick_handler = &handle_second_tick,
            .tick_units = SECOND_UNIT
        }
    };
    app_event_loop(params, &handlers);
}
