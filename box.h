#ifndef _BOX_H
#define _BOX_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

typedef enum {
	BOX_INI = 1 << 0,    /* 环境加载完毕 */
	BOX_RED = 1 << 1,    /* 任务准备就绪 */
	BOX_RUN = 1 << 2,    /* 任务正在运行 */
	BOX_SUS = 1 << 3,    /* 任务已被挂起 */
	BOX_ERR = 1 << 4,    /* 异       常 */
	BOX_NUL = 1 << 5,    /* 未 初 始 化 */
} BoxStat;

typedef struct {
	/* 独有标记 */
	unsigned int    id;

	/* 占用CPU的字节码数 */
	unsigned int    reduction;

	/* 状态 */
	BoxStat         stat;
	
	/* 协程在LUA_REGISTRYINDEX中的位置 */ 
	int             co_ref;

	/* 环境表在LUA_REGISTRYINDEX中的位置 */
	int             env_ref;
	
	/* 持有协程 */
	lua_State *     coroutine;

	/* 所属的lua_State */
	lua_State *     lua_state;
} Box;

Box* 
box_init(lua_State *L);

void 
box_loadfile(Box *box, const char *file_name);

void
box_spawn(Box *box, const char *func_name);

void 
box_run(Box *box);

void 
box_destroy(Box *box);

#endif
