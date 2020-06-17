// StackQueueList.h
//

#pragma once

#include "MultiLinkList.h"

template<typename T>
class StackQueueList {
public:
	StackQueueList(int index);
	~StackQueueList();

	MultiLinkList<T> link;

	int empty();

	void add(T * link);
	T * remove(T * link);
	T * get(int pos);

	void push(T * link);
	T * pop();
	T * peek();

	void inqueue(T * link);
	T * dequeue();

	void clear();
};


template<typename T>
StackQueueList<T>::StackQueueList(int index) :
link(index) {

}

template<typename T>
StackQueueList<T>::~StackQueueList() {

}

template<typename T>
void StackQueueList<T>::clear() {
	while (!this->empty()) {
		delete this->pop();
	}
}

template<typename T>
int StackQueueList<T>::empty() {
	return !(this->link.linkcount > 0);
}

template<typename T>
void StackQueueList<T>::push(T * link) {
	this->link.insertLink(link);
}

template<typename T>
T * StackQueueList<T>::pop() {
	return this->link.removeLink(this->link.prev(this->link.link));
}

template<typename T>
T * StackQueueList<T>::peek() {
	return this->link.prev(this->link.link);
}

template<typename T>
void StackQueueList<T>::inqueue(T * link) {
	this->link.insertLink(link);
}

template<typename T>
T * StackQueueList<T>::dequeue() {
	return this->link.removeLink(this->link.link);
}

template<typename T>
void StackQueueList<T>::add(T * link) {
	this->link.insertLink(link);
}

template<typename T>
T * StackQueueList<T>::remove(T * link) {
	return this->link.removeLink(link);
}

template<typename T>
T * StackQueueList<T>::get(int index) {
	return this->link.getPos(index);
}