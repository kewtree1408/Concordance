#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "ctype.h"

#include "2-3_tree.h"
#include "builder.h"
#include "other_funcs.h"
#include "list_query.h"
#include "parsers.h"

void bparser (BB *btree, int file) {	
	FILE *fp;	
	int ch;
	int line = 0;
	int sen = 0;
	int pos = 0;
	int shift = 0;
	char *word = (char*)malloc(sizeof(char)*80); // т.к. слово обычно не больше 80 символов
	int i = 0;
	int ntokens = 0;
	int state = INIT; // state of automat
	
	printf("! processing file %s\n",btree->fdname[file]);
	
	if ((fp = fopen(btree->fdname[file],"r"))==NULL) {
		printf ("ERROR: I can't open %s for reading\n",btree->fdname[file]);
		return;
	}
	
	while ((ch=getc(fp))!=EOF) {
 		if (i==80) {
 			char *tmp = (char *)realloc(word, 10000);
			word = tmp;
 		}
		if (state == INIT) {
			if (ch == '\n') {
				state = LF;
				continue;
			}
			if (ch == '.' || ch == '!' || ch == '?') {
				word[i]='.'; i++;
				word[i]='\0'; i++;
				shift = ftell(fp);
				add(btree,file,line,sen,pos,shift,word,i);
				ntokens++;
				memset(word,0,i*sizeof(char));			
				i=0;
				sen++;
				continue;
			}
			if (isalnum(ch)) {
				state = SB;
				word[i] = tolower(ch); // запомнили значащий символ
				i++;
				continue;
			}
			else {
				continue;
			}
		}
		if (state == LF) {
			if (/*isupper(ch)*/isalnum(ch) || ch == '\t' || ch == ' ') {
				word[i] = '\\'; i++;
				word[i] = '\0'; i++;
				shift = ftell(fp);
				line++;
				add(btree,file,line,sen,pos,shift,word,i);
				ntokens++;
				memset(word,0,i*sizeof(char));	
				i=0;
// 				line++; // есть абзац
			}
			if (isalnum(ch)) {
				word[i]=tolower(ch);
				i++;
				state = SB;
				continue;
			}
			else { // пропускам все остльные случаи и ищем начало абзаца (начало абзаца -- большая буква или цифра)
				state = INIT;
				continue;
			}
		}
		if (state == SB) {
			if (isalnum(ch) || ch == '-' || ch == '\'') {
				word[i]=tolower(ch);
				i++;
				continue;
			}
			else { // все остльные символы
				shift = ftell(fp);
				word[i] = '\0'; i++;
				add(btree,file,line,sen,pos,shift,word,i);
				ntokens++;
				memset(word,0,i*sizeof(char));
				i=0;
				pos++;
				if (ch == '\n') {
					state = LF;
					continue;
				}
				if (ch == '.' || ch == '!' || ch == '?') {
					word[i] = '.'; i++;
					word[i] = '\0'; i++;
					shift = ftell(fp);
					add(btree,file,line,sen,pos,shift,word,i);
					ntokens++;
					memset(word,0,i*sizeof(char));
					i=0;
					sen++;
					state = INIT;
					continue;
				}
				else {
					state = INIT;
					continue;
				}
			}
		}
		else {
			continue;
		}
	}
	printf ("! file %s processed: %d tokens, %d terms\n\n", btree->fdname[file], ntokens, btree->nterms);
}

