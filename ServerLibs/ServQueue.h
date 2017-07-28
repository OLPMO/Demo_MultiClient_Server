#ifndef _SERV_QUEUE_H_
#define _SERV_QUEUE_H_

#include <memory.h>

// 队列模板 - 数据类型 / 最大个数
template<class T, int SIZE> class ServQueue
{
public:

	void Push(T &data); // 入队

	T Pop(void); // 出队

	T Front(void); // 队首元素

	int Size(void); // 元素数量

	bool Empty(void); // 是否为空

	void Clear(void); // 清空

public:

	ServQueue() : head(0), tail(0), size(0) {}
	~ServQueue() {}

protected:

	T list[SIZE];

	int head, tail, size;

};

// 入队
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


// 出队
template<class T, int SIZE>
inline T ServQueue<T, SIZE>::Pop(void)
{
	if (Empty()) return 0;
	
	int tmp = head;
	head = (head + 1) % SIZE;
	size--;

	return list[tmp];
}


// 队首元素s
template<class T, int SIZE>
inline T ServQueue<T, SIZE>::Front(void)
{
	if (size == 0) return 0;
	return list[head];
}


// 清空
template<class T, int SIZE>
void ServQueue<T, SIZE>::Clear(void)
{
	head = tail = size = 0;
}


// 元素数量
template<class T, int SIZE>
int ServQueue<T, SIZE>::Size(void)
{
	return size;
}


// 是否为空
template<class T, int SIZE>
bool ServQueue<T, SIZE>::Empty(void)
{
	return (size == 0);
}


#endif