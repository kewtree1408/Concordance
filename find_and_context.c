#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "assert.h"
#include "ctype.h"

#include "2-3_tree.h"
#include "list_query.h"
#include "find_and_context.h"

void query_prn(BB *btree, Stats *s) {
// 	printf ("! File %s, line %d, position %d:\n",file_name(btree->fdname[s->fd]),s->ln,s->pos);
	printf ("! File %s, line %d, position %d:\n",btree->fdname[s->fd],s->ln,s->pos);
}

void go_to_table(BB *btree, Node **mass_stat, Stats **pos_pointers, int count, char fl, Qlist *query, int nquery, int N) {
	Node *niter0 = mass_stat[0];
	Node *niterp;
	Stats *iter0 = mass_stat[0]->key.fvalue; // список у 1го слова-вхождения
	Stats *iterp = iter0;
	int p = 1;
	int state = 0;
	int cmp_fd, cmp_pos;
	int i;
	int count_p=0;

	assert(iter0->next!=niter0->key.evalue);
	while ((iter0 = iter0->next) != niter0->key.evalue) {
		cmp_fd = iter0->fd; // compared fd 
		cmp_pos = iter0->pos+1; // compared pos
		iterp = iter0;
		
		while (p<count) {
			niterp = mass_stat[p];
			iterp = pos_pointers[p];
			
			i = iterp->pos;
			while ((i <= cmp_pos) && ((iterp = iterp->next) != niterp->key.evalue)) {
				if (iterp->fd == cmp_fd && iterp->pos == cmp_pos) {
					state = 1; // нашли одно из вхождений
					pos_pointers[p] = iterp;
					break;
				}
				i = iterp->pos;
			}
			if (state == 1) {
				cmp_pos++;
				p++;
				state = 0;
				continue;
			}
			else if (state == 0) {
				p = 1;
				break;
			}
		}
// 		printf ("p = %d\t count = %d\n",p,count);
		if (p == count) {
// 			printf ("OK! query find\n");
			query_prn(btree,iter0);
// 			query_prn(iterp);
 			def_context(btree,fl,iter0,iterp,query,p,N); // define context
			count_p++;
			p = 1;
			continue; // перейти к итерации с iter0
		}
//  		printf ("bad. no such query\n");
	}
	if (count_p == 0) {
		printf ("no such query in index_total\n");
	}
}

void query_find(BB *btree, Qlist *query, int count, char flag, int nquery, int N) {
	Node **mass_stat = (Node **)malloc(sizeof(Node*)*(count+1));
	Stats **pos_pointers = (Stats **)malloc(sizeof(Stats*)*(count+1));
	int i = 0;
	Elem *iter = query->first;
	
	printf ("! query %d execution: ", nquery);
	qlist_prn(query);
	printf ("\n");
	
	while (iter->next!=query->end) {
		if ((mass_stat[i] = btree_find(btree,iter->next->word)) != NULL) {
// 			printf ("I'm find s/th\n");
			pos_pointers[i] = mass_stat[i]->key.fvalue;
			i++;
		}
		iter = iter->next;
	}
	if (count == 0) {
		printf ("incorrect query\n");
		return;
	}
	if (i == count) {
//  		printf ("maybe ok!!!\n");
		go_to_table(btree,mass_stat,pos_pointers,count,flag,query,nquery,N);
	}
	else {
		printf ("no such query in index_total\n");
	}
}

// Ориентируемся по точке, если точки у нас нет, то считываем от начала файла
void context_word(BB *btree, Stats *wfind1, Stats *wfindn, Qlist *query, int nquery, int N) {
// 	printf ("context word\n");
	FILE *fp;
	Node *point = btree_find(btree,".\0");
	Stats *st_point = NULL;
	int diff = 0;
	char *file = btree->fdname[wfind1->fd];
	int nposq = wfindn->pos - wfind1->pos+1; // number of position at query
	
	if ((fp = fopen(file,"r"))==NULL) {
		printf ("No such file\n");
		return;
	}
	if (point == NULL) { // т.е. точка не найдена
		diff = wfind1->pos;// + 1; // - st->pos;
// 		printf ("1) pos point = %d\tpos wfind1 = %d\n",st_point->pos,wfind1->pos);
		word_output(fp,N,diff,nposq);
	}
	else { // point!=NULL
		Stats *iter = point->key.fvalue;
		Stats *iter1 = iter;
		while (iter->next != point->key.evalue) {
			if (wfind1->sn == iter->next->sn && wfind1->fd == iter->next->fd) {
				st_point = iter->next;
				break;
			}
			if (wfind1->fd == iter->next->fd) {
				iter1 = iter->next;
			}
			iter = iter->next;
		}
		if (st_point == NULL) { st_point = iter1; } 
		Stats *st_point1 = st_point;
		while (((diff = wfind1->pos - st_point1->pos) < N) && (st_point!=point->key.fvalue)) {
			if (wfind1->fd == st_point->prev->fd) {
				st_point1 = st_point->prev;
			}
			st_point = st_point->prev;
		}
		if (st_point == point->key.fvalue) { // дошли до начала файла
// 			if (wfind1->pos==0) {
// 				diff = wfind1->pos;
// 				nposq = 0;
// 			}
			diff = wfind1->pos;
			printf ("diff = %d\n",diff);
// 			printf("st_point pos = %d\n",st_point->pos);
// 			printf ("2) pos point = %d\tpos1 point = %d\tpos wfind1 = %d\tdiff = %d\n",st_point->fd,st_point1->fd,wfind1->pos,diff);
			word_output(fp,N,diff,nposq);
		}
		else {
// 			printf("st_point pos = %d\n",st_point->pos);
			fseek(fp,st_point1->shift,SEEK_SET);
// 			printf ("3) pos point = %d\tpos point1 = %d\tpos wfind1 = %d\tdiff = %d\n",st_point->fd,st_point1->fd,wfind1->pos,diff);
			word_output(fp,N,diff,nposq);
		}
	}
	printf ("\n");
	fclose(fp);
}

