/*
 * Sale_UI.c
 * Description: 售票/退票（可用版本）
 */

#include "../View/Sale_UI.h"

#include "../Common/List.h"
#include "../Service/Schedule.h"
#include "../Service/Studio.h"
#include "../Service/Seat.h"
#include "../Service/Ticket.h"

#include <stdio.h>

static int Sale_UI_SelectSchedule(int *outScheduleID, int *outStudioID, double *outPrice) {
	schedule_list_t list;
	List_Init(list, schedule_node_t);
	int total = Schedule_Srv_FetchAll(list);
	if (total <= 0) {
		printf("No schedule found. Please add schedule first.\n");
		List_Destroy(list, schedule_node_t);
		return 0;
	}

	printf("\n--------------------------- Schedule List --------------------------\n");
	printf("%-6s %-6s %-6s %-10s %-5s %-8s\n", "ID", "Play", "Room", "Date", "Time", "Price");
	schedule_node_t *p = NULL;
	List_ForEach(list, p) {
		printf("%-6d %-6d %-6d %04d-%02d-%02d %02d:%02d %-8.2f\n",
			   p->data.id, p->data.playID, p->data.studioID,
			   p->data.year, p->data.month, p->data.day,
			   p->data.hour, p->data.minute,
			   p->data.price);
	}
	printf("-------------------------------------------------------------------\n");

	printf("Input Schedule ID: ");
	scanf("%d", outScheduleID);

	schedule_t sch;
	if (!Schedule_Srv_FetchByID(*outScheduleID, &sch)) {
		printf("Schedule not found.\n");
		List_Destroy(list, schedule_node_t);
		return 0;
	}
	*outStudioID = sch.studioID;
	*outPrice = sch.price;

	List_Destroy(list, schedule_node_t);
	return 1;
}

static void Sale_UI_ShowSeatMap(int studioID, ticket_list_t tickets) {
	studio_t studio;
	if (!Studio_Srv_FetchByID(studioID, &studio)) {
		printf("Room not found.\n");
		return;
	}

	seat_list_t seats;
	List_Init(seats, seat_node_t);
	Seat_Srv_FetchByRoomID(seats, studioID);
	Seat_Srv_SortSeatList(seats);

	printf("\nLegend: O=available  S=sold  X=broken  .=none\n");
	for (int r = 1; r <= studio.rowsCount; r++) {
		printf("%2d | ", r);
		for (int c = 1; c <= studio.colsCount; c++) {
			seat_node_t *sn = Seat_Srv_FindByRowCol(seats, r, c);
			char ch = '?';
			if (!sn) {
				ch = '?';
			} else if (sn->data.status == SEAT_NONE) {
				ch = '.';
			} else if (sn->data.status == SEAT_BROKEN) {
				ch = 'X';
			} else {
				ticket_node_t *tn = Ticket_Srv_FindBySeatID(tickets, sn->data.id);
				if (!tn) {
					ch = '?';
				} else {
					ch = (tn->data.status == TICKET_SOLD) ? 'S' : 'O';
				}
			}
			printf("%c ", ch);
		}
		printf("\n");
	}

	List_Destroy(seats, seat_node_t);
}

