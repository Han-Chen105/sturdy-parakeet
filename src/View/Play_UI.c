/*
 * Play_UI.c
 * Description: 剧目管理界面层
 */

#include "../View/Play_UI.h"

#include "../Common/List.h"
#include "../Service/Play.h"

#include <stdio.h>
#include <string.h>

static void Play_UI_List(void) {
	play_list_t list;
	List_Init(list, play_node_t);
	int total = Play_Srv_FetchAll(list);

	printf("\n---------------------------- Play List ----------------------------\n");
	printf("%-6s %-20s %-6s %-10s %-8s %-8s %-6s\n",
		   "ID", "Name", "Type", "Area", "Minutes", "Price", "Status");
	play_node_t *p = NULL;
	List_ForEach(list, p) {
		printf("%-6d %-20s %-6d %-10s %-8d %-8.2f %-6d\n",
			   p->data.id, p->data.name, (int)p->data.type, p->data.area,
			   p->data.durationMinutes, p->data.price, (int)p->data.status);
	}
	printf("-------------------------------------------------------------------\n");
	printf("Total: %d\n", total);

	List_Destroy(list, play_node_t);
}

static int Play_UI_AddOne(void) {
	play_t rec;
	memset(&rec, 0, sizeof(rec));
	rec.status = PLAY_STATUS_ACTIVE;

	printf("Play name: ");
	scanf(" %63[^\n]", rec.name);
	printf("Type (1=movie,2=opera,3=concert): ");
	scanf("%d", (int*)&rec.type);
	printf("Area: ");
	scanf(" %31[^\n]", rec.area);
	printf("Duration minutes: ");
	scanf("%d", &rec.durationMinutes);
	printf("Base price: ");
	scanf("%lf", &rec.price);

	return Play_Srv_Add(&rec);
}

static int Play_UI_ModifyOne(void) {
	int id;
	printf("Input Play ID: ");
	scanf("%d", &id);

	play_t rec;
	if (!Play_Srv_FetchByID(id, &rec)) {
		printf("Play not found.\n");
		return 0;
	}

	printf("Name[%s]: ", rec.name);
	scanf(" %63[^\n]", rec.name);
	printf("Type[%d]: ", (int)rec.type);
	scanf("%d", (int*)&rec.type);
	printf("Area[%s]: ", rec.area);
	scanf(" %31[^\n]", rec.area);
	printf("Duration[%d]: ", rec.durationMinutes);
	scanf("%d", &rec.durationMinutes);
	printf("Price[%.2f]: ", rec.price);
	scanf("%lf", &rec.price);
	printf("Status[%d] (0/1): ", (int)rec.status);
	scanf("%d", (int*)&rec.status);

	return Play_Srv_Modify(&rec);
}

static int Play_UI_DeleteOne(void) {
	int id;
	printf("Input Play ID: ");
	scanf("%d", &id);
	return Play_Srv_DeleteByID(id);
}

void Play_UI_MgtEntry(int schedulingMode) {
	(void)schedulingMode;

	char choice;
	do {
		printf("\n=======================================================\n");
		printf("********************* Play Management ******************\n");
		printf("[L]ist | [A]dd | [U]pdate | [D]elete | [R]eturn\n");
		printf("Your Choice: ");
		fflush(stdin);
		scanf(" %c", &choice);

		switch (choice) {
		case 'l':
		case 'L':
			Play_UI_List();
			break;
		case 'a':
		case 'A':
			if (Play_UI_AddOne())
				printf("Added.\n");
			else
				printf("Add failed.\n");
			break;
		case 'u':
		case 'U':
			if (Play_UI_ModifyOne())
				printf("Updated.\n");
			else
				printf("Update failed.\n");
			break;
		case 'd':
		case 'D':
			if (Play_UI_DeleteOne())
				printf("Deleted.\n");
			else
				printf("Delete failed.\n");
			break;
		}
	} while (choice != 'r' && choice != 'R');
}

