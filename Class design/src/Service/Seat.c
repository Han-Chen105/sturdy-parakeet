/*
* Copyright(C), 2007-2008, XUPT Univ.	
* 用例编号：TTMS_UC_02 
* File name: Seat.c		  
* Description : 设置座位用例业务逻辑层	
* Author:   XUPT  		 
* Version:  v.1 	 
* Date: 	2015年4月22日	
*/

#include <stdlib.h>
#include "../Common/List.h"
#include "Seat.h"
#include "../Persistence/Seat_Persist.h"

/*
函数功能：用于添加一个新座位数据。
参数说明：data为seat_t类型指针，表示需要添加的座位数据结点。
返 回 值：整型，表示是否成功添加了座位的标志。
*/
int Seat_Srv_Add(const seat_t *data){
	return Seat_Perst_Insert((seat_t *)data);
}

/*
函数功能：批量添加座位数据。
参数说明：list为seat_list_t类型指针，表示需要添加的批量座位数据形成的链表。
返 回 值：整型，表示是否成功添加了批量座位的标志。
*/
int Seat_Srv_AddBatch(seat_list_t list){
	return Seat_Perst_InsertBatch(list);
}

/*
函数功能：用于修改一个座位数据。
参数说明：data为seat_t类型指针，表示需要修改的座位数据结点。
返 回 值：整型，表示是否成功修改了座位的标志。
*/
int Seat_Srv_Modify(const seat_t *data){
	return Seat_Perst_Update(data);
}

/*
函数功能：根据座位ID删除一个座位。
参数说明：ID为整型，表示需要删除的座位数据结点。
返 回 值：整型，表示是否成功删除了座位的标志。
*/
int Seat_Srv_DeleteByID(int ID){
	return Seat_Perst_DeleteByID(ID);
}

/*
函数功能：根据座位ID获取座位数据。
参数说明：第一个参数ID为整型，表示座位ID，第二个参数buf为seat_t指针，指向待获取的座位数据结点。
返 回 值：整型，表示是否成功获取了座位的标志。
*/
int Seat_Srv_FetchByID(int ID, seat_t *buf){
	return Seat_Perst_SelectByID(ID, buf);
}

/*
函数功能：根据演出厅ID删除所有座位。
参数说明：roomID为整型，表示需要删除所有座位的演出厅ID。
返 回 值：整型，表示是否成功删除了演出厅所有座位的标志。
*/
inline int Seat_Srv_DeleteAllByRoomID(int roomID){
	return Seat_Perst_DeleteAllByRoomID(roomID);
}

/*
函数功能：根据演出厅ID获得该演出厅的有效座位。
参数说明：第一个参数list为seat_list_t类型，表示获取到的有效座位链表头指针，第二个参数roomID为整型，表示需要提取有效座位的演出厅ID。
返 回 值：整型，表示演出厅的有效座位个数。
*/
int Seat_Srv_FetchByRoomID(seat_list_t list, int roomID){
       return Seat_Perst_SelectByRoomID(list, roomID);
}

/*
函数功能：根据演出厅ID获得该演出厅的有效座位。
参数说明：第一个参数list为seat_list_t类型，表示获取到的有效座位链表头指针，第二个参数roomID为整型，表示需要提取有效座位的演出厅ID。
返 回 值：整型，表示演出厅的有效座位个数。
*/
int Seat_Srv_FetchValidByRoomID(seat_list_t list, int roomID)
{
       int count = 0;
       seat_list_t tempList;
       List_Init(tempList, seat_node_t);
       Seat_Perst_SelectByRoomID(tempList, roomID);
       seat_node_t *curPos;
       List_ForEach(tempList, curPos) {
           if (curPos->data.status == SEAT_GOOD) {
               seat_node_t *newNode = (seat_node_t *)malloc(sizeof(seat_node_t));
               newNode->data = curPos->data;
               List_AddTail(list, newNode);
               count++;
           }
       }
       List_Destroy(tempList, seat_node_t);
       return count;
}

/*
函数功能：根据给定演出厅的行、列数初始化演出厅的所有座位数据，并将每个座位结点按行插入座位链表。
参数说明：第一个参数list为seat_list_t类型指针，指向座位链表头指针，第二个参数rowsCount为整型，表示座位所在行号，第三个参数colsCount为整型，表示座位所在列号。
返 回 值：整型，表示是否成功初始化了演出厅的所有座位。
*/
int Seat_Srv_RoomInit(seat_list_t list, int roomID, int rowsCount,
		int colsCount) {
	int i, j;
	for (i = 1; i <= rowsCount; i++) {
		for (j = 1; j <= colsCount; j++) {
			seat_node_t *newNode = (seat_node_t *)malloc(sizeof(seat_node_t));
			newNode->data.id = 0; // ID will be set later
			newNode->data.roomID = roomID;
			newNode->data.row = i;
			newNode->data.column = j;
			newNode->data.status = SEAT_GOOD;
			List_AddTail(list, newNode);
		}
	}
	return 1;
}

/*
函数功能：对座位链表list按座位行号、列号进行排序。
参数说明：list为seat_list_t类型，表示待排序座位链表头指针。
返 回 值：无。
*/
void Seat_Srv_SortSeatList(seat_list_t list) {
	if (List_IsEmpty(list)) return;
	seat_list_t sortedList;
	List_Init(sortedList, seat_node_t);
	seat_node_t *curPos;
	List_ForEach(list, curPos) {
		Seat_Srv_AddToSoftedList(sortedList, curPos);
	}
	// Now copy back to original list
	List_Free(list, seat_node_t);
	List_ForEach(sortedList, curPos) {
		seat_node_t *newNode = (seat_node_t *)malloc(sizeof(seat_node_t));
		newNode->data = curPos->data;
		List_AddTail(list, newNode);
	}
	List_Destroy(sortedList, seat_node_t);
}

/*
函数功能：将一个座位结点加入到已排序的座位链表中。
参数说明：第一个参数list为seat_list_t类型，表示待插入结点的座位链表头指针，第二个参数node为seat_node_t指针，表示需要插入的座位数据结点。
返 回 值：无。
*/
void Seat_Srv_AddToSoftedList(seat_list_t list, seat_node_t *node) {
	seat_node_t *curPos;
	List_ForEach(list, curPos) {
		if (curPos->data.row > node->data.row ||
			(curPos->data.row == node->data.row && curPos->data.column > node->data.column)) {
			List_InsertBefore(curPos, node);
			return;
		}
	}
	List_AddTail(list, node);
}

/*
函数功能：根据座位的行、列号获取座位数据。
参数说明：第一个参数list为seat_list_t类型，表示座位链表头指针，
         第二个参数row为整型，表示待获取座位的行号，第三个参数column为整型，表示待获取座位的列号。
返 回 值：为seat_node_t指针，表示获取到的座位数据。
*/
seat_node_t * Seat_Srv_FindByRowCol(seat_list_t list, int row, int column) {
       seat_node_t *curPos;
       List_ForEach(list, curPos) {
           if (curPos->data.row == row && curPos->data.column == column) {
               return curPos;
           }
       }
       return NULL;
}

/*
函数功能：根据座位ID在链表中获取座位数据。
参数说明：第一个参数list为seat_list_t类型，指向座位数据链表，第二个参数ID为整型，表示座位ID。
返 回 值：seat_node_t类型，表示获取的座位数据。
*/
seat_node_t * Seat_Srv_FindByID(seat_list_t list, int rowID) {
       seat_node_t *curPos;
       List_ForEach(list, curPos) {
           if (curPos->data.id == rowID) {
               return curPos;
           }
       }
       return NULL;
}
