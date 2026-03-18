/*
 * Account.c
 * Description: 账号业务逻辑层
 */

#include "Account.h"

#include "../Common/List.h"
#include "../Persistence/Account_Persist.h"

#include <assert.h>

int Account_Srv_Add(account_t *data) {
	assert(NULL != data);
	return Account_Perst_Insert(data);
}

int Account_Srv_Modify(const account_t *data) {
	assert(NULL != data);
	return Account_Perst_Update(data);
}

int Account_Srv_DeleteByID(int ID) {
	return Account_Perst_DeleteByID(ID);
}

int Account_Srv_FetchByID(int ID, account_t *buf) {
	return Account_Perst_SelectByID(ID, buf);
}

int Account_Srv_FetchAll(account_list_t list) {
	return Account_Perst_SelectAll(list);
}

account_node_t *Account_Srv_FindByID(account_list_t list, int ID) {
	assert(NULL != list);
	account_node_t *p = NULL;
	List_ForEach(list, p) {
		if (p->data.id == ID)
			return p;
	}
	return NULL;
}

