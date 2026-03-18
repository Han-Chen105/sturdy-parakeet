/*
 * SalesAnalysis_UI.c
 * Description: 销售统计与排行（可用版本：按场次统计）
 */

#include "../View/SalesAnalysis_UI.h"

#include "../Common/List.h"
#include "../Service/Schedule.h"
#include "../Service/Ticket.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int scheduleID;
	int sold;
	double revenue;
} sch_stat_t;

static int cmp_stat(const void *a, const void *b) {
	const sch_stat_t *x = (const sch_stat_t*)a;
	const sch_stat_t *y = (const sch_stat_t*)b;
	if (x->revenue < y->revenue)
		return 1;
	if (x->revenue > y->revenue)
		return -1;
	return y->sold - x->sold;
}

void SalesAnalysis_UI_MgtEntry(void) {
	schedule_list_t sl;
	List_Init(sl, schedule_node_t);
	int total = Schedule_Srv_FetchAll(sl);
	if (total <= 0) {
		printf("No schedule.\nPress [Enter] to return.\n");
		fflush(stdin);
		getchar();
		List_Destroy(sl, schedule_node_t);
		return;
	}

	sch_stat_t *stats = (sch_stat_t*)malloc(sizeof(sch_stat_t) * (size_t)total);
	if (!stats) {
		List_Destroy(sl, schedule_node_t);
		return;
	}

	int idx = 0;
	schedule_node_t *sp = NULL;
	List_ForEach(sl, sp) {
		ticket_list_t tl;
		List_Init(tl, ticket_node_t);
		Ticket_Srv_FetchBySchedule(tl, sp->data.id);
		int sold = 0;
		double revenue = 0.0;
		ticket_node_t *tp = NULL;
		List_ForEach(tl, tp) {
			if (tp->data.status == TICKET_SOLD) {
				sold++;
				revenue += tp->data.price;
			}
		}
		stats[idx].scheduleID = sp->data.id;
		stats[idx].sold = sold;
		stats[idx].revenue = revenue;
		idx++;
		List_Destroy(tl, ticket_node_t);
	}

	qsort(stats, (size_t)idx, sizeof(sch_stat_t), cmp_stat);

	printf("\n================= Ranking and Statistics =================\n");
	printf("%-4s %-10s %-10s %-10s\n", "No.", "Schedule", "Sold", "Revenue");
	for (int i = 0; i < idx; i++) {
		printf("%-4d %-10d %-10d %-10.2f\n", i + 1, stats[i].scheduleID, stats[i].sold, stats[i].revenue);
	}
	printf("==========================================================\n");
	printf("Press [Enter] to return.\n");
	fflush(stdin);
	getchar();

	free(stats);
	List_Destroy(sl, schedule_node_t);
}

