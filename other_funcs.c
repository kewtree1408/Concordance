#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

#include "2-3_tree.h"
#include "other_funcs.h"

//other funcs
void add(BB *btree, int fd, int ln, int sn, int pos, long int shift, char *word, int wsize) {
	Stats *st = stat_create();	
	stat_init(st,fd,ln,sn,pos,shift);
	
	itemtype *item = item_create();
	item_init(item,word,st,wsize);
	
	btree_insert(btree,item);
	btree->ntokens++;
	
	item_free(item);
}

bool file_copy(char *in, char *out) {
   char command[512];
   sprintf(command, "cp %s %s", in, out);
   if (system(command)!=0)
		 return false;
	 return true;
}

char *file_name(char *nf) {
	int len = strlen(nf);
	char *rev = malloc(len);
	int i,j;
	int l1 = 0;
	for (i=len-1,j=0; i>=0; i--,j++) {
		if (nf[i]=='/') {
			break;
		}
		rev[j] = nf[i];
		l1++;
	}
	char *str = malloc(l1+1);
	for (i=0,j=l1-1; i<l1; i++,j--) {
		str[i] = rev[j];
	}
	str[i]='\0';
	free(rev);
	return str;
}

char *scat(char *fst, char *snd) {
	char *path = malloc(255);
	int i;
	for (i=0; fst[i]!='\0'; i++) {
		path[i] = fst[i];
	}
	path[i] = '/';
	int j,k;
	for (k=0,j=i+1; snd[k] != '\0'; k++,j++) {
		path[j] = snd[k];
	}
	path[j] = '\0';
	return path;
}

bool ispoint(int ch) {
	if (ch == '.' || ch == '?' || ch == '!') 
		return true;
	return false;
}

void term_input(char flag, int N, char *idx_path) {
	printf ("flag: %c",flag);
	printf ("\n");
	printf ("count of context = %d\n",N);
	printf ("the directory = %s\n",idx_path);
}