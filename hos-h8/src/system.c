/* ------------------------------------------------------------------------- */
/*  HOS システム管理                                                         */
/*                                                                           */
/*                                   Copyright (C) 1998-2002 by Project HOS  */
/* ------------------------------------------------------------------------- */

#include "itron.h"


/* グローバル変数 */
UH sysstat;		/* システムの状態 */
UH int_cnt;		/* 割り込みのネスト数 */


/* バージョン参照 */
ER   get_ver(T_VER *pk_ver)
{
	pk_ver->maker   = 0x0000;	/* バージョンなし（実験システム) */
	pk_ver->id      = 0x0301;	/* HOS-H8 */
	pk_ver->spver   = 0x5302;	/* μITRON 3.02 */
	pk_ver->prver   = 0x0100;   /* Ver 1.00 */
	pk_ver->prno[0] = 0;
	pk_ver->prno[1] = 0;
	pk_ver->prno[2] = 0;
	pk_ver->prno[3] = 0;
	pk_ver->cpu     = 0x0000;	/* ＣＰＵ情報なし (H8-300H) */
	pk_ver->var     = 0x8000;	/* μITRON レベルS */
	
	return E_OK;
}


/* システム状態参照 */
ER   ref_sys(T_RSYS *pk_rsys)
{
	__set_imsk();
	
	pk_rsys->sysstat = sysstat;
	
	__res_imsk();
	
	return E_OK;
}