void context_str(BB *btree, Stats *wfind1, Stats *wfindn, Qlist *query, int nquery, int N) {
// 	printf ("context str\n");
	FILE *fp;
	Node *point = btree_find(btree,".\0");
	char *context = (char *)malloc(sizeof(char)*10000);
	Stats *st_point = NULL;
	char *file = btree->fdname[wfind1->fd];
	int nseq = wfindn->sn - wfind1->sn+1; //number of seq
	
	if ((fp = fopen(file,"r"))==NULL) {
		printf ("No such file\n");
		return;
	}
	if (point == NULL) { // т.е. точка не найдена
		while (1) {
			if (fscanf(fp,"%s",context) == EOF) { break; }
			printf ("%s ",context);
		}
		printf ("\n");
		fclose(fp);
		return;
	}
// 	else {
// 		Stats *iter = point->key.fvalue;
// 		Stats *iter1 = iter;
// 		while (iter->next != point->key.evalue) {
// 			if (wfind1->fd == iter->next->fd) {
// 				if (wfind1->sn == iter->next->sn) {
// 					st_point = iter->next;
// 					break;
// 				}
// 				iter1 = iter->next;
// 			}
// 			iter = iter->next;
// 		}
// 		if (st_point == NULL) { st_point = iter1; /*N--;*/ }
// 		if (st_point != NULL) {
// 			int n = N+1;
// 			Stats *st_point1 = st_point;
// 			while ((n != 0) && (st_point != point->key.fvalue)) {
// 				if (wfind1->fd == st_point->prev->fd) {
// 					st_point1 = st_point->prev;
// 					n--;
// 				}
// 				st_point = st_point->prev;
// 				//n--;
// 			}
// 			if ((st_point == point->key.fvalue)) {
// // 				printf ("2) st_point = %d %lu\n",st_point->pos, st_point->shift);
// 				str_output(fp,wfind1->sn+N+nseq);
// 			}
// 			else {
// // 				st_point = st_point->prev;
// // 				printf ("3) st_point = %d %lu\n",st_point1->pos, st_point1->shift);
// 				fseek(fp,st_point1->shift,SEEK_SET);
// 				str_output(fp,N+N+nseq);
// 			}
// 		}
// 	}
	else {
		Stats *iter = point->key.fvalue;
		Stats *iter1 = iter;
		while (iter->next != point->key.evalue) {
			if (wfind1->fd == iter->next->fd) {
				if (wfind1->sn-N-1 == iter->next->sn) {
					st_point = iter->next;
					break;
				}
				iter1 = iter->next;
			}
			iter = iter->next;
		}
		if ((st_point == NULL) || (st_point == point->key.fvalue)) {
// 			printf ("2) st_point = %d %lu\n",st_point->pos, st_point->shift);
			str_output(fp,wfind1->sn+N+nseq);
		}
		else {
// 			printf ("3) st_point = %d %lu\n",st_point->pos, st_point->shift);
			fseek(fp,st_point->shift,SEEK_SET);
			str_output(fp,N+N+nseq);
		}
	}
	fclose(fp);
}

void context_par(BB *btree, Stats *wfind1, Stats *wfindn, Qlist *query, int nquery, int N) {
// 	printf ("context paragraph\n");
	FILE *fp;
	Node *par = btree_find(btree,"\\");
	char *context = (char *)malloc(sizeof(char)*10000);
	Stats *st_par = NULL;
	char *file = btree->fdname[wfind1->fd];
	int nln = wfindn->ln - wfind1->ln+1;
	
	if ((fp = fopen(file,"r"))==NULL) {
		printf ("No such file\n");
		return;
	}
	if (par == NULL) { // LF not find
		while (1) {
			if (fscanf(fp,"%s",context) == EOF) { break; }
			printf ("%s ",context);
		}
		printf ("\n");
		fclose(fp);
		return;
	}
	else {
		Stats *iter = par->key.fvalue;
		Stats *iter1 = iter;
		while (iter->next != par->key.evalue) {
			if (wfind1->fd == iter->next->fd) {
				if (wfind1->ln-N == iter->next->ln) {
					st_par = iter->next;
					break;
				}
				iter1 = iter->next;
			}
			iter = iter->next;
		}
		if ((st_par == NULL) || (st_par == par->key.fvalue)) {
// 			printf ("2) st_point = %d %lu\n",st_point->pos, st_point->shift);
			par_output(fp,wfind1->ln+N+nln);
		}
		else {
// 			printf ("3) st_point = %d %lu\n",st_par->pos, st_par->shift);
			fseek(fp,st_par->shift-1,SEEK_SET);
			par_output(fp,N+N+nln);
		}
	}
	fclose(fp);
}

void def_context(BB *btree, char fl, Stats *find1, Stats *findn, Qlist *query, int nquery, int N) {
	switch (fl) {
		case 'w':
			context_word(btree,find1,findn,query,nquery,N);
			break;
		case 's':
			context_str(btree,find1,findn,query,nquery,N);
			break;
		case 'p':
			context_par(btree,find1,findn,query,nquery,N); //paragraph
			break;
	}
	printf ("\n");
}

