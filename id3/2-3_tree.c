#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "assert.h"
#include "ctype.h"

#include "2-3_tree.h"

//-----stats-----

void stat_prn(Stats *s) {
	printf ("(fd:%d; ln:%d; sn:%d; pos:%d; shift:%lu); ",s->fd,s->ln,s->sn,s->pos,s->shift);
	return;
}

Stats *stat_create() {
  Stats *st = (Stats *)malloc(sizeof(Stats));
  st->fd = 0;
  st->ln = 0;
  st->sn = 0;
  st->pos = 0;
  st->shift = 0;
  st->next = NULL;
	st->prev = NULL;
  return st;
}

void stat_init(Stats *st, int _fd, int _ln, int _sn, int _pos, int _shift) {
  st->fd = _fd;
  st->ln = _ln;
  st->sn = _sn;
  st->pos = _pos;
  st->shift = _shift;
}

Stats *stat_insert(Stats *end, Stats *frst, Stats *st) {
	st->next = end;
	st->prev = end->prev;
	end->prev->next = st;
	end->prev = st;
  return st;
}

void stat_free(Stats *st) {
	free(st);
}

//-----

//-----item-----

void item_prn(itemtype *item) {
// 	printf ("\nPrint Node\n");
	printf ("<term: %s>\t<list: ",item->term);
	if (item->fvalue->next == item->evalue) {
		printf ("null\n");
		return;
	}
// 	printf ("Print Stats\n");
	Stats *iter = item->fvalue;
	while (iter->next != item->evalue) {
		stat_prn(iter->next);
		iter = iter->next;
	}
	printf (">\n");
	return;
}

itemtype *item_create() {
	itemtype *item = (itemtype*)malloc(sizeof(itemtype));
	item->term = NULL;
	item->fvalue = (Stats *)malloc(sizeof(Stats));
	item->evalue = (Stats *)malloc(sizeof(Stats));
	return item;
}

void item_init(itemtype *item, char *_term, Stats *_first, int _sterm) {
	item->sterm = _sterm;
	item->term = (char *)malloc(sizeof(char)*_sterm);
	strcpy(item->term,_term);
	item->fvalue->prev = item->evalue;
	item->evalue->next = item->fvalue;
	item->fvalue->next = item->evalue;
	item->evalue->prev = item->fvalue;
	
	stat_insert(item->evalue,item->fvalue,_first);
}

void item_free(itemtype *item) {
	free(item->term);
}

//-----

//-----node-----

void node_prn(Node* node, int depth) {
	int i;
	for (i=0; i<depth; i++)
		printf(" ");
	if (node==NULL)
		printf ("*\n");
    else {
				//printf ("sterm = %d\n", node->key.sterm);
        item_prn(&node->key);
        if (node->h)
            printf ("\nTRUE h\n");
    }
}

Node *node_create() {
	Node *n = (Node *)malloc(sizeof(Node));
	n->key.fvalue = (Stats *)malloc(sizeof(Stats));
	n->key.evalue = (Stats *)malloc(sizeof(Stats));
	n->key.fvalue->prev = n->key.evalue;
	n->key.evalue->next = n->key.fvalue;
	n->key.fvalue->next = n->key.evalue;
	n->key.evalue->prev = n->key.fvalue;
	n->key.term = NULL;
	n->key.sterm = 0;
	
	n->left = NULL;
	n->right = NULL;
	n->h = false;
  return n;
}

void node_init(Node *n, itemtype *item) {
	n->key.sterm = item->sterm;
	n->key.term = malloc(sizeof(char)*n->key.sterm);
	strcpy(n->key.term,item->term);
	stat_insert(n->key.evalue,n->key.fvalue,item->fvalue->next);
}

void node_free(Node *n) {
	Stats *iter = n->key.fvalue->next;
	Stats *p, *k;
	while (iter!=n->key.evalue) {
		p = iter->next;
		p->prev = iter;
		k = iter->prev;
		k->next = p;
		free(iter);
		iter = p;
		iter->prev = k;
	}
	free(n->key.fvalue);
	free(n->key.evalue);
	free(n->key.term);
	free(n);
}

//-----

void walk_direct (Node* node, int depth, void(*apply)(Node*, int)) {
	apply (node, depth);
	if (node!=NULL) {
		walk_direct(node->left, depth+1, apply);
		walk_direct(node->right, depth+1, apply);
	}
}

void btree_prn(BB *tree) {
	printf ("Print 2-3-tree\n");
	int i;
	printf ("numb of files: ");
	for (i=0; i<tree->nfds; i++) {
		printf ("(%d;%s) ",i,tree->fdname[i]);
	}
	printf ("tokens = %d\tterms = %d;\n",tree->ntokens,tree->nterms);
	printf ("\n\n");
	if (tree->root == NULL) {
		printf ("2-3-tree is empty\n");
		return;
	}
	walk_direct(tree->root, 0, node_prn);
}