void sparser (BB *btree, char fl, int N, char *idx_path) {
	Qlist *query = qlist_create();
	int i=0;
	unsigned int ch;
	char *word = (char*)malloc(sizeof(char)*80);
	int state = INIT;
	int numb_query = 0;
	int count_word = 0;
	while ((ch = getchar())!=EOF) {
		if (i>80) {
// 			printf ("I'm here\n");
 			char *tmp = (char *)realloc(word, 10000);
			word = tmp;
 		}
 		if (state == INIT) {
			if (ch == '\n') {
				word[i++] = '\0';
				if (!(isspace(word[0]) || word[0]=='\0' || ispoint(word[0]))) {
					qlist_insert(query,word,i);
					memset(word,0,i);
					count_word++;
					numb_query++;
				}
				i=0;
				query_find(btree,query,count_word,fl,numb_query,N);
// 				qlist_prn(query);
				qlist_clear(query);
				state = INIT;
				count_word=0;
				continue;
			}
			if (isalnum(ch)) {
				state = SB;
				word[i] = tolower(ch);
				i++;
				continue;
			}
			else {
				state = INIT;
				continue;
			}
		}
		if (state == LF) {
			word[i++] = '\0';
			qlist_insert(query,word,i);
			memset(word,0,i);
			i=0;
			count_word++;
			numb_query++;
			query_find(btree,query,count_word,fl,numb_query,N);
// 			qlist_prn(query);
			qlist_clear(query);
			state = INIT;
			count_word=0;
		}
		if (state == SB) {
			if (isalnum(ch) || ch =='\'' || ch == '-') {
				word[i++]=tolower(ch);
				continue;
			}
			if (ch == '\n') {
				word[i++] = '\0';
				qlist_insert(query,word,i);
				memset(word,0,i);
				i=0;
				count_word++;
				numb_query++;
				query_find(btree,query,count_word,fl,numb_query,N);
// 				qlist_prn(query);
				qlist_clear(query);
				state = INIT;
				count_word=0;
				continue;
			}
			else {
				word[i] = '\0'; i++;
				qlist_insert(query,word,i);
				memset(word,0,i);
				i=0;
				state = INIT;
				count_word++;
			}
		}
	}
	
	printf ("searching queries have finished\n");
	
}

void parser1(FILE *fp, int N, int *diff, bool st, int *ch) {
	int state = INIT;
	long int cur = ftell(fp);
// 	printf ("ch=%c, diff = %d\n",*ch, *diff);
//  	while ((*ch!=EOF) && (!isalnum(*ch))) {
// 		cur = ftell(fp);
// 		*ch = fgetc(fp);
// 	}
//  	fseek(fp,cur,SEEK_SET);
	while ((((*diff) > N) && (*ch=fgetc(fp)) != EOF)/* && ((*diff) > N)*/) {
		if (state == INIT) {
			if (isalnum(*ch)/* || ch == '-' || ch == '\''*/) {
				state = SB;
				if (st) {
					printf ("%c",*ch);
				}
				continue;
			}
			if (isspace(*ch)) {
				state = LF;
				if (st) {
					if (*ch == '\n') printf (" ");
					else printf ("%c",*ch);
				}
				continue;
			}
			if (ispoint(*ch)) {
				state = PT;
				if (st) {
					if (*ch == '\n') printf (" ");
					else printf ("%c",*ch);
				}
				continue;
			}
			else {
				state = OT;
				if (st) {
					printf ("%c",*ch);
				}
				continue;
			}
		}
		if (state == SB) {
			if (isalnum(*ch) || *ch == '-' || *ch == '\'') {
				if (st) {
					printf ("%c",*ch);
				}
				continue;
			}
			if (isspace(*ch)) {
				state = LF;
				(*diff)--;
				if (st) {
					if (*ch == '\n') printf (" ");
					else printf ("%c",*ch);
				}
				continue;
			}
			if (ispoint(*ch)) {
				state = PT;
				(*diff)--;
				if (st) {
					printf ("%c",*ch);
				}
				continue;
			}
			else {
				state = OT;
				(*diff)--;
				if (st) {
					printf ("%c",*ch);
				}
				continue;
			}
		}
		if (state == LF) {
			if (isalnum(*ch)/* || ch == '-' || ch == '\''*/) {
				state = SB;
				if (st) {
					printf ("%c",*ch);
				}
				continue;
			}
			if (isspace(*ch)) {
				if (st) {
					if (*ch == '\n') printf (" ");
					else printf ("%c",*ch);
				}
				continue;
			}
			if (ispoint(*ch)) {
				state = PT;
				if (st) {
					printf ("%c",*ch);
				}
				continue;
			}
			else {
				state = OT;
				if (st) {
					printf ("%c",*ch);
				}
				continue;
			}
		}
		if (state == PT) {
			if (isalnum(*ch)/* || ch == '-' || ch == '\''*/) {
				state = SB;
				if (st) {
					printf ("%c",*ch);
				}
				continue;
			}
			if (isspace(*ch)) {
				state = LF;
				if (st) {
					if (*ch == '\n') printf (" ");
					else printf ("%c",*ch);
				}
				continue;
			}
			if (ispoint(*ch)) {
				state = PT;
				if (st) {
					printf ("%c",*ch);
				}
				continue;
			}
			else {
				if (st) {
					printf ("%c",*ch);
				}
				continue;
			}
		}
		if (state == OT) {
			if (isalnum(*ch)/* || ch == '-' || ch == '\''*/) {
				state = SB;
				if (st) {
					printf ("%c",*ch);
				}
				continue;
			}
			if (isspace(*ch)) {
				state = LF;
				if (st) {
					if (*ch == '\n') printf (" ");
					else printf ("%c",*ch);
				}
				continue;
			}
			if (ispoint(*ch)) {
				state = PT;
				if (st) {
					printf ("%c",*ch);
				}
				continue;
			}
			else {
				if (st) {
					printf ("%c",*ch);
				}
				continue;
			}
		}
	}
// 	printf ("%c",*ch);
	//while ((ch!=EOF) && (isspace(ch))) ch=fgetc(fp);
}


