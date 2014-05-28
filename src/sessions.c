// sessions.c //////////////////////////////////////////////////////////////////
// Display list of sessions
// Created by: Brian Jett
//			 bdjett@me.com
//		     http://logicalpixels.com
////////////////////////////////////////////////////////////////////////////////

#include "common.h"

#define MAX_SESSIONS 15

static Session sessions[MAX_SESSIONS];
static int num_sessions;
static char error[10];

static void clean_list();
static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *ctx);
static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *ctx);
static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *ctx);
static int16_t menu_get_cell_height_callback(MenuLayer *me, MenuIndex *cell_index, void *ctx);
static void menu_draw_header_callback(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context);
static void menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context);
static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void menu_select_long_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);

static Window *window;
static MenuLayer *menu_layer;
static GBitmap *lab_icon;

// INITIALIZATION //////////////////////////////////////////////////////////////

void sessions_init(void) {
	window = window_create();
	menu_layer = menu_layer_create_fullscreen(window);
	lab_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_LAB);
	menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks) {
		.get_num_sections = menu_get_num_sections_callback,
		.get_num_rows = menu_get_num_rows_callback,
		.get_header_height = menu_get_header_height_callback,
		.get_cell_height = menu_get_cell_height_callback,
		.draw_header = menu_draw_header_callback,
		.draw_row = menu_draw_row_callback,
		.select_click = menu_select_callback,
	});
	menu_layer_set_click_config_onto_window(menu_layer, window);
	menu_layer_add_to_window(menu_layer, window);
}

void sessions_show() {
	clean_list();
	window_stack_push(window, true);
}

void sessions_destroy(void) {
	layer_remove_from_parent(menu_layer_get_layer(menu_layer));
	menu_layer_destroy_safe(menu_layer);
	gbitmap_destroy(lab_icon);
	window_destroy_safe(window);
}

// IMPLEMENTATION //////////////////////////////////////////////////////////////

static void clean_list() {
	memset(sessions, 0x0, sizeof(sessions));
	num_sessions = 0;
	error[0] = '\0';
	menu_layer_set_selected_index(menu_layer, (MenuIndex) { .row = 0, .section = 0 }, MenuRowAlignBottom, false);
	menu_layer_reload_data_and_mark_dirty(menu_layer);
}

bool sessions_is_on_top() {
	return window == window_stack_get_top_window();
}

// APP MESSAGE HANDLERS ////////////////////////////////////////////////////////

void get_details(char *id) {
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Getting session info for id: %s", id);
	Tuplet get_session_info_tuple = TupletCString(GET_SESSION_INFO, id);
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	if (iter == NULL) {
		return;
	}

	dict_write_tuplet(iter, &get_session_info_tuple);
	dict_write_end(iter);
	app_message_outbox_send();
	details_show();
}

void sessions_in_received_handler(DictionaryIterator *iter) {
	Tuple *index_tuple = dict_find(iter, SESSION_INDEX);
	Tuple *title_tuple = dict_find(iter, SESSION_TITLE);
	Tuple *time_tuple = dict_find(iter, SESSION_TIME);
	Tuple *type_tuple = dict_find(iter, SESSION_TYPE);
	Tuple *room_tuple = dict_find(iter, SESSION_ROOM);
	Tuple *id_tuple = dict_find(iter, SESSION_ID);

	if (index_tuple && time_tuple && title_tuple && room_tuple && type_tuple) {
		Session session;
		session.index = index_tuple->value->int16;
		strncpy(session.title, title_tuple->value->cstring, sizeof(session.title));
		strncpy(session.time, time_tuple->value->cstring, sizeof(session.time));
		strncpy(session.room, room_tuple->value->cstring, sizeof(session.room));
		strncpy(session.type, type_tuple->value->cstring, sizeof(session.type));
		strncpy(session.id, id_tuple->value->cstring, sizeof(session.id));
		sessions[session.index] = session;
		num_sessions++;
		menu_layer_reload_data_and_mark_dirty(menu_layer);
	}
}

// MENU LAYER CALLBACKS ////////////////////////////////////////////////////////

static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *ctx) {
	return 1;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *ctx) {
	return num_sessions ? num_sessions : 1;
}

static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *ctx) {
	return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static int16_t menu_get_cell_height_callback(MenuLayer *me, MenuIndex *cell_index, void *ctx) {
	return MENU_CELL_BASIC_CELL_HEIGHT;
}

static void menu_draw_header_callback(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context) {
	menu_cell_basic_header_draw(ctx, cell_layer, "Sessions");
}

static void menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
	if (!num_sessions) {
		menu_cell_basic_draw(ctx, cell_layer, "Loading...", NULL, NULL);
	} else {
		if (strcmp(sessions[cell_index->row].type, "Lab") == 0) {
			menu_cell_basic_draw(ctx, cell_layer, sessions[cell_index->row].title, sessions[cell_index->row].time, lab_icon);
		} else {
			menu_cell_basic_draw(ctx, cell_layer, sessions[cell_index->row].title, sessions[cell_index->row].time, NULL);
		}
	}
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
	get_details(sessions[cell_index->row].id);
}

static void menu_select_long_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {

}
