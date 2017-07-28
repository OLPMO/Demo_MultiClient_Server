#ifndef _SERV_QUEUE_H_
#define _SERV_QUEUE_H_

#include <memory.h>

// ����ģ�� - �������� / ������
template<class T, int SIZE> class ServQueue
{
public:

	void Push(T &data); // ���

	T Pop(void); // ����

	T Front(void); // ����Ԫ��

	int Size(void); // Ԫ������

	bool Empty(void); // �Ƿ�Ϊ��

	void Clear(void); // ���

public:

	ServQueue() : head(0), tail(0), size(0) {}
	~ServQueue() {}

protected:

	T list[SIZE];

	int head, tail, size;

};

// ���
template<class T, int SIZE> 
inline void ServQueue<T, SIZE>::Push(T &data)
{
	int next = (tail + 1) % SIZE;
	if (next != head)
	{
		memcpy(&list[tail], &data, sizeof(T));
		tail = next;
		size++;
	}
	
}


// ����
template<class T, int SIZE>
inline T ServQueue<T, SIZE>::Pop(void)
{
	if (Empty()) return 0;
	
	int tmp = head;
	head = (head + 1) % SIZE;
	size--;

	return list[tmp];
}


// ����Ԫ��s
template<class T, int SIZE>
inline T ServQueue<T, SIZE>::Front(void)
{
	if (size == 0) return 0;
	return list[head];
}


// ���
template<class T, int SIZE>
void ServQueue<T, SIZE>::Clear(void)
{
	head = tail = size = 0;
}


// Ԫ������
template<class T, int SIZE>
int ServQueue<T, SIZE>::Size(void)
{
	return size;
}


// �Ƿ�Ϊ��
template<class T, int SIZE>
bool ServQueue<T, SIZE>::Empty(void)
{
	return (size == 0);
}


#endif