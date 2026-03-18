/*
* Copyright(C), 2007-2008, XUPT Univ.	 
* 用例编号：TTMS_UC_02
* File name: Seat_UI.c			  
* Description : 设置座位用例界面层	
* Author:   XUPT  		 
* Version:  v.1 	 
* Date: 	2015年4月22日	
*/

#include "Seat_UI.h"
#include "../Service/Seat.h"

#include "../Service/Studio.h"
#include "../Common/List.h"
#include <stdio.h>
#include <stdlib.h>

/*
表识符：TTMS_SCU_Seat_UI_S2C 
函数功能：根据座位状态获取界面显示符号。
参数说明：status为seat_status_t类型，表示座位状态。
返 回 值：字符型，表示座位的界面显示符号。
*/
inline char Seat_UI_Status2Char(seat_status_t status) {

	(void)status;
	switch (status) {
	case SEAT_NONE:
		return '.';
	case SEAT_GOOD:
		return 'O';
	case SEAT_BROKEN:
		return 'X';
	default:
		return '?';
	}
}

/*
标识符：TTMS_SCU_Seat_UI_C2S
函数功能：根据输入符号获取座位状态。
参数说明：statusChar为字符型，表示设置座位的输入符号。
返 回 值：seat_status_t类型，表示座位的状态。
*/
inline seat_status_t Seat_UI_Char2Status(char statusChar) {
	switch (statusChar) {
	case '.':
	case '0':
		return SEAT_NONE;
	case 'O':
	case 'o':
	case '1':
		return SEAT_GOOD;
	case 'X':
	case 'x':
	case '9':
		return SEAT_BROKEN;
	default:
		return SEAT_NONE;
	}
}

/*
标识符：TTMS_SCU_Seat_UI_MgtEnt
函数功能：界面层管理座位的入口函数，显示当前的座位数据，并提供座位数据添加、修改、删除功能操作的入口。
参数说明：roomID为整型，是需要设置座位的演出厅ID。
返 回 值：无。
*/ 
void Seat_UI_MgtEntry(int roomID) {
	studio_t studio;
	if (!Studio_Srv_FetchByID(roomID, &studio)) {
		printf("The room does not exist!\nPress [Enter] key to return!\n");
		getchar();
		return;
	}

	seat_list_t list;
	List_Init(list, seat_node_t);

	int seatCount = Seat_Srv_FetchByRoomID(list, roomID);
	if (seatCount <= 0) {
		Seat_Srv_RoomInit(list, roomID, studio.rowsCount, studio.colsCount);
		Seat_Srv_AddBatch(list);
		seatCount = Seat_Srv_FetchByRoomID(list, roomID);
	}
	Seat_Srv_SortSeatList(list);

	char choice;
	do {
		printf("\n=======================================================\n");
		printf("********************* Seat Management ******************\n");
		printf("Room ID:%d  Name:%s  Rows:%d  Cols:%d\n", studio.id, studio.name,
				studio.rowsCount, studio.colsCount);
		printf("Legend: O=good  X=broken  .=none\n");
		printf("-------------------------------------------------------\n");

		for (int r = 1; r <= studio.rowsCount; r++) {
			printf("%2d | ", r);
			for (int c = 1; c <= studio.colsCount; c++) {
				seat_node_t *node = Seat_Srv_FindByRowCol(list, r, c);
				char ch = node ? Seat_UI_Status2Char(node->data.status) : '?';
				printf("%c ", ch);
			}
			printf("\n");
		}

		printf("-------------------------------------------------------\n");
		printf("[M]odify seat | [R]efresh | [Q]uit\n");
		printf("Your Choice:");
		fflush(stdin);
		scanf("%c", &choice);
		fflush(stdin);

		switch (choice) {
		case 'm':
		case 'M': {
			int row, col;
			char st;
			printf("Input row (1-%d):", studio.rowsCount);
			scanf("%d", &row);
			printf("Input column (1-%d):", studio.colsCount);
			scanf("%d", &col);
			printf("Input status (O/X/.):");
			fflush(stdin);
			scanf("%c", &st);
			fflush(stdin);

			seat_node_t *node = Seat_Srv_FindByRowCol(list, row, col);
			if (!node) {
				printf("Seat not found.\n");
				break;
			}
			node->data.status = Seat_UI_Char2Status(st);
			if (Seat_Srv_Modify(&(node->data))) {
				printf("Updated.\n");
			} else {
				printf("Update failed.\n");
			}
			break;
		}
		case 'r':
		case 'R':
			Seat_Srv_FetchByRoomID(list, roomID);
			Seat_Srv_SortSeatList(list);
			break;
		}
	} while (choice != 'q' && choice != 'Q');

	List_Destroy(list, seat_node_t);
}

/*
识符：TTMS_SCU_Seat_UI_Add
函数功能：用于添加一个新的座位数据。
参数说明：第一个参数list为seat_list_t类型指针，指向座位链表头指针，
         第二个参数rowsCount为整型，表示座位所在行号，第三个参数colsCount为整型，表示座位所在列号。
返 回 值：整型，表示是否成功添加了座位的标志。
*/
int Seat_UI_Add(seat_list_t list, int roomID, int row, int column) {  //输入一个座位
	(void)list;
	seat_t rec;
	rec.id = 0;
	rec.roomID = roomID;
	rec.row = row;
	rec.column = column;
	rec.status = SEAT_GOOD;
	return Seat_Srv_Add(&rec);
}

/*
标识符：TTMS_SCU_Seat_UI_Mod 
函数功能：用于修改一个座位数据。
参数说明：第一个参数list为seat_list_t类型指针，指向座位链表头指针，第二个参数rowsCount为整型，表示座位所在行号，第三个参数colsCount为整型，表示座位所在列号。
返 回 值：整型，表示是否成功修改了座位的标志。
*/
int Seat_UI_Modify(seat_list_t list, int row, int column) {
	seat_node_t *node = Seat_Srv_FindByRowCol(list, row, column);
	if (!node)
		return 0;

	char st;
	printf("Input status for (%d,%d) (O/X/.):", row, column);
	fflush(stdin);
	scanf("%c", &st);
	fflush(stdin);
	node->data.status = Seat_UI_Char2Status(st);
	return Seat_Srv_Modify(&(node->data));

}

/*
标识符：TTMS_SCU_Seat_UI_Del
函数功能：用于删除一个座位的数据。
参数说明：第一个参数list为seat_list_t类型指针，指向座位链表头指针，第二个参数rowsCount为整型，表示座位所在行号，第三个参数colsCount为整型，表示座位所在列号。
返 回 值：整型，表示是否成功删除了座位的标志。
*/
int Seat_UI_Delete(seat_list_t list, int row, int column) {
	seat_node_t *node = Seat_Srv_FindByRowCol(list, row, column);
	if (!node)
		return 0;

	node->data.status = SEAT_NONE;
	return Seat_Srv_Modify(&(node->data));

}

