/* ------------------------------------------------------------------------- */
/*  HOS �����ߴ���                                                         */
/*                                                                           */
/*                                   Copyright (C) 1998-2002 by Project HOS  */
/* ------------------------------------------------------------------------- */


#include <itron.h>


/* �����ߤȥǥ����ѥå��ζػ� */
ER   loc_cpu(void)
{
#if __ERR_CHECK_LEVEL >= 3
	/* ��������Ω������ϸƤ٤ʤ� */
	if ( sysstat & TSS_INDP )
		return E_CTX;
#endif
	
	/* �����ߤȥǥ����ѥå��ζػ� */
	__di();
	sysstat = TSS_LOC;
	
	return E_OK;
}


/* �����ߤȥǥ����ѥå��ε��� */
ER   unl_cpu(void)
{
#if __ERR_CHECK_LEVEL >= 3
	/* ��������Ω������ϸƤ٤ʤ� */
	if ( sysstat & TSS_INDP )
		return E_CTX;
#endif

	/* ǰ�Τ��� */
	__di();
	
	/* �ٱ�ǥ����ѥå� */
	sysstat = TSS_TSK;
	if ( dlydsp )
		__tsk_dsp();
	__ei();
	
	return E_OK;
}
