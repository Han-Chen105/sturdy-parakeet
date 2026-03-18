/*
 * Account_UI.c
 * Description: 账号管理（可用版本）
 */

#include "../View/Account_UI.h"

#include "../Common/List.h"
#include "../Service/Account.h"

#include <stdio.h>
#include <string.h>

static void Account_UI_List(void) {
	account_list_t list;
	List_Init(list, account_node_t);
	int total = Account_Srv_FetchAll(list);

	printf("\n--------------------------- Account List ---------------------------\n");
	printf("%-6s %-20s %-6s\n", "ID", "Username", "Status");
	account_node_t *p = NULL;
	List_ForEach(list, p) {
		printf("%-6d %-20s %-6d\n", p->data.id, p->data.username, (int)p->data.status);
	}
	printf("-------------------------------------------------------------------\n");
	printf("Total: %d\n", total);

	List_Destroy(list, account_node_t);
}

static int Account_UI_AddOne(void) {
	account_t rec;
	memset(&rec, 0, sizeof(rec));
	rec.status = ACC_STATUS_ENABLED;

	printf("Username: ");
	scanf(" %31[^\n]", rec.username);
	printf("Password: ");
	scanf(" %31[^\n]", rec.password);
	printf("Status(0/1) [%d]: ", (int)rec.status);
	scanf("%d", (int*)&rec.status);

	return Account_Srv_Add(&rec);
}

static int Account_UI_ModifyOne(void) {
	int id;
	printf("Input Account ID: ");
	scanf("%d", &id);

	account_t rec;
	if (!Account_Srv_FetchByID(id, &rec)) {
		printf("Account not found.\n");
		return 0;
	}

	printf("Username[%s]: ", rec.username);
	scanf(" %31[^\n]", rec.username);
	printf("Password[%s]: ", rec.password);
	scanf(" %31[^\n]", rec.password);
	printf("Status[%d] (0/1): ", (int)rec.status);
	scanf("%d", (int*)&rec.status);

	return Account_Srv_Modify(&rec);
}

static int Account_UI_DeleteOne(void) {
	int id;
	printf("Input Account ID: ");
	scanf("%d", &id);
	return Account_Srv_DeleteByID(id);
}

void Account_UI_MgtEntry(void) {
	char choice;
	do {
		printf("\n=======================================================\n");
		printf("******************** Account Management ****************\n");
		printf("[L]ist | [A]dd | [U]pdate | [D]elete | [R]eturn\n");
		printf("Your Choice: ");
		scanf(" %c", &choice);

		switch (choice) {
		case 'l':
		case 'L':
			Account_UI_List();
			break;
		case 'a':
		case 'A':
			if (Account_UI_AddOne())
				printf("Added.\n");
			else
				printf("Add failed.\n");
			break;
		case 'u':
		case 'U':
			if (Account_UI_ModifyOne())
				printf("Updated.\n");
			else
				printf("Update failed.\n");
			break;
		case 'd':
		case 'D':
			if (Account_UI_DeleteOne())
				printf("Deleted.\n");
			else
				printf("Delete failed.\n");
			break;
		}
	} while (choice != 'r' && choice != 'R');
}

