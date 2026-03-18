/*
 * Ticket.c
 * Description: 票务业务逻辑层
 */

#include "Ticket.h"

#include "../Common/List.h"
#include "../Persistence/Ticket_Persist.h"
#include "../Service/Seat.h"

#include <assert.h>
#include <stdlib.h>

int Ticket_Srv_GenerateBySchedule(int scheduleID, int studioID, double price) {
	// 为一个场次按“有效座位”生成票（已存在票则不重复生成：简单策略是若已有记录则不生成）
	ticket_list_t existing;
	List_Init(existing, ticket_node_t);
	int existCount = Ticket_Perst_SelectBySchedule(existing, scheduleID);
	List_Destroy(existing, ticket_node_t);
	if (existCount > 0)
		return existCount;

	seat_list_t seats;
	List_Init(seats, seat_node_t);
	int seatCount = Seat_Srv_FetchValidByRoomID(seats, studioID);
	if (seatCount <= 0) {
		List_Destroy(seats, seat_node_t);
		return 0;
	}

	ticket_list_t tickets;
	List_Init(tickets, ticket_node_t);

	seat_node_t *sp = NULL;
	List_ForEach(seats, sp) {
		ticket_node_t *tn = (ticket_node_t*)malloc(sizeof(ticket_node_t));
		if (!tn)
			break;
		tn->data.id = 0;
		tn->data.scheduleID = scheduleID;
		tn->data.seatID = sp->data.id;
		tn->data.price = price;
		tn->data.status = TICKET_AVAILABLE;
		List_AddTail(tickets, tn);
	}

	int inserted = Ticket_Perst_InsertBatch(tickets);
	List_Destroy(tickets, ticket_node_t);
	List_Destroy(seats, seat_node_t);
	return inserted;
}

int Ticket_Srv_Modify(const ticket_t *data) {
	assert(NULL != data);
	return Ticket_Perst_Update(data);
}

int Ticket_Srv_FetchByID(int ID, ticket_t *buf) {
	return Ticket_Perst_SelectByID(ID, buf);
}

int Ticket_Srv_FetchBySchedule(ticket_list_t list, int scheduleID) {
	return Ticket_Perst_SelectBySchedule(list, scheduleID);
}

ticket_node_t *Ticket_Srv_FindBySeatID(ticket_list_t list, int seatID) {
	assert(NULL != list);
	ticket_node_t *p = NULL;
	List_ForEach(list, p) {
		if (p->data.seatID == seatID)
			return p;
	}
	return NULL;
}