BB *btree_create() {
	BB *bt = (BB *)malloc(sizeof(BB));
	bt->root = NULL;
	bt->fdname = NULL;
	bt->nfds = 0;
	bt->nterms = 0;
	bt->ntokens = 0;
	return bt;
}

void btree_init(BB *bt, Node *_root, char **_fdname, int _nfds) {
	bt->root = _root;
	bt->nfds = _nfds;
	bt->fdname = (char **)malloc(sizeof(char*)*_nfds);
	int i;
	for (i=0; i<_nfds; i++) {
		bt->fdname[i] = (char *)malloc(sizeof(char)*255);
		strcpy(bt->fdname[i],_fdname[i]);
	}
}

//-----splitters-----

void split_1(Node** cur) {
    Node* tmp;
    (*cur)->h=false;
    tmp=(*cur)->left;
    tmp->h=true;
    (*cur)->left=tmp->right;
    tmp->right=(*cur);
    (*cur)=tmp;
}

void split_2(Node** cur) {
    (*cur)->h=false;
}

void split_3(Node** cur) {
    (*cur)->h=true;
}

void split_4(Node** cur) {
    Node* tmp;
    tmp=(*cur)->right;
    (*cur)->h=false;
    (*cur)->right=tmp->left;
    tmp->left=(*cur);
    tmp->h=false;
    (*cur)=tmp;
}

//-----

bool stop;  //for exit from recursive-insert
bool state; //in insert: this link vertical or horizontal?

//-----insert_23tree-----

Node *node_insert(BB *bt, Node** cur, itemtype* data_key) {
  if (*cur==NULL) { //insert
		*cur=node_create();
		node_init(*cur,data_key);
// 		free(data_key);
    state=true;
    return *cur; //
	}
	int res_cmp=strcmp(data_key->term,(*cur)->key.term);
	
	if (res_cmp<0) {
		node_insert(bt,&(*cur)->left,data_key);
    if (stop) return *cur;
    if (state) {
      if (!(*cur)->h) {
         if (!(*cur)->left->h) {
            if (state) {
              split_1(cur);
              state=false;
              return *cur;
            }
         }
			}
      else {
				if (state) { 
					split_2(cur); 
					return *cur;
				}
			}
		}
	}
	else if (res_cmp>0) {
		node_insert(bt,&(*cur)->right,data_key);
	  if (stop) return *cur;
		if ((!(*cur)->h)) {
			if (!(*cur)->right->h) {
				if (state) {
					split_3(cur);
					state=false;
					return *cur;
				}
			}
		}
		else {
			if ((*cur)->right->h) {
				split_4(cur);
				state=true;
				return *cur;
			}
		}
	}
  else {
		bt->nterms--;
		stat_insert((*cur)->key.evalue,(*cur)->key.fvalue,data_key->fvalue->next);
		state = false;
		return *cur;
	}
	return *cur;
}

Node *btree_insert(BB* tree, itemtype* key) {
	assert(tree!=NULL);
	tree->nterms++;
	return node_insert(tree,&tree->root,key);
}

//-----

//-----find_23tree-----

Node* node_find(Node* cur, char* term) {
	if (cur==NULL) {
		return NULL;
	}
	int res_cmp=strcmp(term,cur->key.term);
	if (res_cmp<0) {
		return node_find(cur->left,term);
	}
	else if (res_cmp>0) {
		return node_find(cur->right,term);
	}
	else {
		return cur;
	}
	return NULL;
}

Node* btree_find(BB* tree, char* term) {
	assert(tree!=NULL);
	if (tree->root==NULL) {
		return NULL;
	}
	else return node_find(tree->root,term);
}

//-----

//-----save_23tree

void walk_save(FILE *fp, Node* node) {
  bool have=true;
  bool not_have=false;
	char begin = '<';
	
	int length = node->key.sterm;
	fwrite(&length,sizeof(int),1,fp);
	fwrite(node->key.term,sizeof(char)*length,1,fp);
	fwrite(&begin,sizeof(char),1,fp);
	Stats *iter = node->key.fvalue;
	while (iter->next != node->key.evalue) {
		fwrite(&have,1,1,fp);
		iter = iter->next;
		fwrite(&iter->fd,sizeof(int),1,fp);
		fwrite(&iter->ln,sizeof(int),1,fp);
		fwrite(&iter->sn,sizeof(int),1,fp);
		fwrite(&iter->pos,sizeof(int),1,fp);
		fwrite(&iter->shift,sizeof(long int),1,fp);
		
	}
	fwrite(&not_have,1,1,fp);
	
	if (node->left!=NULL) {
		fwrite(&have,1,1,fp);
		walk_save(fp,node->left);
	}
	else {
		fwrite(&not_have,1,1,fp);
	}

	if (node->right!=NULL) {
		fwrite(&have,1,1,fp);
		fwrite(&(node->h),1,1,fp); // for horisontal link
		walk_save(fp,node->right);
  }
  else {
		fwrite(&not_have,1,1,fp);
	}
}

