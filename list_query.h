#ifndef _LIST_QUERY_H_
#define _LIST_QUERY_H_

typedef struct elem Elem;
typedef struct qlist Qlist;

struct elem {
  char *word;
  Elem *next;
	Elem *prev;
};

struct qlist {
	Elem *first;
	Elem *end;
};

//-----elem-----
Elem *elem_create();
void elem_init(Elem *, char *, int );
//-----

//-----qlist-----
Qlist *qlist_create();
void qlist_prn(Qlist *);
Elem *qlist_insert(Qlist *, char *, int );
void qlist_clear(Qlist *);
//-----

#endif