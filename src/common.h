// common.h ////////////////////////////////////////////////////////////////////
// Common header for all files
// Created by: Brian Jett
//			 bdjett@me.com
//		     http://logicalpixels.com
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <pebble.h>
#include "pebble-assist.h"
#include "sessions.h"
#include "times.h"
#include "details.h"

typedef struct {
	int index;
	char id[5];
	char title[60];
	char type[8];
	char room[30];
	char time[20];
} Session;

typedef struct {
	int index;
	char name[10];
	char number[15];
	char key[21];
} Section;

enum {
	SESSION_INDEX = 0x0,
	SESSION_ID = 0x1,
	SESSION_TITLE = 0x2,
	SESSION_TYPE = 0x3,
	SESSION_ROOM = 0x4,
	SESSION_TIME = 0x5,
	SECTION_NAME = 0x6,
	SECTION_NUMBER = 0x7,
	SECTION_INDEX = 0x8,
	GET_SESSIONS = 0x9,
	SECTION_KEY = 0xA,
	GET_SESSION_INFO = 0xB,
	ERROR = 0xC
};
