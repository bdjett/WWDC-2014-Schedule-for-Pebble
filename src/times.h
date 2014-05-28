// sessions.h //////////////////////////////////////////////////////////////////
// Header file for sessions.c
// Created by: Brian Jett
//			 bdjett@me.com
//		     http://logicalpixels.com
////////////////////////////////////////////////////////////////////////////////

void times_init(void);
void times_show();
void times_destroy(void);
void times_in_received_handler(DictionaryIterator *iter);
bool times_is_on_top();
