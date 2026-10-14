#pragma once

#include <stdint.h>

#pragma region Queue

template<typename T>
class Queue{
    public:
        Queue(uint32_t capacity);
        ~Queue();
        void enque(T item);
        T deque();
        bool any();
        bool remove(T item);
    private:
        uint32_t capacity;
        T* data;
        uint32_t size;
        uint32_t head;
        uint32_t tail;
        void resize(uint32_t new_capacity);
};

#pragma endregion

#pragma region Linked List

template<typename T>
class ListNode{
    public:
        ListNode(T data);
        ~ListNode();
        T data;
        ListNode* next_node;
};

template<typename T>
class List{
    public:
        List();
        ~List();
        void append(T item);
        T remove_first();
        bool any();
    private:
        ListNode<T>* head;
        ListNode<T>* tail;
};

#pragma endregion

#pragma region Array

template<typename T>
class Array{
    public:
        Array(uint32_t capacity);
        ~Array();
        bool contains(T item);
        void add(T item);
        void remove(T item);
        T* begin();
        T* end();
    private:
        uint32_t capacity;
        uint32_t size;
        T* data;
        void resize(uint32_t new_capacity);
};

#pragma endregion

#pragma region Stack

template<typename T>
class Stack{
    public:
        Stack(uint32_t capacity);
        ~Stack();
        void push(T item);
        T pop();
        bool any();
    private:
        uint32_t capacity;
        uint32_t size;
        T* data;
        void resize(uint32_t new_capacity);
};

#pragma endregion
