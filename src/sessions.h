// sessions.h //////////////////////////////////////////////////////////////////
// Header file for sessions.c
// Created by: Brian Jett
//			 bdjett@me.com
//		     http://logicalpixels.com
////////////////////////////////////////////////////////////////////////////////

void sessions_init(void);
void sessions_show();
void sessions_destroy(void);
void sessions_in_received_handler(DictionaryIterator *iter);
bool sessions_is_on_top();
