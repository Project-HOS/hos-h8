/* ------------------------------------------------------------------------- */
/*  HOS �����ƥ����                                                         */
/*                                                                           */
/*                                   Copyright (C) 1998-2002 by Project HOS  */
/* ------------------------------------------------------------------------- */

#include "itron.h"


/* �����Х��ѿ� */
UH sysstat;		/* �����ƥ�ξ��� */
UH int_cnt;		/* �����ߤΥͥ��ȿ� */


/* �С�����󻲾� */
ER   get_ver(T_VER *pk_ver)
{
	pk_ver->maker   = 0x0000;	/* �С������ʤ��ʼ¸������ƥ�) */
	pk_ver->id      = 0x0301;	/* HOS-H8 */
	pk_ver->spver   = 0x5302;	/* ��ITRON 3.02 */
	pk_ver->prver   = 0x0100;   /* Ver 1.00 */
	pk_ver->prno[0] = 0;
	pk_ver->prno[1] = 0;
	pk_ver->prno[2] = 0;
	pk_ver->prno[3] = 0;
	pk_ver->cpu     = 0x0000;	/* �ãУվ���ʤ� (H8-300H) */
	pk_ver->var     = 0x8000;	/* ��ITRON ��٥�S */
	
	return E_OK;
}


/* �����ƥ���ֻ��� */
ER   ref_sys(T_RSYS *pk_rsys)
{
	__set_imsk();
	
	pk_rsys->sysstat = sysstat;
	
	__res_imsk();
	
	return E_OK;
}