bool tree_save (BB *tree, const char* file_name) {
	FILE *fp;
	bool not_have = false;
	bool have = true;
	
	if ((fp=fopen(file_name,"wb"))==NULL) {
		printf("ERROR: I can't open %s for writing\n",file_name);
		return false;
	}
	
	fwrite(&tree->nfds,sizeof(int),1,fp);
	if (tree->nfds!=0) {
		int i;
		for (i=0; i<tree->nfds; i++) {
			fwrite(tree->fdname[i],sizeof(char)*255,1,fp);
		}
	}
	
	if (tree->root==NULL) {
		not_have=false;
		fwrite(&not_have,1,1,fp);
		fclose(fp);
		return true;
	}
	else fwrite(&have,1,1,fp);

	walk_save(fp,tree->root);
	fclose(fp);
	return true;
}

//-----

//-----load_23tree-----

bool incor;

Node* walk_load(FILE *fp) {
	if (incor) 
		return NULL;
	
	char begin = '<';
	bool have;
  bool tmp;
	int lenght;
	Stats *st;
	Node *node = node_create();
  
	if (fread(&lenght,sizeof(int),1,fp)!=1) {
		incor=true;
		return NULL;
	}
	node->key.sterm = lenght;
	
	node->key.term = (char *)malloc(sizeof(char)*lenght);
	if (fread(node->key.term,sizeof(char)*lenght,1,fp)!=1) {
		incor=true;
		return NULL;
	}
	if (fread(&begin,sizeof(char),1,fp)!=1) {
		incor=true;
		return NULL;
	}
	while((fread(&have,1,1,fp)==1) && have) {
		st = stat_create();
		fread(&st->fd,sizeof(int),1,fp);
		fread(&st->ln,sizeof(int),1,fp);
		fread(&st->sn,sizeof(int),1,fp);
		fread(&st->pos,sizeof(int),1,fp);
		fread(&st->shift,sizeof(long int),1,fp);
		stat_insert(node->key.evalue,node->key.fvalue,st);
	}
	
	if (fread(&(tmp),1,1,fp)!=1) {
		incor=true;
		return NULL;
	}
	if (tmp) {
		node->left=walk_load(fp);
	}
	else {
		node->left=NULL;
 	}		
  if (fread(&(tmp),1,1,fp)!=1) {
		incor=true;
		return NULL;
	}
	
	if (tmp) {
		if (fread(&(node->h),1,1,fp)!=1) { // for horisontal link
			incor=true;
			return NULL;
		}
    node->right=walk_load(fp);
  }
  else {
		node->h=false;
		node->right=NULL;
  }
  
	return node;
}

bool tree_load (BB *tree, const char *file_name) {
	FILE *fp;
	incor=false;
	if ((fp=fopen(file_name,"rb"))==NULL) {
		printf("ERROR: I can't open %s for reading\n",file_name);
		return false;
	}

  bool tmp;
	
	if (fread(&tree->nfds,sizeof(int),1,fp)!=1) {
		fclose(fp);
		return false;
	}
	
	if (tree->nfds!=0) {
		tree->fdname = (char **)malloc(sizeof(char*)*(tree->nfds));
		if (tree->fdname==NULL) {
		 	printf ("error: nfds = %d\tIncorrect data in file\n",tree->nfds);
			tree->nfds = 0;
			return 0;
		}
		int i;
		for (i=0; i<tree->nfds; i++) {
			tree->fdname[i] = (char *)malloc(sizeof(char)*255);
			if (fread(tree->fdname[i],sizeof(char)*255,1,fp)!=1) {
				fclose(fp);
				return false;
			}
		}
	} else {
		fclose(fp);
		return false;
	}
	
	if (fread(&tmp,1,1,fp)!=1) {
		rewind(fp);
		tree->root = NULL;
		if (fread(&(tmp),1,1,fp)==1) {
			fclose(fp);
			if (!tmp) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			fclose(fp);
			return false;
		}
	}
	
	tree->root=walk_load(fp);
	fclose(fp);

	if (incor) 
		return false;
	return true;
}


//-----
