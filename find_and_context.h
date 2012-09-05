#ifndef _FIND_AND_CONTEXT_H_
#define _FIND_AND_CONTEXT_H_

void query_prn(BB *, Stats *);
void go_to_table(BB *, Node **, Stats **, int , char , Qlist *, int , int );
void query_find(BB *, Qlist *, int , char , int , int);

// Ориентируемся по точке, если точки у нас нет, то считываем от начала файла
void context_word(BB *, Stats *, Stats *, Qlist *, int , int );
void context_str(BB *, Stats *, Stats *, Qlist *, int , int );
void context_par(BB *, Stats *, Stats *, Qlist *, int , int );
void def_context(BB *, char , Stats *, Stats *, Qlist *, int , int );

#endif