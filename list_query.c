#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "list_query.h"

//-----elem-----

Elem *elem_create() {
	Elem *el = (Elem *)malloc(sizeof(Elem));
	el->word = NULL;
	el->next = NULL;
	return el;
}

void elem_init(Elem *el, char *_word, int _sword) {
	el->word = (char *)malloc(sizeof(char)*_sword);
	strncpy(el->word,_word,_sword);
}

//-----

//-----qlist-----

Qlist *qlist_create() {
	Qlist *ql = (Qlist *)malloc(sizeof(Qlist));
	ql->first = (Elem *)malloc(sizeof(Elem));
	ql->end = (Elem *)malloc(sizeof(Elem));
	ql->first->prev = ql->end;
	ql->end->next = ql->first;
	ql->first->next = ql->end;
	ql->end->prev = ql->first;

	return ql;
}

void qlist_prn(Qlist *l) {
// 	printf ("query:\t");
	Elem *iter = l->first;
	while (iter->next!=l->end) {
		iter = iter->next;
		printf ("%s ",iter->word);
	}
	printf ("\n");
}

Elem *qlist_insert(Qlist *ql, char *word, int sword) {
	Elem *el = elem_create();
	elem_init(el,word,sword);
	
	el->next = ql->end;
	el->prev = ql->end->prev;
	ql->end->prev->next = el;
	ql->end->prev = el;

	return el;
}

void qlist_clear(Qlist *ql) {
	Elem *iter = ql->first->next;
	Elem *p, *k;
	while (iter!=ql->end) {
		p = iter->next;
		p->prev = iter;
		k = iter->prev;
		k->next = p;
		free(iter);
		iter = p;
		iter->prev = k;
	}
}

//-----
