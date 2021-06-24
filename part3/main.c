#include <stdio.h>
#include <string.h>

//global変数、番兵の役割をする
char NONWORD[] = "\n";

enum
{
	//プレフィクスの語数
	NPREF = 2,
	//状態ハッシュテーブル配列のサイズ
	NHASH = 4093,
	//生成される単語数の上限
	MAXGEN = 10000,
	//ハッシュ関数の乗数 37でもいい。
	MULTIPLIER = 31,
};

typedef struct State State;
typedef struct Suffix Suffix;

//プレフィクス＋サフィクスリスト
struct State{
	//プレフィクスの単語、二次元配列
	char *pref[NPREF];
	//サフィックスリスト
	Suffix *suf;
	// ハッシュテーブル中の次の要素
	State *next;
};

//サフィックスリスト
struct Suffix{
	//Suffix
	char *word;
	//サフィックスリスト中の次の要素
	Suffix *next;
};

//状態のハッシュテーブル
State *statetab[NHASH];

//hash: NPREF個の文字列からなる配列のハッシュ値を計算
unsigned int hash(char *s[NPREF])
{
	unsigned int h;
	unsigned char *p;
	int i;

	h = 0;
	i = 0;
	while (i < NPREF)
	{
		p = (unsigned char *)s[i];
		while (*p != '\0')
		{
			h = MULTIPLIER * h + *p;
			p++;
		}
		i++;
	}
	return (h % NHASH);
}

//lookup:プレフィクスを検索、指定されれば、それを生成。
//見つかるか、生成したらポインタを返す、それ以外はNULLを返す

State *lookup(char *prefix[NPREF], int create)
{
	int i;
	int h;
	State *sp;

	h = hash(prefix);
	sp = statetab[h];
	while (sp != NULL)
	{
		i = 0;
		while (i < NPREF)
		{
			if (strcmp(prefix[i], sp->pref[i]) != 0)
				break;
			i++;
		}
		if (i == NPREF)
			return (sp);
		sp = sp->next;
	}
	if (create)
	{
		sp = (State *)malloc(sizeof(State));
		i = 0;
		while (i < NPREF)
		{
			sp->pref[i] = prefix[i];
			i++;
		}
		sp->suf = NULL;
		sp->next = statetab[h];
		statetab[h] = sp;
	}
	return (sp);
}
//addsuffix:状態に追加、あとでサフィックスが変化してはならない
void addsuffix(State *sp, char *suffix)
{
	Suffix *suf;
	suf = (Suffix *)malloc(sizeof(Suffix));
	suf->word = suffix;
	suf->next = sp->suf;
	sp->suf = suf;
}

//add:単語をサフィックスリストに追加し、プレフィクスを更新
void add (char *prefix[NPREF], char *suffix)
{
	State *sp;
	sp = lookup(prefix, 1);
	addsuffix(sp, suffix);
	//プレフィクス中の単語を前にずらす
	memmove(prefix, prefix + 1, (NPREF - 1) * sizeof(prefix[0]));
	prefix[NPREF - 1] = suffix;
}

//build:入力を読み、プレフィクステーブルを作成する
void build (char *prefix[NPREF], FILE *f)
{
	char buf[100];
	char fmt[10];

	sprintf(fmt, "%%%ds", sizeof(buf) - 1);
	while (fscanf(f,fmt, buf) != EOF)
		add(prefix, strdup(buf));
}

//generate:一行に一語ずつ出力を生成
void generate(int nwords)
{
	State *sp;
	Suffix *suf;
	char *prefix[NPREF];
	char *w;
	int i;
	int nmatch;

	i = 0;
	while (i < NPREF)
	{
		prefix[i] = NONWORD;
		i++;
	}
	i = 0;
	while (i < nwords)
	{
		sp = lookup(prefix, 0);
		nmatch = 0;
		suf = sp->suf;
		while (suf != NULL)
		{
			if (rand() % ++nmatch == 0)
				w = suf->word;
			suf = suf->next;
		}
		if (strcmp(w, NONWORD) == 0)
			break;
		printf("%s\n", w);
		memmove(prefix, prefix + 1, (NPREF - 1) * sizeof(prefix[0]));
		i++;
	}
}

//main
int main(void)
{
	int i;
	int nwords = MAXGEN;
	char *prefix[NPREF];
	
	i = 0;
	while(i < NPREF)
	{
		prefix[i] = NONWORD;
		i++;
	}
	build(prefix, stdin);
	add(prefix, NONWORD);
	generate(nwords);
	return (0);
}