/*
 * Schedule_Persist.c
 * Description: 演出计划（场次）持久化层
 */

#include "Schedule_Persist.h"

#include "EntityKey_Persist.h"
#include "../Common/List.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static const char SCHEDULE_DATA_FILE[] = "Schedule.dat";
static const char SCHEDULE_DATA_TEMP_FILE[] = "ScheduleTmp.dat";
static const char SCHEDULE_KEY_NAME[] = "Schedule";

int Schedule_Perst_Insert(schedule_t *data) {
	assert(NULL != data);
	long key = EntKey_Perst_GetNewKeys(SCHEDULE_KEY_NAME, 1);
	if (key <= 0)
		return 0;
	data->id = (int)key;

	FILE *fp = fopen(SCHEDULE_DATA_FILE, "ab");
	if (NULL == fp) {
		printf("Cannot open file %s!\n", SCHEDULE_DATA_FILE);
		return 0;
	}
	int rtn = (int)fwrite(data, sizeof(schedule_t), 1, fp);
	fclose(fp);
	return rtn;
}

int Schedule_Perst_Update(const schedule_t *data) {
	assert(NULL != data);
	FILE *fp = fopen(SCHEDULE_DATA_FILE, "rb+");
	if (NULL == fp) {
		printf("Cannot open file %s!\n", SCHEDULE_DATA_FILE);
		return 0;
	}
	schedule_t buf;
	int found = 0;
	while (!feof(fp)) {
		if (fread(&buf, sizeof(schedule_t), 1, fp)) {
			if (buf.id == data->id) {
				fseek(fp, -((int)sizeof(schedule_t)), SEEK_CUR);
				fwrite(data, sizeof(schedule_t), 1, fp);
				found = 1;
				break;
			}
		}
	}
	fclose(fp);
	return found;
}

int Schedule_Perst_DeleteByID(int ID) {
	if (rename(SCHEDULE_DATA_FILE, SCHEDULE_DATA_TEMP_FILE) < 0)
		return 0;

	FILE *fpSour = fopen(SCHEDULE_DATA_TEMP_FILE, "rb");
	if (NULL == fpSour)
		return 0;
	FILE *fpTarg = fopen(SCHEDULE_DATA_FILE, "wb");
	if (NULL == fpTarg) {
		fclose(fpSour);
		return 0;
	}

	schedule_t buf;
	int found = 0;
	while (!feof(fpSour)) {
		if (fread(&buf, sizeof(schedule_t), 1, fpSour)) {
			if (ID == buf.id) {
				found = 1;
				continue;
			}
			fwrite(&buf, sizeof(schedule_t), 1, fpTarg);
		}
	}
	fclose(fpTarg);
	fclose(fpSour);
	remove(SCHEDULE_DATA_TEMP_FILE);
	return found;
}

int Schedule_Perst_SelectByID(int ID, schedule_t *buf) {
	assert(NULL != buf);
	FILE *fp = fopen(SCHEDULE_DATA_FILE, "rb");
	if (NULL == fp)
		return 0;

	schedule_t data;
	int found = 0;
	while (!feof(fp)) {
		if (fread(&data, sizeof(schedule_t), 1, fp)) {
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

int Schedule_Perst_SelectAll(schedule_list_t list) {
	assert(NULL != list);
	List_Free(list, schedule_node_t);

	FILE *fp = fopen(SCHEDULE_DATA_FILE, "rb");
	if (NULL == fp)
		return 0;

	schedule_t data;
	int recCount = 0;
	while (!feof(fp)) {
		if (fread(&data, sizeof(schedule_t), 1, fp)) {
			schedule_node_t *node = (schedule_node_t*)malloc(sizeof(schedule_node_t));
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

