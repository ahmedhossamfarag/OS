#include "dslib.h"

#pragma region Queue

template<typename T>
Queue<T>::Queue(uint32_t capacity)
{
    this->capacity = capacity;
    this->data = new T[capacity];
    this->size = this->head = this->tail = 0;   
}

template<typename T>
Queue<T>::~Queue(){
    delete[] this->data;
}

template<typename T>
void Queue<T>::enque(T item)
{
    if (this-> size == this->capacity)
    {
        this->resize(this->capacity * 2);
    }

    if(this->size < this->capacity){
        if(this->size == 0){
            this->data[0] = item;
            this->head = this->tail = 0;
        }else{
            this->tail = (this->tail + 1) % this->capacity;
            this->data[this->tail] = item;
        }
        this->size ++;
    }
}

template<typename T>
T Queue<T>::deque()
{
    if(this->size){
        void *item = this->data[this->head];
        this->head = (this->head + 1) % this->capacity;
        this->size --;
        return item;
    }
    
    // TODO: throw exception
    while(1);
}

template<typename T> 
bool Queue<T>::any()
{
    return this->size > 0;
}

template<typename T>
bool Queue<T>::remove(T item)
{
    if(!this->size) return false;

    uint32_t this_end = this->tail >= this->head ? this->tail : this->tail + this->capacity;
    for (uint32_t i = this->head; i <= this_end; i++)
    {
        if(this->data[i%this->capacity] == item){
            for (uint32_t j = i; j < this_end - 1; j++)
            {
                this->data[j%this->capacity] = this->data[(j+1)%this->capacity];
            }
            this->size --;
            this->tail = (this_end - 1) % this->capacity;
            return true;
        }
    }
    return false;
}


template<typename T>
void Queue<T>::resize(uint32_t new_capacity)
{
    T* new_data = new T[new_capacity];

    for (uint32_t i = 0; i < this->size; i++)
    {
        uint32_t index = (this->head + i) % this->capacity;
        new_data[i] = this->data[index];
    }

    delete[] this->data;
    this->data = new_data;
    this->capacity = new_capacity;
    this->head = 0;
    this->tail = this->size - 1;
}

#pragma endregion

#pragma region Linked List

template<typename T>
ListNode<T>::ListNode(T data)
{
    this->data = data;
    this->next_node = nullptr;
}

template<typename T>
ListNode<T>::~ListNode()
{

}

template<typename T>
List<T>::List()
{
    this->head = this->tail = nullptr;
}

template<typename T>
List<T>::~List()
{
    ListNode<T>* node = this->head;
    while(node){
        ListNode<T>* next = (ListNode<T>*)node->next_node;
        delete node;
        node = next;
    }
}

template<typename T>
void List<T>::append(T item)
{
    ListNode<T>* node = new ListNode<T>(item);

    if(this->tail){
        this->tail->next_node = node;
        this->tail = node;
    }else{
        this->head = this->tail = node;
    }
}

template<typename T>
T List<T>::remove_first()
{
    if(this->head){
        T item = this->head->data;
        ListNode<T>* next = (ListNode<T>*)this->head->next_node;
        delete this->head;
        this->head = next;
        if(!next){
            this->tail = nullptr;
        }
        return item;
    }

    // TODO: throw exception
    while(1);
}

template<typename T>
bool List<T>::any(){
    return this->head != nullptr;
}

#pragma endregion

#pragma region Array

template<typename T>
Array<T>::Array(uint32_t capacity)
{
    this->capacity = capacity;
    this->size = 0;
    this->data = new T[capacity];
}

template<typename T>
Array<T>::~Array()
{
    delete[] this->data;
}

template<typename T>
bool Array<T>::contains(T item)
{
    for (T* p = this->data; p < this->data+this->size; p++) {
        if(*p == item){
            return true;
        }
    }

    return false;
}

template<typename T>
void Array<T>::add(T item)
{
    if(this->size == this->capacity){
        this->resize(this->capacity * 2);
    }
    if(this->size < this->capacity){
        this->data[this->size] = item;
        this->size ++;
    }
}

template<typename T>
void Array<T>::remove(T item)
{
    for (T* p = this->data; p < this->data+this->size; p++)
    {
        if(*p == item){
            for (T* q = p; q < this->data + this->size - 1; q++)
            {
                *q = *(q+1);
            }
            this->size --;
            return;
        }
    }
}

template<typename T>
T* Array<T>::begin()
{
    return this->data;
}

template<typename T>
T* Array<T>::end()
{
    return this->data + this->size;
}

template<typename T>
uint32_t Array<T>::count()
{
    return this->size;
}

template<typename T>
T& Array<T>::operator[](uint32_t index)
{
    return this->data[index];
}

template<typename T>
void Array<T>::resize(uint32_t new_capacity)
{
    T* new_data = new T[new_capacity];
    for (uint32_t i = 0; i < this->size; i++)
    {
        new_data[i] = this->data[i];
    }
    delete[] this->data;
    this->data = new_data;
    this->capacity = new_capacity;
}

#pragma endregion

#pragma region Stack

template<typename T>
Stack<T>::Stack(uint32_t capacity)
{
    this->capacity = capacity;
    this->data = new T[capacity];
    this->size = 0;
}

template<typename T>
Stack<T>::~Stack()
{
    delete[] this->data;
}

template<typename T>
void Stack<T>::push(T item)
{
    if(this->size == this->capacity){
        this->resize(this->capacity * 2);
    }
    this->data[this->size] = item;
    this->size ++;
}

template<typename T>
T Stack<T>::pop()
{
    if(this->size){
        T item = this->data[this->size - 1];
        this->size --;
        return item;
    }

    // TODO: throw exception
    while(1);
}

template<typename T>
bool Stack<T>::any()
{
    return this->size > 0;
}

template<typename T>
void Stack<T>::resize(uint32_t new_capacity)
{
    T* new_data = new T[new_capacity];
    for (uint32_t i = 0; i < this->size; i++)
    {
        new_data[i] = this->data[i];
    }
    delete[] this->data;
    this->data = new_data;
    this->capacity = new_capacity;
}

#pragma endregion