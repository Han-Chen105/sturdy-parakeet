/*
 * Ticket_Persist.h
 * Description: 票务持久化层
 */

#ifndef TICKET_PERSIST_H_
#define TICKET_PERSIST_H_

#include "../Service/Ticket.h"

int Ticket_Perst_Insert(ticket_t *data);
int Ticket_Perst_InsertBatch(ticket_list_t list);
int Ticket_Perst_Update(const ticket_t *data);
int Ticket_Perst_SelectByID(int ID, ticket_t *buf);
int Ticket_Perst_SelectBySchedule(ticket_list_t list, int scheduleID);

#endif /* TICKET_PERSIST_H_ */

