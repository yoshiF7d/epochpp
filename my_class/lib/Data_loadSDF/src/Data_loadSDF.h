#ifndef DATA_LOAD_SDF_H
#define DATA_LOAD_SDF_H
#include <LinkedList.h>
Data Data_loadSDF(char *file, char *variable);
LinkedList Data_loadSDFList(char *file, int n, ...);
LinkedList Data_loadSDFList2(char *file, LinkedList namelist);
#endif
