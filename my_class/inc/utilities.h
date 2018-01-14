#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include <limits.h>

//#define LEAKDETECT 1
/*LEAKDETECT 0 :  NORMAL*/
/*LEAKDETECT 1 :  VERBOSE*/

#define KNRM  "\x1B[0m"
#define KBLK  "\x1B[30m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT "\x1B[37m"

#define BNRM  "\x1B[0m"
#define BBLK  "\x1B[40m"
#define BRED  "\x1B[41m"
#define BGRN  "\x1B[42m"
#define BYEL  "\x1B[43m"
#define BBLU  "\x1B[44m"
#define BMAG  "\x1B[45m"
#define BCYN  "\x1B[46m"
#define BWHT "\x1B[47m"

#define RESET  "\x1B[0m"

#define LONGBAR "================================================================================================\n"
#define LONGLINE "------------------------------------------------------------------------------------------------\n"
#define LONGBAR128 "================================================================================================================================\n"
#define LONGLINE128 "--------------------------------------------------------------------------------------------------------------------------------\n"

#define DELETELINE "\033[F\033[2K"
#ifdef LEAKDETECT
void LeakDetector_set(FILE *fp);
void LeakDetector_printall(void);
void LeakDetector_finish(void);
#endif

/*以下のメモリ操作関数を使えばLEAKDETECTをdefineすることで、メモリの使用を追跡してメモリリークが無いかチェックできる。*/
/*メモリ操作関数：allocate,callocate,reallocate,deallocate*/
/*動作はmalloc,calloc,realloc,freeと同様*/
/*
使い方：
	プログラム先頭
#ifdef LEAKDETECT
	LeakDetector_set(fp) : メモリの使用結果をダンプするファイルのファイルポインタをセットする。
#endif
	プログラムの要所
#ifdef LEAKDETECT
	LeakDetector_printall() : メモリの使用状況を出力
#endif
	プログラム終了
#ifdef LEAKDETECT
	LeakDetector_finish()：リークディテクタの使用を終了
#endif
*/
#define allocate(size) allocate_imp(size,__func__)
#define callocate(n,size) callocate_imp(n,size,__func__)
#define reallocate(ptr,size) reallocate_imp(ptr,size,__func__)

void *allocate2(size_t size);
void *callocate2(size_t n, size_t size);
void *reallocate2(void *ptr, size_t oldsize, size_t size);
void deallocate2(void *ptr, size_t size);
void deallocate(void *ptr);

#define inputfile(filein,buflen) INPUTFILE(filein,buflen)
#define inputdir(filein,buflen) INPUTDIR(filein,buflen)
#define outputfile(filein,buflen) OUTPUTFILE(filein,buflen)
#define query(message,buf,buflen) QUERY(message,buf,buflen)
#define loadfile(file,len,prompt) \
puts(prompt);\
fgets(file,len,stdin);\
file[strlen(file)-1] ='\0'

void printface(char *name, char *author, char *version, char *etc);
void *allocate_imp(size_t size, const char *funcname);
void *callocate_imp(size_t n, size_t size, const char *funcname);
void *reallocate_imp(void *ptr, size_t size, const char *funcname);

FILE *wopen(char *path);
void printerr(char *message, char *funcname);
void printbit(void *p, int length);

void putstr(void *s);
char *ftoa(double a);
char *itoa(int a);
char *strhead(char *str);
char *strtail(char *str);
void strrev(char *str);
char *rstrtok(char *s, const char *delim);
char *strtok2(char *str,char **seps, int *len, int nsep);
char *strtok3(char *str,char **seps, int *len, int nsep, char **last);
int isInt(char *s);
int isFloat(char *s);
int ipow(int base, int exp);

int countsep(char *str, char *sep);
int countchr(char *str, char c);
int countRow(FILE *fp);
int countColumn(FILE *fp, int c, char *sep);
int countNumeric(char *file, char *sep);

void skipLine(FILE *fp);
void skipUntilMatch(FILE *fp, char *sep, int (*matcher)(char *s));
int skipSeparator(FILE *fp, char *sep);
void skipOneString(FILE *fp, char *sep);

typedef enum en_state{start,transition,accept}State;
State fsmstr(char c, char *str, int *len);

char *getFileExtension(char *filename);
char *getFileName(char *filename);
size_t getFileSizeBin(const char *filename);
size_t getFileSize(FILE *fp);
char *getSeparator(char *filename);

char *repeatedString(char c, int len);
char *String_tail(char *str);
char *String_endline(char *str);
int String_countstr(char *str, char *str2);
int String_countWordLine(char *str, char *str2);
int String_countchr(char *str, char c);
int String_countmatch(char *str, int (*matcher)(int c));

char *String_skipchr(char *str, char c, int n);
char *String_skipstr(char *str, char *str2, int n);
char *String_copy(const char *str);
char *String_ncopy(const char *str, int size);
char *String_char(const char c);
char *String_clip(char *s, char *e);
char *String_read(char *file);
char *String_fread(FILE *fp);
void String_write(char *file, char *buf);
void String_fwrite(FILE *fp, char *buf);
char *String_match(char *str, char *str2);
char *String_matchLine(char *str, char *sep, int (*matcher)(char *s));
char *String_getword(char *str, char *sep);
int String_countBlankLinesAtEnd(char *str);
void String_removeBlankLine(char *buf);
void String_replace(char *buf, char *str, char c); /*when characters in str are found in buf, that character is replaced by c*/
char *String_join(char *s1, char *s2);
int readFromStdin(char *str,int size);

char *String_stripdq(char *buf); /*return allocated string*/

#define BLOCKSIZE 256
#define BLOCKSIZE_MAX 65536
char *readAllocLine(FILE *fp);
char *readAllocBlock(FILE *fp, char *sep);
void freadString(FILE *fp, char *str, int maxlen, char *sep);
int freadLine(FILE *fp, char *buf, int maxlen);
int readFromStdin(char *str,int size);
void removeBlankLine(char *filename); 
void loadbar(unsigned int x, unsigned int n, unsigned int w);
void bomb(int size);
void dividedbyzero();
#define DBZ_LINE_DIVIDED 19
#define DBZ_LINE_BY 24
#define DBZ_LINE_ZERO 17
extern char dbz_divided[];
extern char dbz_by[];
extern char dbz_zero[];

void randomSeed(unsigned long);
double randomReal(void);
unsigned long randomInteger(void);

#define INPUTFILE(filein,buflen) {	\
if(argc >= 2){	\
	snprintf(filein,buflen,"%s",argv[1]); \
	}else{	\
		puts("input file name =");	\
		fgets(filein,buflen,stdin);	\
		filein[strlen(filein)-1] = '\0';	\
	}	\
}	

#define INPUTDIR(dirin,buflen) {	\
if(argc >= 2){	\
	snprintf(dirin,buflen,"%s",argv[1]); \
	}else{	\
		puts("input directory name =");	\
		fgets(dirin,buflen,stdin);	\
		dirin[strlen(dirin)-1] = '\0';	\
	}	\
}	

#define OUTPUTFILE(fileout,buflen) { \
if(argc >=3){ \
	snprintf(fileout,buflen,"%s",argv[2]); \
	}else{ \
		puts("output file name ="); \
		fgets(fileout,buflen,stdin); \
		fileout[strlen(fileout)-1] = '\0'; \
	} \
}

#define QUERY(message,buf,buflen) { \
	puts(message); \
	fgets(buf,buflen,stdin) \
	buf[strlen(buf)-1] = '\0'; \
}

#endif
