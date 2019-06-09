#include<bufferManager/bufferManager.h>
#include<cstdio>

extern errorReporter * errorHandler;

const int bufferManager::maxBlockNum = bufferMaxSize;  // the buffer can hold at most maxBlockNum blocks.b

bufferManager::bufferManager()
{
	blockCnt = 0;
	pilarHead = new bufferNode(NULL);
	pilarTail = new bufferNode(NULL);
	pilarHead->last = pilarHead;
	pilarHead->next = pilarTail;
	pilarTail->next = pilarTail;
	pilarTail->last = pilarHead;
}

bufferManager::~bufferManager()
{
	while (pilarHead->next != pilarTail)
		delete pilarHead->next;
	delete pilarHead;
	delete pilarTail;
}

Block * bufferManager::getBlock(const string fileName, int id)
{
	string blockName = fileName + "_" + to_string(id);
	if (nodeMap.count(blockName) != 0)
	{
		bufferNode * temp = nodeMap[blockName];
		acessNode(temp);
		return temp->block;
	} 

	/*
		if node has already existed in the buffer, return directly. 
		As it is acessed again, move it to the tail of the queue,
		thus it will be kept in the buffer longer.
	*/
	
	//ELSE : we must retrive the block from the file system.
	if (blockCnt == maxBlockNum)  // If the buffer is full, free the least rencently used node.
	{
		bufferNode * temp = pilarHead->next;

		while (temp != pilarTail && temp->block->pin)
			temp = temp->next;

		if (temp == pilarTail)
		{
			errorHandler->reportErrorCode(0); // error code 0 :  unexpected buffer explosion.
		}
		else
		{
			deleteNode(temp);
		}
	}
	return loadBlock(fileName,id);
}

bool bufferManager::acessNode(bufferNode * x)
{
	x->remove();
	pilarTail->last->next = x;
	x->last = pilarTail->last;
	pilarTail->last = x;
	x->next = pilarTail;
	return true;
}  
// After accessing a node, this node should be placed at the tail of the queue so that it won't be deleted first.

bool bufferManager::writeBlock(const string fileName, int id, const char *data)
{
	Block *block = getBlock(fileName, id);
	return writeBlock(block, data);
}
bool bufferManager::writeBlock(Block * block, const char *data)
{
	memcpy(block->data, data, blockSize);
	block->changed = true;
	string filePath = ("data/" + block->fileName + ".mdb");
	FILE * file = fopen(filePath.c_str(), "rb+");
	if (file == NULL)
	{
		errorHandler->reportErrorCode(2);
	}
	string x;
	fseek(file, block->id*blockSize, SEEK_SET);
	fwrite(block->data, blockSize, 1, file);
	return fclose(file);
}
//Use the writeThrough strategy to ensure the consistency.

Block * bufferManager::loadBlock(const string fileName,int id)
{
	Block * block = new Block(fileName,id);
	string filePath = ("data/" + fileName + ".mdb");
	FILE * file = fopen(filePath.c_str(), "rb");
	if (file == NULL)
	{
		errorHandler->reportErrorCode(1);
	}

	fseek(file, id*blockSize, SEEK_SET);
	fread(block->data, blockSize, 1, file);
	fclose(file);
	// read the data.
	
	bufferNode * temp = new bufferNode(block);
	acessNode(temp);
	nodeMap[fileName + "_" + to_string(id)] = temp;
	
	blockCnt++;
	return block;
}

bool bufferManager::deleteNode(bufferNode* x)
{
	delete x;
	string nodeName = x->block->fileName + "_" + to_string(x->block->id);
	nodeMap.erase(nodeName);
	blockCnt--;
	return true;
}