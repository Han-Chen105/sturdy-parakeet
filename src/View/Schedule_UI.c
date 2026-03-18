/*
 * Schedule_UI.c
 * Description: 演出计划（场次）管理界面
 */

#include "../View/Schedule_UI.h"

#include "../Common/List.h"
#include "../Service/Schedule.h"
#include "../Service/Play.h"
#include "../Service/Studio.h"
#include "../Service/Ticket.h"

#include <stdio.h>
#include <string.h>

static void Schedule_UI_List(void) {
	schedule_list_t list;
	List_Init(list, schedule_node_t);
	int total = Schedule_Srv_FetchAll(list);

	printf("\n-------------------------- Schedule List --------------------------\n");
	printf("%-6s %-6s %-6s %-10s %-5s %-8s %-6s\n",
		   "ID", "Play", "Room", "Date", "Time", "Price", "Status");
	schedule_node_t *p = NULL;
	List_ForEach(list, p) {
		printf("%-6d %-6d %-6d %04d-%02d-%02d %02d:%02d %-8.2f %-6d\n",
			   p->data.id, p->data.playID, p->data.studioID,
			   p->data.year, p->data.month, p->data.day,
			   p->data.hour, p->data.minute,
			   p->data.price, (int)p->data.status);
	}
	printf("-------------------------------------------------------------------\n");
	printf("Total: %d\n", total);

	List_Destroy(list, schedule_node_t);
}

static int Schedule_UI_AddOne(void) {
	schedule_t rec;
	memset(&rec, 0, sizeof(rec));
	rec.status = SCH_STATUS_ACTIVE;

	printf("Play ID: ");
	scanf("%d", &rec.playID);
	play_t play;
	if (!Play_Srv_FetchByID(rec.playID, &play)) {
		printf("Play not found.\n");
		return 0;
	}

	printf("Room(Studio) ID: ");
	scanf("%d", &rec.studioID);
	studio_t studio;
	if (!Studio_Srv_FetchByID(rec.studioID, &studio)) {
		printf("Room not found.\n");
		return 0;
	}

	printf("Date (YYYY MM DD): ");
	scanf("%d %d %d", &rec.year, &rec.month, &rec.day);
	printf("Time (HH MM): ");
	scanf("%d %d", &rec.hour, &rec.minute);

	rec.price = play.price;
	printf("Price[%.2f] (input new or same): ", rec.price);
	scanf("%lf", &rec.price);

	if (!Schedule_Srv_Add(&rec))
		return 0;

	// 自动生成该场次票
	Ticket_Srv_GenerateBySchedule(rec.id, rec.studioID, rec.price);
	return 1;
}

static int Schedule_UI_DeleteOne(void) {
	int id;
	printf("Input Schedule ID: ");
	scanf("%d", &id);
	return Schedule_Srv_DeleteByID(id);
}

void Schedule_UI_MgtEntry(void) {
	char choice;
	do {
		printf("\n=======================================================\n");
		printf("******************* Schedule Management ****************\n");
		printf("[L]ist | [A]dd | [D]elete | [R]eturn\n");
		printf("Your Choice: ");
		scanf(" %c", &choice);

		switch (choice) {
		case 'l':
		case 'L':
			Schedule_UI_List();
			break;
		case 'a':
		case 'A':
			if (Schedule_UI_AddOne())
				printf("Added.\n");
			else
				printf("Add failed.\n");
			break;
		case 'd':
		case 'D':
			if (Schedule_UI_DeleteOne())
				printf("Deleted.\n");
			else
				printf("Delete failed.\n");
			break;
		}
	} while (choice != 'r' && choice != 'R');
}

