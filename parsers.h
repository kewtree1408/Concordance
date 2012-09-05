#ifndef _PARSERS_H_
#define _PARSERS_H_

#define INIT 0
#define LF 1 // LINE FEED
#define PT 2 // POINT
#define SB 3 // SYMB
#define OT 4 // OTHER

void bparser (BB *, int );
void sparser (BB *, char , int , char *);

// Выводим контекст
void word_output(FILE *, int , int , int );
void str_output(FILE *, int );
void par_output(FILE *, int );


#endif