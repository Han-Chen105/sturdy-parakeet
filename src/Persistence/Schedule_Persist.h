/*
 * Schedule_Persist.h
 * Description: 演出计划（场次）持久化层
 */

#ifndef SCHEDULE_PERSIST_H_
#define SCHEDULE_PERSIST_H_

#include "../Service/Schedule.h"

int Schedule_Perst_Insert(schedule_t *data);
int Schedule_Perst_Update(const schedule_t *data);
int Schedule_Perst_DeleteByID(int ID);
int Schedule_Perst_SelectByID(int ID, schedule_t *buf);
int Schedule_Perst_SelectAll(schedule_list_t list);

#endif /* SCHEDULE_PERSIST_H_ */

