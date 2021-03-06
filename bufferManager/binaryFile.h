#ifndef binaryFile_H
#define binaryFile_H
#include<table/Table.h>
#include<config.h>
#include<vector>
class binaryFile
{
public:
	static bool writeTableValue(char *p, vector<tableValue>* list, Table* table);
	static bool readTableValue(char *p, vector<tableValue>* list, Table *table);
	static bool writeInt(char *p, int x);
	static bool writeFloat(char *p, float x);
	static bool writeChar(char *p, char *data, int n);
	static bool readInt(const char *p, int *x);
	static bool readFloat(const char *p, float *x);
	static bool readChar(const char *p, char *data, int n);
};
#endif // !binaryFile_H
