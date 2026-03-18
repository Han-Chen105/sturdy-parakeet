/*
 * Play.h
 * Description: 剧目业务逻辑层头文件
 */

#ifndef PLAY_H_
#define PLAY_H_

// 剧目类型（精简版）
typedef enum {
	PLAY_TYPE_MOVIE = 1,
	PLAY_TYPE_OPERA = 2,
	PLAY_TYPE_CONCERT = 3
} play_type_t;

// 剧目状态
typedef enum {
	PLAY_STATUS_INACTIVE = 0,
	PLAY_STATUS_ACTIVE = 1
} play_status_t;

typedef struct {
	int id;
	char name[64];
	play_type_t type;
	char area[32];
	int durationMinutes;
	double price;
	play_status_t status;
} play_t;

typedef struct play_node {
	play_t data;
	struct play_node *next, *prev;
} play_node_t, *play_list_t;

int Play_Srv_Add(play_t *data);
int Play_Srv_Modify(const play_t *data);
int Play_Srv_DeleteByID(int ID);
int Play_Srv_FetchByID(int ID, play_t *buf);
int Play_Srv_FetchAll(play_list_t list);
play_node_t *Play_Srv_FindByID(play_list_t list, int ID);

#endif /* PLAY_H_ */

