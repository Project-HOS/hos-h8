/* ------------------------------------------------------------------------- */
/*  HOS 割り込み管理                                                         */
/*                                                                           */
/*                                   Copyright (C) 1998-2002 by Project HOS  */
/* ------------------------------------------------------------------------- */


#include <itron.h>


/* 割り込みとディスパッチの禁止 */
ER   loc_cpu(void)
{
#if __ERR_CHECK_LEVEL >= 3
	/* タスク独立部からは呼べない */
	if ( sysstat & TSS_INDP )
		return E_CTX;
#endif
	
	/* 割り込みとディスパッチの禁止 */
	__di();
	sysstat = TSS_LOC;
	
	return E_OK;
}


/* 割り込みとディスパッチの許可 */
ER   unl_cpu(void)
{
#if __ERR_CHECK_LEVEL >= 3
	/* タスク独立部からは呼べない */
	if ( sysstat & TSS_INDP )
		return E_CTX;
#endif

	/* 念のため */
	__di();
	
	/* 遅延ディスパッチ */
	sysstat = TSS_TSK;
	if ( dlydsp )
		__tsk_dsp();
	__ei();
	
	return E_OK;
}