// Выводим контекст
void word_output(FILE *fp, int N, int diff, int nquery) {
	int state = INIT;
	int ch = ' ';
// 	printf ("diff = %d\n",diff);
	parser1(fp, N, &diff, false, &ch);
// 	printf ("ch = %c\n;",ch);
// 	printf ("diff = %d\n",diff);
	diff = diff+N+nquery; // N+
	
	parser1(fp, 0, &diff, true, &ch);
	
}

void str_output(FILE *fp, int end) {
	int state = 1;
	int cpoint = 0;
	int ch;
	while ((ch=fgetc(fp)) != EOF) {
		if ((state == 1) && (isspace(ch) || ispoint(ch))) {
			continue;
		}
		if (ch == '\n') {
			state = 0;
			continue; 
		}
		if (ispoint(ch)) {
			state = 1;
			printf ("%c\n",ch);
			cpoint++;
			if (cpoint == end) { break; }
		}
		else {
			state = 0;
			printf("%c",ch);
		}
	}
}

void par_output(FILE *fp, int end) {
	int state = INIT;
	int ch;
	int cpar = 0;
//  	end++;
	
	while ((ch=fgetc(fp)) != EOF) {
		if (state == INIT) {
			if (isalnum(ch)) {
				state = SB;
				printf ("%c",ch);
				continue;
			}
			if (ch == '\n') {
				state = LF;
				printf ("%c",ch);
				continue;
			}
			else {
				state = OT;
				printf ("%c",ch);
				continue;
			}
		}
		if (state == SB) {
			if (isalnum(ch)) {
				printf ("%c",ch);
				continue;
			}
			if (ch == '\n') {
				state = LF;
				printf ("%c",ch);
				continue;
			}
			else {
				state = OT;
				printf ("%c",ch);
				continue;
			}
		}
		if (state == LF) {
			if (isalnum(ch) || ch == ' ' || ch == '\t') {
				state = SB;
				cpar++;
				if (cpar == end)
					break;
				printf ("\n");
			}
			if (ch == '\n') {
				printf ("%c",ch);
				continue;
			}
			else {
				state = OT;
				printf ("%c",ch);
				continue;
			}
		}
		if (state == OT) {
			if (isalnum(ch)) {
				state = SB;
				printf ("%c",ch);
				continue;
			}
			if (ch == '\n') {
				state = LF;
				printf ("%c",ch);
				continue;
			}
			else {
				printf ("%c",ch);
				continue;
			}
		}
	}
	
}


// void par_output(FILE *fp, int end) {
// 	int state = 1;
// 	int cpar = 0;
// 	int ch;
// 	while ((ch=fgetc(fp)) != EOF) {
// 		if ((state == 1) && (!(isalnum(ch) || ch == ' ' || ch == '\t'))) {
// 			continue;
// 		}
// 		if (ch == '\n') {
// 			state = 1;
// 			printf ("%c\n",ch);
// 			cpar++;
// 			if (cpar == end) { break; }
// 		}
// 		else {
// 			state = 0;
// 			printf("%c",ch);
// 		}
// 	}
// }
