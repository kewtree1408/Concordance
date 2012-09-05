#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

#include "2-3_tree.h"
#include "list_query.h"
#include "searcher.h"
#include "parsers.h"

int main (int argc, char** argv) {
	
	if (argc<2) {
		printf ("ERROR: need more arguments:\nUSAGE: ./idx_searcher [flags] idx-path\n[flags]: '-w N', '-s N', '-p N'\n");
		return 0;
	}
	
	int ncontext = 0; // размер контекста
	char flag;
	char *idx_path;
	if (argv[1][0] == '-') { // нашли флаг
		flag = argv[1][1];
		if (!(flag == 'w' || flag == 's' || flag == 'p')) {
			printf ("ERROR: incorrect flag\n");
			return 0;
		}
	}
	else {
		flag='w';
	}
	if ((argv[2]!=NULL) && (isdigit(argv[2][0]))) {
		ncontext = atoi(argv[2]);
		idx_path = argv[3];
	}
	else {
		ncontext = 1;
		if (argv[2]!=NULL) {
			idx_path = argv[2];
		} else {
			idx_path = argv[1];
		}
	}
	
	
//  	term_input(flag,ncontext,idx_path);
	
	BB* bb = btree_create();
	
	char *path = strcat(idx_path,"/\0");
	const char *ex1 = strcat(path, "total_index.idx\0");
	
 	if (!tree_load(bb, ex1)) {
		printf ("ERROR: I can't load %s\n",ex1);
		return 0;
	}
 	
//  	btree_prn(bb);
	
	sparser(bb,flag,ncontext,idx_path);
	
	return 0;
}