// sessions.c //////////////////////////////////////////////////////////////////
// Display list of sessions
// Created by: Brian Jett
//			 bdjett@me.com
//		     http://logicalpixels.com
////////////////////////////////////////////////////////////////////////////////

#include "common.h"

#define MAX_SECTIONS 10

static Section sections[MAX_SECTIONS];
static int num_sections;
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

// INITIALIZATION //////////////////////////////////////////////////////////////

void times_init(void) {
	window = window_create();
	menu_layer = menu_layer_create_fullscreen(window);
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

void times_show() {
	clean_list();
	window_stack_push(window, true);
}

void times_destroy(void) {
	layer_remove_from_parent(menu_layer_get_layer(menu_layer));
	menu_layer_destroy_safe(menu_layer);
	window_destroy_safe(window);
}

// IMPLEMENTATION //////////////////////////////////////////////////////////////

static void clean_list() {
	memset(sections, 0x0, sizeof(sections));
	num_sections = 0;
	error[0] = '\0';
	menu_layer_set_selected_index(menu_layer, (MenuIndex) { .row = 0, .section = 0 }, MenuRowAlignBottom, false);
	menu_layer_reload_data_and_mark_dirty(menu_layer);
}

bool times_is_on_top() {
	return window == window_stack_get_top_window();
}

// APP MESSAGE HANDLERS ////////////////////////////////////////////////////////

void get_sessions(char *key) {
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Getting sessions for index: %s", key);
	Tuplet get_sessions_tuple = TupletCString(9, key);
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);

	if (iter == NULL) {
		return;
	}

	dict_write_tuplet(iter, &get_sessions_tuple);
	dict_write_end(iter);
	app_message_outbox_send();
	sessions_show();
}

void times_in_received_handler(DictionaryIterator *iter) {
	Tuple *index_tuple = dict_find(iter, SECTION_INDEX);
	Tuple *section_name_tuple = dict_find(iter, SECTION_NAME);
	Tuple *section_number_tuple = dict_find(iter, SECTION_NUMBER);
	Tuple *section_key_tuple = dict_find(iter, SECTION_KEY);

	if (index_tuple && section_name_tuple && section_number_tuple) {
		Section section;
		section.index = index_tuple->value->int16;
		strncpy(section.name, section_name_tuple->value->cstring, sizeof(section.name));
		strncpy(section.number, section_number_tuple->value->cstring, sizeof(section.number));
		strncpy(section.key, section_key_tuple->value->cstring, sizeof(section.key));
		sections[section.index] = section;
		num_sections++;
		menu_layer_reload_data_and_mark_dirty(menu_layer);
	}
}

// MENU LAYER CALLBACKS ////////////////////////////////////////////////////////

static uint16_t menu_get_num_sections_callback(MenuLayer *me, void *ctx) {
	return 1;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *me, uint16_t section_index, void *ctx) {
	return num_sections;
}

static int16_t menu_get_header_height_callback(MenuLayer *me, uint16_t section_index, void *ctx) {
	return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static int16_t menu_get_cell_height_callback(MenuLayer *me, MenuIndex *cell_index, void *ctx) {
	return MENU_CELL_BASIC_CELL_HEIGHT;
}

static void menu_draw_header_callback(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context) {
	menu_cell_basic_header_draw(ctx, cell_layer, "Start Times");
}

static void menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
	if (!num_sections) {
		menu_cell_basic_draw(ctx, cell_layer, "Loading...", NULL, NULL);
	} else {
		menu_cell_basic_draw(ctx, cell_layer, sections[cell_index->row].name, sections[cell_index->row].number, NULL);
	}
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
	get_sessions(sections[cell_index->row].key);
}

static void menu_select_long_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {

}
