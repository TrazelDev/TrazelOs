#pragma once

template <class T>
class Queue 
{
private:

public:
    Queue();
    bool isEmpty();
    bool isFull();
    void enqueue(T value);
    void dequeue();
    void display();
    Queue<T>& operator=(const Queue<T>& other)

    T front;
    T rear;

};