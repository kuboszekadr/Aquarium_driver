#include "ReadingsQueue.h"

// returns first element from the queue
Reading ReadingsQueue::pop()
{
    if (_size == 0) { return; } // return nothing if queue is empty
    
    Reading r = _queue[_front];
    _front = (_front + 1) % MAX_SIZE;
    _size--;

    return r;
}

// adds new reading in the end of the list
bool ReadingsQueue::add(Reading *reading)
{
    if (_size == MAX_SIZE) { return false; }

    _size++;
    _queue[_rear] = *reading;
    _rear = (_rear + 1) % MAX_SIZE;

    return true;
}

bool ReadingsQueue::is_empty()
{
    return (_size == 0);
}

bool ReadingsQueue::is_full()
{
    return (_size == MAX_SIZE);
}

// returns amount of elements in the queue 
int ReadingsQueue::size()
{
    return _size;
}

