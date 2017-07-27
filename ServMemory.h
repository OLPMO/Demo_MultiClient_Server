#ifndef _SERV_MEMORY_H_
#define _SERV_MEMORY_H_

#include <mutex>

/*

	�ڴ��ģ��

	��һ�����������ݴ���һ���ڴ��

	�ɴ��������ٶȵ�ͬʱ�����ڴ�й©������

	Release����� - һ�ڴ��ڴ�������ͷŲ���

	new/ delete  5969 ms

	   �ڴ��     413  ms

*/

#define DEFAULT_INIT_NUM (256) // Ĭ�ϳ�ʼ������

#define MAX_NUM_BLKS (32) // ��������ڴ���

#define NUM_EACH_EXPAND (128) // ÿ������ĸ���



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

	T* Alloc(void); // �����

	void Release(void *blk); // �ͷſ�

	int Size(void); // ʣ������

	bool Expand(int num); // ����

private:

	void Initialize(int num);

	void Clear(void);

protected:

	T *memblks[MAX_NUM_BLKS];

	int numMemBlk, numFreeBlk; // ��������

	LIST_NODE_PTR freeblks; // ���п�����

};


// ��ʼ��
template<class T>
inline void ServMemory<T>::Initialize(int num)
{
	Expand(num);
}


// �����
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


// �ͷſ�
template<class T>
inline void ServMemory<T>::Release(void *blk)
{
	if (blk == nullptr) return;

	LIST_NODE_PTR node = (LIST_NODE_PTR)blk;
	node->next = freeblks;
	freeblks = node;
	numFreeBlk++;
}


// ʣ������
template<class T>
inline int ServMemory<T>::Size(void)
{
	return numFreeBlk;
}


// ����
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


// �ͷ��ڴ���ڴ�
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