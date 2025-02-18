/* ------------------------------------------------------------------------- */
/*  HOS タスクキュー操作                                                     */
/*                                                                           */
/*                                   Copyright (C) 1998-2002 by Project HOS  */
/* ------------------------------------------------------------------------- */

#include "itron.h"



/* タスクをキュー末尾に追加 */
void __adt_que(T_QUE *que, T_TCB *tcb)
{
	T_TCB *tcb_head;
	T_TCB *tcb_tail;
	
	tcb->que = que;
	if ( que->tskcnt++ == 0 ) {
		que->head = tcb;
		tcb->next = tcb;
		tcb->prev = tcb;
	} else {
		tcb_head       = que->head;
		tcb_tail       = tcb_head->prev;
		tcb->next      = tcb_head;
		tcb->prev      = tcb_tail;
		tcb_head->prev = tcb;
		tcb_tail->next = tcb;
	}
}


/* タスクをキュー先頭に追加 */
void __adh_que(T_QUE *que, T_TCB *tcb) {
	__adt_que(que, tcb);
	que->head = tcb;
}


/* キューにタスクを挿入 */
void __ins_que(T_TCB *tcbs, T_TCB *tcbd)
{
	T_QUE *que;
	T_TCB *tcb_next;
	
	que      = tcbs->que;
	tcb_next = tcbs->next;
	tcbd->que      = que;
	tcbd->next     = tcb_next;
	tcb_next->prev = tcbd;
	tcbd->prev     = tcbs;
	tcbs->next     = tcbd;
	que->tskcnt++;
}


/* キューからタスクを外す */
void __del_que(T_TCB *tcb) {
	T_TCB *tcb_next;
	T_TCB *tcb_prev;
	T_QUE *que;
	
	que = tcb->que;
	if ( --que->tskcnt != 0 ) {
		tcb_next = tcb->next;
		tcb_prev = tcb->prev;
		tcb_next->prev = tcb_prev;
		tcb_prev->next = tcb_next;
		if ( que->head == tcb )
			que->head = tcb_next;
	}
	tcb->que = NADR;
}


/* キューの回転 */
void __rot_que(T_QUE *que)
{
	if ( que->tskcnt != 0 )
		que->head = que->head->next;
}

