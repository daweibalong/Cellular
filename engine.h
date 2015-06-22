#ifndef _ENGINE_H
#define _ENGINE_H

#include <string.h>
#include "box.h"
#include "queue.h"

#define MAX_BOXS 10000

typedef enum {
	ENG_NUL = 1 << 0, 
	ENG_INI = 1 << 1,
	ENG_RUN = 1 << 2,
	ENG_SUS = 1 << 3,
	ENG_DES = 1 << 4,
} EngStat;

typedef struct {
	unsigned int    box_size;
	Box *           box_list[MAX_BOXS];
	TaskQueue       task_queue;
	lua_State *     lua_state;
	EngStat         status;
} Engine;


Engine *
engine_init();

void 
engine_new_box(Engine *eng, const char *box_guid);

void
engine_drop_box(Engine *eng, const char *box_guid);

void
engine_load_comctx(Engine *eng, const char *file_name);

void 
engine_load_boxctx(Engine *eng, const char *box_guid, const char *file_name);

void 
engine_call_func(Engine *eng, const char *box_guid, const char *func_name);

void
engine_schedule(Engine *eng);

void 
engine_suspend(Engine *eng);

void 
engine_destroy(Engine *eng);

#endif
