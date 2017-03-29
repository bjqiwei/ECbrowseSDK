//************************************************
// Module: TQueue Class Head File Define Here.
// Author: HuangYunYan
// Time  : 2000-08-12
// Modify: Modified On 2000.Created By:Mr.HYY.
//************************************************
#ifndef __TQUEUE_H__
#define __TQUEUE_H__

#if WINCE
#include <assert.h>
#include "TMutex.h"

template <class T>
class TQueue
{
private:
	TSemaphore* MaxLock;
	TSemaphore* MinLock;
	TMutex* MulLock;
	int MaxQueueSize;
	int Head;
	int Tail;
	int Count;
	T** Table;
public:
	TQueue(int MaxQueue);
	int Peek(T& Data);
	int Put(T& Data);
	int Put2(T& Data);
	int Get(T& Data);
	int GetCount();
	~TQueue();
};

template <class T>
TQueue<T>::TQueue(int MaxQueue):MaxQueueSize(MaxQueue),Head(0),Tail(0)
{
	Table = new T*[MaxQueue];
	assert(Table);
	for(int i = 0;i<MaxQueue;i++)
		Table[i] = (T*)0;
	Count = 0;
	MaxLock = new TSemaphore(MaxQueue);
	assert(MaxLock);
	MinLock = new TSemaphore(0);;
	assert(MinLock);
	MulLock = new TMutex();
	assert(MulLock);
}

template <class T>
int TQueue<T>::Peek(T& Data)
{
	MulLock->Lock();
	int Temp = 1;
	if(Table[Head]!=(T*)0)
	{
		Data = *Table[Head];
		Temp = 0;
	}
	MulLock->Unlock();
	return Temp;
}

template <class T>
int TQueue<T>::Put(T& Data)
{
	MaxLock->Wait();
	MulLock->Wait();
	int Temp = 1;
	if(Table[Tail]==(T*)0)
	{
		Table[Tail] = new T();
		assert(Table[Tail]);
		*Table[Tail] = Data;
		Tail = (Tail+1)%MaxQueueSize;
		Temp = 0;
		Count++;
	}
	MulLock->Post();
	MinLock->Post();
	return Temp;
}

template <class T>
int TQueue<T>::Put2(T& Data)
{
	MaxLock->Wait();
	MulLock->Wait();
	int Temp = 1;
	if(Table[Head]!=(T*)0)
	{
		if(Table[(Head-1)%MaxQueueSize] == (T*)0)
		{
			Head = (Head-1)%MaxQueueSize;
			Table[Head] = new T();
			assert(Table[Head]);
			*Table[Head] = Data;
			Temp = 0;
			Count++;
		}
	}
	else if(Table[Tail]==(T*)0)
	{
		Table[Tail] = new T();
		assert(Table[Tail]);
		*Table[Tail] = Data;
		Tail = (Tail+1)%MaxQueueSize;
		Temp = 0;
		Count++;
	}
	MulLock->Post();
	MinLock->Post();
	return Temp;
}
template <class T>
int TQueue<T>::Get(T& Data)
{
	MinLock->Wait();
	MulLock->Wait();
	int Temp = 1;
	if(Table[Head]!=(T*)0)
	{
		Data = *Table[Head];
		delete Table[Head];
		Table[Head] = (T*)0;
		Head = (Head+1)%MaxQueueSize;
		Temp = 0;
		Count--;
	}
	MulLock->Post();
	MaxLock->Post();
	return Temp;
}

template <class T>
int TQueue<T>::GetCount()
{
	MulLock->Wait();
	int i = Count;
	MulLock->Post();
	return i;
}

template <class T>
TQueue<T>::~TQueue()
{
	MulLock->Wait();
	for(int i = 0;i<MaxQueueSize;i++)
		if(Table[i])
			delete Table[i];
	delete[] Table;
	delete MaxLock;
	delete MinLock;
	MulLock->Post();
	delete MulLock;
}

#else
namespace stdvnc
{

template <class T>
class deque
{
private:
	int MaxQueueSize;
	int Head;
	int Tail;
	int Count;
	
public:
	T** Table;
	deque();
	int Peek(T& Data);
	int push_back(T& Data);
	int Get(T& Data);
	int GetCount();
	int size();
	~deque();
	T operator[] (int i);
};

template <class T>
deque<T>::deque()
{
	Head = 0;
	Tail = 0;
	MaxQueueSize = 100;

	Table = new T*[MaxQueueSize];
	for(int i = 0;i<MaxQueueSize;i++)
		Table[i] = (T*)0;
	Count = 0;
}

template <class T>
int deque<T>::Peek(T& Data)
{
	int Temp = 1;
	if(Table[Head]!=(T*)0)
	{
		Data = *Table[Head];
		Temp = 0;
	}
	return Temp;
}

template <class T>
int deque<T>::push_back(T& Data)
{
	int Temp = 1;
	if(Table[Tail]==(T*)0)
	{
		Table[Tail] = new T();
		*Table[Tail] = Data;
		Tail = (Tail+1)%MaxQueueSize;
		Temp = 0;
		Count++;
	}
	return Temp;
}

template <class T>
int deque<T>::Get(T& Data)
{
	int Temp = 1;
	if(Table[Head]!=(T*)0)
	{
		Data = *Table[Head];
		delete Table[Head];
		Table[Head] = (T*)0;
		Head = (Head+1)%MaxQueueSize;
		Temp = 0;
		Count--;
	}
	return Temp;
}

template <class T>
int deque<T>::GetCount()
{
	int i = Count;
	return i;
}

template <class T>
deque<T>::~deque()
{
	for(int i = 0;i<MaxQueueSize;i++)
		if(Table[i])
			delete Table[i];
	delete[] Table;
}

template <class T>
int deque<T>::size()
{
	return Count;
}

template <class T>
T deque<T>::operator[] (int i)
{
	return *Table[i];
}

}

#endif

#endif
