/*
 * Schedule.c
 * Description: 演出计划（场次）业务逻辑层
 */

#include "Schedule.h"

#include "../Common/List.h"
#include "../Persistence/Schedule_Persist.h"

#include <assert.h>

int Schedule_Srv_Add(schedule_t *data) {
	assert(NULL != data);
	return Schedule_Perst_Insert(data);
}

int Schedule_Srv_Modify(const schedule_t *data) {
	assert(NULL != data);
	return Schedule_Perst_Update(data);
}

int Schedule_Srv_DeleteByID(int ID) {
	return Schedule_Perst_DeleteByID(ID);
}

int Schedule_Srv_FetchByID(int ID, schedule_t *buf) {
	return Schedule_Perst_SelectByID(ID, buf);
}

int Schedule_Srv_FetchAll(schedule_list_t list) {
	return Schedule_Perst_SelectAll(list);
}

schedule_node_t *Schedule_Srv_FindByID(schedule_list_t list, int ID) {
	assert(NULL != list);
	schedule_node_t *p = NULL;
	List_ForEach(list, p) {
		if (p->data.id == ID)
			return p;
	}
	return NULL;
}

