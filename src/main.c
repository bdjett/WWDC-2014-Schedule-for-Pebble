// main.c //////////////////////////////////////////////////////////////////////
// Set up app message handlers and initialize windows
// Created by: Brian Jett
//			 bdjett@me.com
//		     http://logicalpixels.com
////////////////////////////////////////////////////////////////////////////////

#include "common.h"

static Window *window;
static TextLayer *text_layer;
static GBitmap *apple_bg;
static BitmapLayer *apple_bg_layer;

// ERROR HANDLER ///////////////////////////////////////////////////////////////
void main_show() {
    window_stack_push(window, true);
}

bool main_is_on_top() {
    return window == window_stack_get_top_window();
}

// APP MESSAGE HANDLERS ////////////////////////////////////////////////////////

void out_sent_handler(DictionaryIterator *sent, void *ctx) {
    // outgoing message was delivered
}

void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *ctx) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Message failed because %d", reason);
}

void in_received_handler(DictionaryIterator *iter, void *ctx) {
    Tuple *section_index_tuple = dict_find(iter, SECTION_INDEX);
    Tuple *session_index_tuple = dict_find(iter, SESSION_INDEX);
    Tuple *session_title_tuple = dict_find(iter, SESSION_TITLE);
    Tuple *error_tuple = dict_find(iter, ERROR);

    if (error_tuple) {
        if (!main_is_on_top()) {
            main_show();
        }
        if (main_is_on_top()) {
            text_layer_set_text(text_layer, error_tuple->value->cstring);
        }
    } else if (section_index_tuple) {
        if (!times_is_on_top()) {
            window_stack_pop_all(true);
            times_show();
        }
        if (times_is_on_top()) {
            times_in_received_handler(iter);
        }
    } else if (session_index_tuple) {
        if (!sessions_is_on_top()) {
            //sessions_show();
        }
        if (sessions_is_on_top()) {
            sessions_in_received_handler(iter);
        }
    } else if (session_title_tuple) {
        if (!details_is_on_top()) {
            //details_show();
        }
        if (details_is_on_top()) {
            details_in_received_handler(iter);
        }
    }
}

void in_dropped_handler(AppMessageResult reason, void *ctx) {
    // incoming message dropped
}

// WINDOW LOADING AND UNLOADING ////////////////////////////////////////////////

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    apple_bg = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_APPLE_BG);
    apple_bg_layer = bitmap_layer_create((GRect) { .origin = { 0, 0 }, .size = bounds.size });
    bitmap_layer_set_bitmap(apple_bg_layer, apple_bg);
    layer_add_child(window_layer, bitmap_layer_get_layer(apple_bg_layer));

    text_layer = text_layer_create((GRect) { .origin = { 0, 85 }, .size = { bounds.size.w, 28 } });
    text_layer_set_text(text_layer, "Loading...");
    text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
    text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
    gbitmap_destroy(apple_bg);
    bitmap_layer_destroy(apple_bg_layer);
    text_layer_destroy(text_layer);
}

// INITIALIZATION AND DEINITIALIZATION /////////////////////////////////////////

static void init(void) {
    window = window_create();
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    app_message_register_inbox_received(in_received_handler);
    app_message_register_inbox_dropped(in_dropped_handler);
    app_message_register_outbox_sent(out_sent_handler);
    app_message_register_outbox_failed(out_failed_handler);
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
    sessions_init();
    times_init();
    details_init();
    const bool animated = true;
    window_stack_push(window, animated);
}

static void deinit(void) {
    sessions_destroy();
    times_destroy();
    details_destroy();
    window_destroy(window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
