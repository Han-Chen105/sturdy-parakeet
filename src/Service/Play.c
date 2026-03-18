/*
 * Play.c
 * Description: 剧目业务逻辑层
 */

#include "Play.h"

#include "../Persistence/Play_Persist.h"
#include "../Common/List.h"
#include <assert.h>

int Play_Srv_Add(play_t *data) {
	assert(NULL != data);
	return Play_Perst_Insert(data);
}

int Play_Srv_Modify(const play_t *data) {
	assert(NULL != data);
	return Play_Perst_Update(data);
}

int Play_Srv_DeleteByID(int ID) {
	return Play_Perst_DeleteByID(ID);
}

int Play_Srv_FetchByID(int ID, play_t *buf) {
	return Play_Perst_SelectByID(ID, buf);
}

int Play_Srv_FetchAll(play_list_t list) {
	return Play_Perst_SelectAll(list);
}

play_node_t *Play_Srv_FindByID(play_list_t list, int ID) {
	assert(NULL != list);
	play_node_t *p = NULL;
	List_ForEach(list, p) {
		if (p->data.id == ID)
			return p;
	}
	return NULL;
}

