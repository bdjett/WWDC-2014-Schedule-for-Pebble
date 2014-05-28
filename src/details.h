// details.h /./////////////////////////////////////////////////////////////////
// Header file for details.c
// Created by: Brian Jett
//			 bdjett@me.com
//		     http://logicalpixels.com
////////////////////////////////////////////////////////////////////////////////

void details_init(void);
void details_show();
void details_destroy(void);
void details_in_received_handler(DictionaryIterator *iter);
bool details_is_on_top();
