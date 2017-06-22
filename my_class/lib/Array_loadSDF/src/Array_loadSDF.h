#ifndef ARRAY_LOAD_SDF_H
#define ARRAY_LOAD_SDF_H
Array Array_loadSDF(char *file, char *variable);
LinkedList Array_loadSDFList(char *file, int n, ...);
LinkedList Array_loadSDFList2(char *file, LinkedList namelist);
#endif
