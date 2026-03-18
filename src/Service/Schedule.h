/*
 * Schedule.h
 * Description: 演出计划（场次）业务逻辑层
 */

#ifndef SCHEDULE_H_
#define SCHEDULE_H_

typedef enum {
	SCH_STATUS_INACTIVE = 0,
	SCH_STATUS_ACTIVE = 1
} schedule_status_t;

typedef struct {
	int id;
	int playID;
	int studioID;
	int year, month, day;
	int hour, minute;
	double price;              // 实际场次票价（默认继承 play.price）
	schedule_status_t status;
} schedule_t;

typedef struct schedule_node {
	schedule_t data;
	struct schedule_node *next, *prev;
} schedule_node_t, *schedule_list_t;

int Schedule_Srv_Add(schedule_t *data);
int Schedule_Srv_Modify(const schedule_t *data);
int Schedule_Srv_DeleteByID(int ID);
int Schedule_Srv_FetchByID(int ID, schedule_t *buf);
int Schedule_Srv_FetchAll(schedule_list_t list);
schedule_node_t *Schedule_Srv_FindByID(schedule_list_t list, int ID);

#endif /* SCHEDULE_H_ */

