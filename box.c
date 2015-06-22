#include "box.h"

bool _stop = false;

static void
_hook(lua_State *L, lua_Debug *ar)
{
	if (_stop) {
		_stop = false;
		lua_yield(L, 0);
	}
}

void 
_box_make_sandbox(Box *box)
{
	lua_newtable(box->coroutine);

	/* create new metatable */
	lua_createtable(box->coroutine, 0, 1);

	/* mt.__index = _G */
	lua_pushvalue(box->coroutine, LUA_GLOBALSINDEX);
	lua_setfield(box->coroutine, -2, "__index");  
	
	/* setmetable(env, {__index = _G}) */	
	lua_setmetatable(box->coroutine, -2);

	/* hold the env table */
	int ref = luaL_ref(box->coroutine, LUA_REGISTRYINDEX);
	if (ref != LUA_NOREF) {
		box->env_ref = ref;
		box->stat = BOX_INI;
	}
}

Box *
box_init(lua_State *L)
{
	int base = lua_gettop(L);

	Box *box = NULL;
	box = (Box*)malloc(sizeof(Box));
	box->lua_state = L;
	box->coroutine = lua_newthread(L);
	box->reduction = 5;
	
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	if (ref != LUA_NOREF) {
		box->co_ref = ref;
		box->stat = BOX_NUL;
	}

	/* coroutine 存入LUA_REGISTRYINDEX中，从栈上删除 */
	lua_settop(L, base);

	/* 创建沙箱环境 */
	_box_make_sandbox(box);
	
	return box;
}

void 
box_loadfile(Box *box, const char *file_name)
{
	if (box == NULL || box->coroutine == NULL)
		return;

	if (box->stat != BOX_INI)
		return;

	luaL_loadfile(box->coroutine, file_name);

	/* 将环境表取到栈顶 */
	lua_rawgeti(box->coroutine, LUA_REGISTRYINDEX, box->env_ref);	

	/* 设置代码块环境表 */
	int hr = lua_setfenv(box->coroutine, -2);
	printf("setfenv %d %s\n", hr, lua_tostring(box->coroutine, -1));

	/* 保证load之后并设置了env之后的代码块一直在栈顶, *\ 
	 * 这里复制一份，保证pcall之后栈顶保存脚本编译的  *
	\* 函数,以便获取编译后的环境表                    */
	lua_pushvalue(box->coroutine, -1);

	if (!lua_pcall(box->coroutine, 0, 0, 0)) {
		box->stat = BOX_INI;
		printf("%d pcall ok\n", box->id);
	} else {
		box->stat = BOX_ERR;
		printf("pcall error %s\n", lua_tostring(box->coroutine, -1));
	}
	lua_getfenv(box->coroutine, -1);

	/* 更新环境表 */
	lua_rawseti(box->coroutine, LUA_REGISTRYINDEX, box->env_ref);

	/* 将栈上的环境表和函数出栈 */
	lua_settop(box->coroutine, -2);
}

void
box_spawn(Box *box, const char *func_name)
{
	if (box == NULL)
		return;

	switch (box->stat) {
		case BOX_INI:
			printf("box [%d] %s start:\n", box->id, func_name);

			/* 将环境表放到栈顶 */
			lua_rawgeti(box->coroutine, LUA_REGISTRYINDEX, box->env_ref);

			/* 在环境表中查找函数 */
			lua_getfield(box->coroutine, -1, func_name);

			/* 移除环境表 */
			lua_remove(box->coroutine, -2);

			box->stat = BOX_RED;
			break;
		case BOX_RED:
		case BOX_SUS:
		case BOX_RUN:
			printf("box [%d] is ocupied by other task\n", box->id);
			break;
		case BOX_NUL:
			printf("box [%d] has not loaded script\n", box->id);
			break;
		default:
			printf("error %d\n", box->id);
			break;;
	}
	box_run(box);
}

void 
box_run(Box *box)
{
	if (box == NULL)
		return;

	if (box->stat != BOX_RED && box->stat != BOX_SUS) {
		printf("box [%d] is not ready for run\n", box->id);
		return;
	}

	box->stat = BOX_RUN;
	
	/* 利用hook实现抢占式调度 */
	lua_sethook(box->coroutine, _hook, LUA_MASKCOUNT, box->reduction);

	int state = lua_resume(box->coroutine, 0);
	switch (state) {
		case LUA_YIELD:
			box->stat = BOX_SUS;
			printf("box [%d] suspend\n", box->id);
			break;
		case 0:
			box->stat = BOX_INI;
			printf("box [%d] end\n", box->id);

			/* TODO: handle the result returned by lua function */
			int nres = lua_gettop(box->coroutine);
			lua_pop(box->coroutine, nres);
			break;
		default:
			box->stat = BOX_ERR;
			/* pop error code */
			lua_pop(box->coroutine, 1);
			printf("oks\n");
			break;
	}
}

void 
box_destroy(Box *box)
{
	if (box->lua_state != NULL) {
		luaL_unref(box->lua_state, LUA_REGISTRYINDEX, box->co_ref);
	}
	if (box->coroutine != NULL) {
		luaL_unref(box->coroutine, LUA_REGISTRYINDEX, box->env_ref);
	}
	box->lua_state = NULL;
	box->coroutine = NULL;
	
	free(box);
	box = NULL;
}

