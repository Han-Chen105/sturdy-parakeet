/*
 * Play_Persist.c
 * Description: 剧目持久化层
 */

#include "Play_Persist.h"

#include "EntityKey_Persist.h"
#include "../Common/List.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static const char PLAY_DATA_FILE[] = "Play.dat";
static const char PLAY_DATA_TEMP_FILE[] = "PlayTmp.dat";
static const char PLAY_KEY_NAME[] = "Play";

int Play_Perst_Insert(play_t *data) {
	assert(NULL != data);

	long key = EntKey_Perst_GetNewKeys(PLAY_KEY_NAME, 1);
	if (key <= 0)
		return 0;
	data->id = (int)key;

	FILE *fp = fopen(PLAY_DATA_FILE, "ab");
	if (NULL == fp) {
		printf("Cannot open file %s!\n", PLAY_DATA_FILE);
		return 0;
	}
	int rtn = (int)fwrite(data, sizeof(play_t), 1, fp);
	fclose(fp);
	return rtn;
}

int Play_Perst_Update(const play_t *data) {
	assert(NULL != data);
	FILE *fp = fopen(PLAY_DATA_FILE, "rb+");
	if (NULL == fp) {
		printf("Cannot open file %s!\n", PLAY_DATA_FILE);
		return 0;
	}

	play_t buf;
	int found = 0;
	while (!feof(fp)) {
		if (fread(&buf, sizeof(play_t), 1, fp)) {
			if (buf.id == data->id) {
				fseek(fp, -((int)sizeof(play_t)), SEEK_CUR);
				fwrite(data, sizeof(play_t), 1, fp);
				found = 1;
				break;
			}
		}
	}
	fclose(fp);
	return found;
}

int Play_Perst_DeleteByID(int ID) {
	if (rename(PLAY_DATA_FILE, PLAY_DATA_TEMP_FILE) < 0) {
		return 0;
	}

	FILE *fpSour = fopen(PLAY_DATA_TEMP_FILE, "rb");
	if (NULL == fpSour)
		return 0;
	FILE *fpTarg = fopen(PLAY_DATA_FILE, "wb");
	if (NULL == fpTarg) {
		fclose(fpSour);
		return 0;
	}

	play_t buf;
	int found = 0;
	while (!feof(fpSour)) {
		if (fread(&buf, sizeof(play_t), 1, fpSour)) {
			if (ID == buf.id) {
				found = 1;
				continue;
			}
			fwrite(&buf, sizeof(play_t), 1, fpTarg);
		}
	}
	fclose(fpTarg);
	fclose(fpSour);
	remove(PLAY_DATA_TEMP_FILE);
	return found;
}

int Play_Perst_SelectByID(int ID, play_t *buf) {
	assert(NULL != buf);
	FILE *fp = fopen(PLAY_DATA_FILE, "rb");
	if (NULL == fp)
		return 0;

	play_t data;
	int found = 0;
	while (!feof(fp)) {
		if (fread(&data, sizeof(play_t), 1, fp)) {
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

int Play_Perst_SelectAll(play_list_t list) {
	assert(NULL != list);
	List_Free(list, play_node_t);

	FILE *fp = fopen(PLAY_DATA_FILE, "rb");
	if (NULL == fp)
		return 0;

	play_t data;
	int recCount = 0;
	while (!feof(fp)) {
		if (fread(&data, sizeof(play_t), 1, fp)) {
			play_node_t *node = (play_node_t*)malloc(sizeof(play_node_t));
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

