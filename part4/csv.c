#include "csv.h"

//reset:変数を最初の値に戻す
static void reset(void)
{
	free(line);
	free(sline);
	line = NULL;
	sline = NULL;
	field = NULL;
	maxline = maxfield = nfield = 0;
}


//advquited:クォーとでくくられたフィールド：次のセパレータのポインタを返す
static char *advquoted(char *p)
{
	int i;
	int j;
	i = 0;
	j = 0;
	while (p[j] != '\0')
	{
		if (p[j] == '"' && p[++j] != '"')
		{
			int k = strcspn(p + j, fieldsep);
			memmove(p + i, p + j, k);
			i += k;
			j += k;
			break;
		}
		p[i] = p[j];
		i++;
		j++;
	}
	p[i] = '\0';
	return (p + j);
}

//split : 行をフィールド単位の分割する
static int split(void)
{
	char *p;
	char **newf;
	char *sepp;
	int sepc;

	nfield = 0;
	if (line[0] == '\0')
		return (0);
	strcpy(sline, line);
	p = sline;
	do
	{
		if (nfield >= maxfield)
		{
			maxfield *= 2;
			newf = (char **)realloc(field, maxfield * sizeof(field[0]));
			if (newf == NULL)
				return NOMEM;
			field = newf;
		}
		if (*p == '"')
			sepp = advquoted(++p);
		else
			sepp = p + strcspn(p, fieldsep);
		sepc = sepp[0];
		sepp[0] = '\0';
		field[nfield++] = p;
		p = sepp + 1;
	} while (sepc == ',');
	return (nfield);
}

//endofline: \r, \n, \r\n, EOFをチェックして捨てる
static int endofline(FILE *fin, int c)
{
	int eol;
	eol = (c == '\r' || c == '\n');
	if (c == '\n')
	{
		c = getc(fin);
		if (c != '\n' && c != EOF)
		{
			ungetc(c, fin);
		}
	}
	return (eol);
}

//csvfield: n番目のフィールドのポインタを返す
char *csvfield(int n)
{
	if (n < 0 || n >= nfield)
		return (NULL);
	return (field[n]);
}

//csvnfield: フィールド数を返す
int csvnfield(void)
{
	return (nfield);
}

//csvgetline:一行取得し、必要に応じて伸張

char *csvgetline(FILE *fin)
{
	int i;
	int c;
	char *newl;
	char *news;

	if (line == NULL)
	{
		maxline = maxfield = 1;
		line = (char *)malloc(maxline);
		sline = (char *)malloc(maxline);
		field = (char **)malloc(maxfield * sizeof(field[0]));
		if (line == NULL || sline == NULL || field == NULL)
		{
			reset();
			return (NULL);
		}
	}
	i = 0;
	while ((c = getc(fin)) != EOF && !endofline(fin, c))
	{
		if (i >= maxline - 1)
		{
			maxline *= 2;
			newl = (char *)realloc(line, maxline);
			news = (char *)realloc(sline, maxline);
			if (newl == NULL || news == NULL)
			{
				reset();
				return (NULL);
			}
			line = newl;
			sline = news;
		}
		line[i] = c;
		i++;
	}
	line[i] = '\0';
	if (split() == NOMEM)
	{
		reset();
		return (NULL);
	}
	return (c == EOF && i == 0 ? NULL : line);
}

int main(void)
{
	int i;
	char *line;
	while ((line = csvgetline(stdin)) != NULL)
	{
		printf("line = %s\n", line);
		i = 0;
		while (i < csvnfield())
		{
			printf("field[%d] = %s\n", i, csvfield(i));
			i++;
		}
	}
	return (0);
}