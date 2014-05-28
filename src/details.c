// details.c ///////////////////////////////////////////////////////////////////
// Display details of session
// Created by: Brian Jett
//			 bdjett@me.com
//		     http://logicalpixels.com
////////////////////////////////////////////////////////////////////////////////

#include "common.h"

static Session session;

static Window *window;
static TextLayer *title_layer;
static TextLayer *time_layer;
static TextLayer *room_layer;

// LOADING AND UNLOADING ///////////////////////////////////////////////////////

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	title_layer = text_layer_create((GRect) { .origin = { 5, 0 }, .size = { (bounds.size.w - 10), 60 } });
	text_layer_set_text(title_layer, "Loading...");
	text_layer_set_text_alignment(title_layer, GTextAlignmentLeft);
	text_layer_set_overflow_mode(title_layer, GTextOverflowModeWordWrap);
	text_layer_set_font(title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(title_layer));

	time_layer = text_layer_create((GRect) { .origin = { 5, 60 }, .size = { (bounds.size.w - 10), 25 } });
	text_layer_set_text_alignment(time_layer, GTextAlignmentLeft);
	text_layer_set_overflow_mode(time_layer, GTextOverflowModeWordWrap);
	text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	layer_add_child(window_layer, text_layer_get_layer(time_layer));

	room_layer = text_layer_create((GRect) { .origin = { 5, 80 }, .size = { (bounds.size.w - 10), 50 } });
	text_layer_set_text_alignment(room_layer, GTextAlignmentLeft);
	text_layer_set_overflow_mode(room_layer, GTextOverflowModeWordWrap);
	text_layer_set_font(room_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	layer_add_child(window_layer, text_layer_get_layer(room_layer));
}

static void window_unload(Window *window) {
	text_layer_destroy(title_layer);
	text_layer_destroy(time_layer);
	text_layer_destroy(room_layer);
}

// INITIALIZATION //////////////////////////////////////////////////////////////

void details_init(void) {
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
}

void details_show() {
	window_stack_push(window, true);
}

void details_destroy(void) {
	window_destroy_safe(window);
}

// IMPLEMENTATION //////////////////////////////////////////////////////////////

bool details_is_on_top() {
	return window == window_stack_get_top_window();
}

// APP MESSAGE HANDLERS ////////////////////////////////////////////////////////

void details_in_received_handler(DictionaryIterator *iter) {
	Tuple *title_tuple = dict_find(iter, SESSION_TITLE);
	Tuple *time_tuple = dict_find(iter, SESSION_TIME);
	Tuple *type_tuple = dict_find(iter, SESSION_TYPE);
	Tuple *room_tuple = dict_find(iter, SESSION_ROOM);

	if (title_tuple) {
		strncpy(session.title, title_tuple->value->cstring, sizeof(session.title));
		strncpy(session.time, time_tuple->value->cstring, sizeof(session.time));
		strncpy(session.room, room_tuple->value->cstring, sizeof(session.room));
		text_layer_set_text(title_layer, session.title);
		text_layer_set_text(time_layer, session.time);
		text_layer_set_text(room_layer, session.room);
	}
}
