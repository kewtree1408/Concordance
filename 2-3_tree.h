#ifndef _2_3_THREE_H_
#define _2_3_THREE_H_

typedef struct stats Stats;
typedef struct item itemtype;
typedef struct node Node;
typedef struct btree23 BB; // Bynary B-tree

struct stats {
  int fd; 
	int ln; // 	int line;
	int sn; // 	int sentense;
	int pos; // 	int position;
	long int shift;
	
	Stats *next;
  Stats *prev;
};

struct item {
  char *term; // word
  int sterm;
	Stats *fvalue; // список
	Stats *evalue;
};

struct node {
	itemtype key;
	Node *left, *right;
	bool h; //horizontal link to right brother
};

struct btree23 {
	Node* root;
	char **fdname; // глобальный массив соответсвий номера файла с названием файла
	int nfds;
	int nterms;
	int ntokens;
};

//-----stats-----
void stat_prn(Stats *);
Stats *stat_create();
void stat_init(Stats *, int , int , int , int , int );
Stats *stat_insert(Stats *, Stats *, Stats *);
void stat_free(Stats *);
//-----

//-----item-----
void item_prn(itemtype *);
itemtype *item_create();
void item_init(itemtype *, char *, Stats *, int );
void item_free(itemtype *);
//-----

//-----node-----
void node_prn(Node* , int );
Node *node_create();
void node_init(Node *, itemtype *);
void node_free(Node *);
//-----

//-----2-3_tree-----
void walk_direct (Node* , int , void(*apply)(Node*, int));
void btree_prn(BB *);
BB *btree_create();
void btree_init(BB *, Node *, char **, int );
//-----

//-----splitters-----
void split_1(Node** );
void split_2(Node** );
void split_3(Node** );
void split_4(Node** );
//-----

//-----insert_2-3_tree-----
bool stop;
bool state;
Node *node_insert(BB *, Node** , itemtype*) ;
Node *btree_insert(BB* , itemtype* );
//-----

//-----find_2-3_tree-----
Node* node_find(Node* , char* );
Node* btree_find(BB* , char* );
//-----

//-----save_2-3_tree
void walk_save(FILE *, Node* );
bool tree_save (BB *, const char* );
//-----

//-----load_23tree-----
bool incor;
Node* walk_load(FILE *);
bool tree_load (BB *, const char *);
//-----

#endif