void Sale_UI_MgtEntry(void) {
	int scheduleID = 0, studioID = 0;
	double price = 0.0;
	if (!Sale_UI_SelectSchedule(&scheduleID, &studioID, &price))
		return;

	// 若票不存在则生成
	Ticket_Srv_GenerateBySchedule(scheduleID, studioID, price);

	ticket_list_t tickets;
	List_Init(tickets, ticket_node_t);
	Ticket_Srv_FetchBySchedule(tickets, scheduleID);

	char choice;
	do {
		printf("\n=======================================================\n");
		printf("*********************** Ticket Sale ********************\n");
		Sale_UI_ShowSeatMap(studioID, tickets);
		printf("-------------------------------------------------------\n");
		printf("[B]uy | [R]efresh | [Q]uit\n");
		printf("Your Choice: ");
		scanf(" %c", &choice);

		if (choice == 'b' || choice == 'B') {
			int row, col;
			printf("Input row: ");
			scanf("%d", &row);
			printf("Input column: ");
			scanf("%d", &col);

			seat_list_t seats;
			List_Init(seats, seat_node_t);
			Seat_Srv_FetchByRoomID(seats, studioID);
			seat_node_t *sn = Seat_Srv_FindByRowCol(seats, row, col);
			if (!sn || sn->data.status != SEAT_GOOD) {
				printf("Seat is not available.\n");
				List_Destroy(seats, seat_node_t);
				continue;
			}

			ticket_node_t *tn = Ticket_Srv_FindBySeatID(tickets, sn->data.id);
			if (!tn) {
				printf("Ticket not found.\n");
				List_Destroy(seats, seat_node_t);
				continue;
			}
			if (tn->data.status == TICKET_SOLD) {
				printf("Already sold.\n");
				List_Destroy(seats, seat_node_t);
				continue;
			}

			tn->data.status = TICKET_SOLD;
			if (Ticket_Srv_Modify(&(tn->data))) {
				printf("Sold.\n");
			} else {
				printf("Sell failed.\n");
			}

			List_Destroy(seats, seat_node_t);
		} else if (choice == 'r' || choice == 'R') {
			Ticket_Srv_FetchBySchedule(tickets, scheduleID);
		}
	} while (choice != 'q' && choice != 'Q');

	List_Destroy(tickets, ticket_node_t);
}

void Sale_UI_ReturnTicket(void) {
	int scheduleID = 0, studioID = 0;
	double price = 0.0;
	if (!Sale_UI_SelectSchedule(&scheduleID, &studioID, &price))
		return;

	Ticket_Srv_GenerateBySchedule(scheduleID, studioID, price);

	ticket_list_t tickets;
	List_Init(tickets, ticket_node_t);
	Ticket_Srv_FetchBySchedule(tickets, scheduleID);

	char choice;
	do {
		printf("\n=======================================================\n");
		printf("*********************** Return Ticket ******************\n");
		Sale_UI_ShowSeatMap(studioID, tickets);
		printf("-------------------------------------------------------\n");
		printf("[R]eturn one | re[F]resh | [Q]uit\n");
		printf("Your Choice: ");
		scanf(" %c", &choice);

		if (choice == 'r' || choice == 'R') {
			int row, col;
			printf("Input row: ");
			scanf("%d", &row);
			printf("Input column: ");
			scanf("%d", &col);

			seat_list_t seats;
			List_Init(seats, seat_node_t);
			Seat_Srv_FetchByRoomID(seats, studioID);
			seat_node_t *sn = Seat_Srv_FindByRowCol(seats, row, col);
			if (!sn || sn->data.status != SEAT_GOOD) {
				printf("Seat invalid.\n");
				List_Destroy(seats, seat_node_t);
				continue;
			}

			ticket_node_t *tn = Ticket_Srv_FindBySeatID(tickets, sn->data.id);
			if (!tn) {
				printf("Ticket not found.\n");
				List_Destroy(seats, seat_node_t);
				continue;
			}
			if (tn->data.status != TICKET_SOLD) {
				printf("This seat is not sold.\n");
				List_Destroy(seats, seat_node_t);
				continue;
			}

			tn->data.status = TICKET_AVAILABLE;
			if (Ticket_Srv_Modify(&(tn->data))) {
				printf("Returned.\n");
			} else {
				printf("Return failed.\n");
			}
			List_Destroy(seats, seat_node_t);
		} else if (choice == 'f' || choice == 'F') {
			Ticket_Srv_FetchBySchedule(tickets, scheduleID);
		}
	} while (choice != 'q' && choice != 'Q');

	List_Destroy(tickets, ticket_node_t);
}

