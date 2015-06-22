#include "engine.h"

extern bool _stop;

unsigned int
_hash(const char *str)
{
	int hash = 0;

	int i;
	for(i = 0; i < strlen(str); i++) {
		hash = hash * 10 + str[i] - '0';
	}
	return hash;
}
	
Engine *
engine_init()
{
	Engine *eng = (Engine *) malloc(sizeof(Engine));
	eng->box_size = 0;
	eng->task_queue = create_queue(); 
	eng->status = ENG_INI;
	eng->lua_state = luaL_newstate();
	luaL_openlibs(eng->lua_state);

	int i;
	for (i = 0; i < MAX_BOXS; i++) {
		eng->box_list[i] = NULL;
	}

	return eng;
}

void
engine_new_box(Engine *eng, const char *box_guid)
{
	Box *box = box_init(eng->lua_state);
	box->id = _hash(box_guid);
	eng->box_list[box->id] = box;
	eng->box_size++;
}

void
engine_drop_box(Engine *eng, const char *box_guid)
{
	int hash = _hash(box_guid);
	Box *box = eng->box_list[hash];
	box_destroy(box);
	eng->box_size--;
	eng->box_list[hash] = NULL;
}

void
engine_load_comctx(Engine *eng, const char *file_name)
{
	if (eng->status != ENG_INI) {
		return;
	}
	int res = luaL_dofile(eng->lua_state, file_name);
	if (res != 0) {
		lua_pop(eng->lua_state, -1);
	}
}

void 
engine_load_boxctx(Engine *eng, const char *box_guid, const char *file_name)
{
	if (eng->status != ENG_INI) {
		return;
	}

	int hash = _hash(box_guid);
	printf("%s %s\n", box_guid, file_name);
	Box *box = NULL;
	if (eng->box_list[hash] == NULL) {
		 engine_new_box(eng, box_guid);
	}
	box = eng->box_list[hash];
	box_loadfile(box, file_name);
}


void
engine_call_func(Engine *eng, const char *box_guid, const char *func_name)
{
	int hash = _hash(box_guid);
	Box *box = eng->box_list[hash];

	if (box != NULL) {
		/*printf("Call func %s\n", func_name);*/
		Task *task = (Task*)malloc(sizeof(Task));
		task->box = box;
		task->func_name = func_name;
		enqueue(eng->task_queue, task);
	}
}

void 
engine_schedule(Engine *eng)
{
	int i = 0;
	eng->status = ENG_RUN;
	while (!is_empty(eng->task_queue) && eng->status == ENG_RUN) {
		Task *t = front_and_dequeue(eng->task_queue);
	
		if (t->box->stat == BOX_SUS) {
			enqueue(eng->task_queue, t);
		} else {
			lua_pushinteger(eng->lua_state, t->box->id);
			lua_setfield(eng->lua_state, LUA_GLOBALSINDEX, "_ID");
			box_spawn(t->box, t->func_name);
			free(t);
		}
	}
}

void 
engine_suspend(Engine *eng)
{
	eng->status = ENG_SUS;
	_stop = true;
}

void 
engine_destroy(Engine *eng)
{
	int i;
	for (i = 0; i < eng->box_size; i++) {
		box_destroy(eng->box_list[i]);
	}

	if (eng->lua_state != NULL) {
		lua_close(eng->lua_state);
		eng->lua_state = NULL;
	}
	eng->status = ENG_DES;
}
