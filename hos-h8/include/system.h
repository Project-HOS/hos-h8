/* ------------------------------------------------------------------------- */
/*  HOS システム管理 ヘッダファイル                                          */
/*                                          Copyright (C) 1998-2000 by Ryuz  */
/* ------------------------------------------------------------------------- */

#ifndef __HOS__SYSTEM_H_
#define __HOS__SYSTEM_H_


/* ------------------------------------ */
/*             定数定義                 */
/* ------------------------------------ */

/* システムの状態 */
#define TSS_TSK  0x00		/* タスク部実行中 */
#define TSS_INDP 0x04		/* タスク独立部実行中 */
#define TSS_DDSP 0x01		/* ディスパッチ禁止 (dis_dsp 有効) */
#define TSS_DINT 0x02		/* 割り込み禁止 */
#define TSS_LOC  0x03		/* ディスパッチ＆割り込み禁止 (loc_cpu 有効 ) */



/* ------------------------------------ */
/*              型定義                  */
/* ------------------------------------ */

/* バージョン情報参照用 */
typedef struct t_ver {
	UH   maker;		/* ＯＳ製造メーカー */
	UH   id;		/* ＯＳ形式番号 */
	UH   spver;		/* ITRON・μITRON仕様書バージョン番号 */
	UH   prver;		/* ＯＳ製品バージョン番号 */
	UH   prno[4];	/* 製品番号、製品管理番号 */
	UH   cpu;		/* ＣＰＵ情報 */
	UH   var;		/* バリエーション記述子 */
} T_VER;

/* システム状態参照 */
typedef struct t_rsys {
	INT  sysstat;
} T_RSYS;



/* ------------------------------------ */
/*          グローバル変数              */
/* ------------------------------------ */

extern UH  sysstat;			/* システムの状態 */



/* ------------------------------------ */
/*             関数宣言                 */
/* ------------------------------------ */

ER   get_ver(T_VER *pk_ver);	/* バージョン参照 */
ER   ref_sys(T_RSYS *pk_rsys);	/* システム状態参照 */

/* タスク独立部用定義 */
#define iget_ver  get_ver
#define iref_sys  ref_sys


#endif /* __HOS__SYSTEM_H_ */
