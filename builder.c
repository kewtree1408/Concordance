#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "ctype.h"
#include <sys/stat.h>
#include <sys/types.h>

#include "2-3_tree.h"
#include "other_funcs.h"
#include "builder.h"
#include "parsers.h"

int main (int argc, char** argv) {
	
	char **nfiles = (char **)malloc(sizeof(char*)*argc);
	if (argc<3) {
		printf ("ERROR: need more arguments:\nUSAGE: ./idx_builder idx-path file1 file2 ...\n");
		return 0;
	}
	
	char *idx_path = argv[1];
	char *end_path;
	
	mkdir(idx_path,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	
	int j;
	for (j=2; j<argc; j++) {
		end_path = scat(idx_path,file_name(argv[j]));
		nfiles[j-2] = end_path;
		if (!file_copy(argv[j],end_path)) { return 0; }
	}
	
	BB *bb = btree_create();
	btree_init(bb,NULL,nfiles,argc-2);
	
	printf ("\n! idx_builder started\n\n");
	int i;
	for (i=0; i<argc-2; i++) {
		bparser(bb,i);
	}
	
// 	btree_prn(bb);
	printf ("! idx_builder finished: total %d tokens, %d terms\n\n", bb->ntokens, bb->nterms);
	
	char *path = strcat(idx_path,"/\0");
	const char *ex1 = strcat(path, "total_index.idx\0");
	if (!tree_save(bb, ex1)) 
		printf ("ERROR: I can't save: %s\n",ex1);
	else 
		printf ("Successful save: %s\n",ex1);
	
	return 0;
}

