/*
 * Queries_UI.c
 * Description: 查询（可用版本）
 */

#include "../View/Queries_UI.h"

#include "../Common/List.h"
#include "../Service/Schedule.h"
#include "../Service/Ticket.h"

#include <stdio.h>

static void Queries_UI_ScheduleSummary(void) {
	schedule_list_t sl;
	List_Init(sl, schedule_node_t);
	int total = Schedule_Srv_FetchAll(sl);
	if (total <= 0) {
		printf("No schedule.\n");
		List_Destroy(sl, schedule_node_t);
		return;
	}

	printf("\n---------------------- Schedule Summary ----------------------\n");
	printf("%-6s %-10s %-10s %-10s\n", "ID", "Tickets", "Sold", "Revenue");
	schedule_node_t *sp = NULL;
	List_ForEach(sl, sp) {
		ticket_list_t tl;
		List_Init(tl, ticket_node_t);
		int tcount = Ticket_Srv_FetchBySchedule(tl, sp->data.id);
		int sold = 0;
		double revenue = 0.0;
		ticket_node_t *tp = NULL;
		List_ForEach(tl, tp) {
			if (tp->data.status == TICKET_SOLD) {
				sold++;
				revenue += tp->data.price;
			}
		}
		printf("%-6d %-10d %-10d %-10.2f\n", sp->data.id, tcount, sold, revenue);
		List_Destroy(tl, ticket_node_t);
	}
	printf("-------------------------------------------------------------\n");

	List_Destroy(sl, schedule_node_t);
}

static void Queries_UI_TicketList(void) {
	int scheduleID;
	printf("Input Schedule ID: ");
	scanf("%d", &scheduleID);

	ticket_list_t tl;
	List_Init(tl, ticket_node_t);
	int tcount = Ticket_Srv_FetchBySchedule(tl, scheduleID);
	if (tcount <= 0) {
		printf("No ticket for this schedule.\n");
		List_Destroy(tl, ticket_node_t);
		return;
	}

	printf("\n--------------------------- Ticket List ---------------------------\n");
	printf("%-6s %-10s %-10s %-8s %-6s\n", "ID", "Schedule", "SeatID", "Price", "Status");
	ticket_node_t *tp = NULL;
	List_ForEach(tl, tp) {
		printf("%-6d %-10d %-10d %-8.2f %-6d\n",
			   tp->data.id, tp->data.scheduleID, tp->data.seatID, tp->data.price, (int)tp->data.status);
	}
	printf("-------------------------------------------------------------------\n");

	List_Destroy(tl, ticket_node_t);
}

void Queries_Menu(void) {
	char choice;
	do {
		printf("\n=======================================================\n");
		printf("************************ Queries ************************\n");
		printf("[S]chedule summary | [T]icket list by schedule | [R]eturn\n");
		printf("Your Choice: ");
		scanf(" %c", &choice);

		switch (choice) {
		case 's':
		case 'S':
			Queries_UI_ScheduleSummary();
			break;
		case 't':
		case 'T':
			Queries_UI_TicketList();
			break;
		}
	} while (choice != 'r' && choice != 'R');
}

