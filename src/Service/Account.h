/*
 * Account.h
 * Description: 账号业务逻辑层（最简：本地账号表）
 */

#ifndef ACCOUNT_H_
#define ACCOUNT_H_

typedef enum {
	ACC_STATUS_DISABLED = 0,
	ACC_STATUS_ENABLED = 1
} account_status_t;

typedef struct {
	int id;
	char username[32];
	char password[32]; // 简化：明文存储（仅作课程/框架演示）
	account_status_t status;
} account_t;

typedef struct account_node {
	account_t data;
	struct account_node *next, *prev;
} account_node_t, *account_list_t;

int Account_Srv_Add(account_t *data);
int Account_Srv_Modify(const account_t *data);
int Account_Srv_DeleteByID(int ID);
int Account_Srv_FetchByID(int ID, account_t *buf);
int Account_Srv_FetchAll(account_list_t list);
account_node_t *Account_Srv_FindByID(account_list_t list, int ID);

#endif /* ACCOUNT_H_ */

