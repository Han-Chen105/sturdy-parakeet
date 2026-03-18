/*
 * Ticket.h
 * Description: 票务业务逻辑层
 */

#ifndef TICKET_H_
#define TICKET_H_

typedef enum {
	TICKET_AVAILABLE = 0,
	TICKET_SOLD = 1
} ticket_status_t;

typedef struct {
	int id;
	int scheduleID;
	int seatID;
	double price;
	ticket_status_t status;
} ticket_t;

typedef struct ticket_node {
	ticket_t data;
	struct ticket_node *next, *prev;
} ticket_node_t, *ticket_list_t;

int Ticket_Srv_GenerateBySchedule(int scheduleID, int studioID, double price);
int Ticket_Srv_Modify(const ticket_t *data);
int Ticket_Srv_FetchByID(int ID, ticket_t *buf);
int Ticket_Srv_FetchBySchedule(ticket_list_t list, int scheduleID);
ticket_node_t *Ticket_Srv_FindBySeatID(ticket_list_t list, int seatID);

#endif /* TICKET_H_ */

