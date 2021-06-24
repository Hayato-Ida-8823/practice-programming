#ifndef _CSV_H
#define _CSV_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//次の入力行を読む
extern char *csvgetline(FILE *f);
//フィールドnを返す
extern char *csvfield(int n);
//フィールド数を返す
extern int csvnfield(void);

enum
{
	//メモリ不足のシグナル
	NOMEM = -2
};

//入力文字
static char *line = NULL;
//splitで使われる
static char *sline = NULL;
//line[]とsline[]のサイズ
static int maxline = 0;
//フィールドポインタ
static char **field = NULL;
//field[]のサイズ
static int maxfield = 0;
//field[]のフィールド数
static int nfield = 0;
//フィールドセパレータ文字
static char fieldsep[] = ",";

#endif