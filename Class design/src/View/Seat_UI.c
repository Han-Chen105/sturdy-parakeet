/*
 * Seat_UI.c - cleaned ASCII-only implementation
 * - Removed non-ASCII comments and duplicated blocks
 * - Implementations match prototypes in Seat_UI.h
 */

#include "Seat_UI.h"
#include "../Service/Seat.h"
#include "../Service/Studio.h"
#include "../Common/List.h"
#include <stdio.h>

char Seat_UI_Status2Char(seat_status_t status) {
    switch (status) {
    case SEAT_GOOD: return '#';
    case SEAT_BROKEN: return 'X';
    default: return ' ';
    }
}

seat_status_t Seat_UI_Char2Status(char statusChar) {
    switch (statusChar) {
    case '#': return SEAT_GOOD;
    case 'X': case 'x': return SEAT_BROKEN;
    default: return SEAT_NONE;
    }
}

void Seat_UI_MgtEntry(int roomID) {
    studio_t studio;
    seat_list_t list;
    List_Init(list, seat_node_t);
    if (!Studio_Srv_FetchByID(roomID, &studio)) {
        printf("Studio %d not found\n", roomID);
        List_Destroy(list, seat_node_t);
        return;
    }
    Seat_Srv_FetchByRoomID(list, roomID);
    int r, c;
    for (r = 1; r <= studio.rowsCount; r++) {
        for (c = 1; c <= studio.colsCount; c++) {
            seat_node_t *node = Seat_Srv_FindByRowCol(list, r, c);
            char ch = node ? Seat_UI_Status2Char(node->data.status) : ' ';
            printf("%c ", ch);
        }
        printf("\n");
    }
    List_Destroy(list, seat_node_t);
}

int Seat_UI_Add(seat_list_t list, int roomID, int rowsCount, int colsCount) {
    if (NULL == list) return 0;
    int row, col;
    printf("Enter row (1-%d): ", rowsCount); if (scanf("%d", &row)!=1) return 0;
    printf("Enter column (1-%d): ", colsCount); if (scanf("%d", &col)!=1) return 0;
    if (row < 1 || row > rowsCount || col < 1 || col > colsCount) {
        printf("Invalid position\n");
        return 0;
    }
    seat_t s = {0};
    s.roomID = roomID; s.row = row; s.column = col; s.status = SEAT_GOOD;
    return Seat_Srv_Add(&s);
}

int Seat_UI_Modify(seat_list_t list, int rowsCount, int colsCount) {
    if (NULL == list) return 0;
    int row, col;
    printf("Enter row to modify (1-%d): ", rowsCount); if (scanf("%d", &row)!=1) return 0;
    printf("Enter column to modify (1-%d): ", colsCount); if (scanf("%d", &col)!=1) return 0;
    seat_node_t *node = Seat_Srv_FindByRowCol(list, row, col);
    if (NULL == node) {
        printf("Seat not found\n");
        return 0;
    }
    seat_t s = node->data;
    s.status = (s.status == SEAT_GOOD) ? SEAT_BROKEN : SEAT_GOOD;
    int ret = Seat_Srv_Modify(&s);
    if (ret) node->data = s;
    return ret;
}

int Seat_UI_Delete(seat_list_t list, int rowsCount, int colsCount) {
    if (NULL == list) return 0;
    int row, col;
    printf("Enter row to delete (1-%d): ", rowsCount); if (scanf("%d", &row)!=1) return 0;
    printf("Enter column to delete (1-%d): ", colsCount); if (scanf("%d", &col)!=1) return 0;
    seat_node_t *node = Seat_Srv_FindByRowCol(list, row, col);
    if (NULL == node) {
        printf("Seat not found\n");
        return 0;
    }
    return Seat_Srv_DeleteByID(node->data.id);
}
