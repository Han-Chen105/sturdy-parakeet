/*
 * Ticket_Persist.c
 * Description: 票务持久化层
 */

#include "Ticket_Persist.h"

#include "EntityKey_Persist.h"
#include "../Common/List.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static const char TICKET_DATA_FILE[] = "Ticket.dat";
static const char TICKET_KEY_NAME[] = "Ticket";

int Ticket_Perst_Insert(ticket_t *data) {
	assert(NULL != data);
	long key = EntKey_Perst_GetNewKeys(TICKET_KEY_NAME, 1);
	if (key <= 0)
		return 0;
	data->id = (int)key;

	FILE *fp = fopen(TICKET_DATA_FILE, "ab");
	if (!fp) {
		printf("Cannot open file %s!\n", TICKET_DATA_FILE);
		return 0;
	}
	int rtn = (int)fwrite(data, sizeof(ticket_t), 1, fp);
	fclose(fp);
	return rtn;
}

int Ticket_Perst_InsertBatch(ticket_list_t list) {
	assert(NULL != list);
	if (List_IsEmpty(list))
		return 0;

	int count = 0;
	ticket_node_t *p = NULL;
	List_ForEach(list, p) {
		count++;
	}
	long startKey = EntKey_Perst_GetNewKeys(TICKET_KEY_NAME, count);
	if (startKey <= 0)
		return 0;

	FILE *fp = fopen(TICKET_DATA_FILE, "ab");
	if (!fp) {
		printf("Cannot open file %s!\n", TICKET_DATA_FILE);
		return 0;
	}

	int inserted = 0;
	long key = startKey;
	List_ForEach(list, p) {
		p->data.id = (int)key++;
		if (fwrite(&(p->data), sizeof(ticket_t), 1, fp))
			inserted++;
		else
			break;
	}
	fclose(fp);
	return inserted;
}

int Ticket_Perst_Update(const ticket_t *data) {
	assert(NULL != data);
	FILE *fp = fopen(TICKET_DATA_FILE, "rb+");
	if (!fp) {
		printf("Cannot open file %s!\n", TICKET_DATA_FILE);
		return 0;
	}

	ticket_t buf;
	int found = 0;
	while (!feof(fp)) {
		if (fread(&buf, sizeof(ticket_t), 1, fp)) {
			if (buf.id == data->id) {
				fseek(fp, -((int)sizeof(ticket_t)), SEEK_CUR);
				fwrite(data, sizeof(ticket_t), 1, fp);
				found = 1;
				break;
			}
		}
	}
	fclose(fp);
	return found;
}

int Ticket_Perst_SelectByID(int ID, ticket_t *buf) {
	assert(NULL != buf);
	FILE *fp = fopen(TICKET_DATA_FILE, "rb");
	if (!fp)
		return 0;

	ticket_t data;
	int found = 0;
	while (!feof(fp)) {
		if (fread(&data, sizeof(ticket_t), 1, fp)) {
			if (ID == data.id) {
				*buf = data;
				found = 1;
				break;
			}
		}
	}
	fclose(fp);
	return found;
}

int Ticket_Perst_SelectBySchedule(ticket_list_t list, int scheduleID) {
	assert(NULL != list);
	List_Free(list, ticket_node_t);

	FILE *fp = fopen(TICKET_DATA_FILE, "rb");
	if (!fp)
		return 0;

	ticket_t data;
	int recCount = 0;
	while (!feof(fp)) {
		if (fread(&data, sizeof(ticket_t), 1, fp)) {
			if (data.scheduleID != scheduleID)
				continue;
			ticket_node_t *node = (ticket_node_t*)malloc(sizeof(ticket_node_t));
			if (!node)
				break;
			node->data = data;
			List_AddTail(list, node);
			recCount++;
		}
	}
	fclose(fp);
	return recCount;
}

