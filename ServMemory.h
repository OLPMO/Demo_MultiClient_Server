#ifndef _SERV_MEMORY_H_
#define _SERV_MEMORY_H_

#include <mutex>

/*

	内存池模板

	对一定的类型数据创建一个内存池

	可大幅度提高速度的同时避免内存泄漏的问题

	Release版测试 - 一亿次内存申请和释放操作

	new/ delete  5969 ms

	   内存池     413  ms

*/

#define DEFAULT_INIT_NUM (256) // 默认初始化个数

#define MAX_NUM_BLKS (32) // 最大整块内存数

#define NUM_EACH_EXPAND (128) // 每次扩充的个数



typedef struct LIST_NODE_TYP
{
	LIST_NODE_TYP *next;
} LIST_NODE, *LIST_NODE_PTR;



template<class T>
class ServMemory
{
public:

	ServMemory() : numMemBlk(0), numFreeBlk(0) { Initialize(DEFAULT_INIT_NUM); }
	ServMemory(int num) : numMemBlk(0), numFreeBlk(0) { Initialize(num); }
	~ServMemory() { Clear(); }

public:

	T* Alloc(void); // 申请块

	void Release(void *blk); // 释放块

	int Size(void); // 剩余数量

	bool Expand(int num); // 扩充

private:

	void Initialize(int num);

	void Clear(void);

protected:

	T *memblks[MAX_NUM_BLKS];

	int numMemBlk, numFreeBlk; // 空闲数量

	LIST_NODE_PTR freeblks; // 空闲块链表

};


// 初始化
template<class T>
inline void ServMemory<T>::Initialize(int num)
{
	Expand(num);
}


// 申请块
template<class T>
inline T* ServMemory<T>::Alloc(void)
{
	if (freeblks == nullptr && Expand(NUM_EACH_EXPAND) == false)
		return false;

	LIST_NODE_PTR node = freeblks;
	freeblks = node->next;
	numFreeBlk--;

	return ((T*)node);
}


// 释放块
template<class T>
inline void ServMemory<T>::Release(void *blk)
{
	if (blk == nullptr) return;

	LIST_NODE_PTR node = (LIST_NODE_PTR)blk;
	node->next = freeblks;
	freeblks = node;
	numFreeBlk++;
}


// 剩余数量
template<class T>
inline int ServMemory<T>::Size(void)
{
	return numFreeBlk;
}


// 扩充
template<class T>
inline bool ServMemory<T>::Expand(int num)
{
	if (numMemBlk >= MAX_NUM_BLKS) return false;

	memblks[numMemBlk] = (T*)malloc(sizeof(T) * num);
	if (memblks[numMemBlk] == nullptr) return false;
	for (int i = 0; i < num; i++)
		Release(&memblks[numMemBlk][i]);

	numMemBlk++;

	return true;
}


// 释放内存池内存
template<class T>
inline void ServMemory<T>::Clear(void)
{
	for (int i = 0; i < numMemBlk; i++)
	{
		if (memblks[i]) delete memblks[i];
		memblks[i] = nullptr;
	}

	freeblks = nullptr;
	numFreeBlk = 0;
	numMemBlk = 0;
}


#endif