#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#include <iostream>
#include "Node.hpp"

using namespace std;

/*....DO NOT EDIT BELOW....*/
template <class T> 
class LinkedList {
    private:
		/*First node of the linked-list*/
        Node<T>* head;
        /*Last node of the linked-list*/
		Node<T>* tail;
		/*size of the linked-list*/
		size_t  size; 
    public:

        LinkedList();
        LinkedList(const LinkedList<T>& ll);
        LinkedList<T>& operator=(const LinkedList<T>& ll);
        ~LinkedList();

        /* Return head of the linked-list*/
        Node<T>* getHead() const;
        /* Set head of the linked-list*/
        void setHead(Node<T>* n);
        /* Return tail of the linked-list*/
        Node<T>* getTail() const;
        /* Set tail of the linked-list*/
        void setTail(Node<T>* n);
        /* Get the previous node of the node that contains the data item. 
         * If the head node contains the data item, this method returns NULL.*/
        Node<T>* findPrev(const T& data) const;
        /* Get the node that stores the data item. 
         * If data is not found in the list, this function returns NULL.*/
        Node<T>* findNode(const T& data) const;
        /* Insert a new node to store the data item. 
         * The new node should be placed after the “prev” node. 
         * If prev is NULL then insert new node to the head.*/
        void insertNode(Node<T>* prev, const T& data); 
        /* This method is used to delete the node that is next to “prevNode”. 
         * PS:prevNode is not the node to be deleted. */
        void deleteNode(Node<T>* prevNode);  
        /* This method is used to clear the contents of the list.*/
        void clear();
        /* This method returns true if the list empty, otherwise returns false.*/
        bool isEmpty() const;
        /* This method returns the current size of the list. */
        size_t getSize() const;
        /*Prints the list. This method was already implemented. Do not modify.*/
        void print() const;
};

template <class T>
void LinkedList<T>::print() const{
    const Node<T>* node = head;
    while (node) {
        std::cout << node->getData();
        node = node->getNext();
    }
    cout<<std::endl;
}

/*....DO NOT EDIT ABOVE....*/

/* TO-DO: method implementations below */

template <class T>
LinkedList<T>::LinkedList()
{
    head = NULL;
    tail = NULL;
    size = 0;
}

template <class T>
LinkedList<T>::LinkedList(const LinkedList<T>& ll)
{
    Node<T>* q = ll.getHead();
    if (q)
    {
        Node<T>* temp = new Node<T>(q -> getData());;
        Node<T>* p;
        q = q -> getNext();
        head = temp;
        size = 1;
        while (q)
        {
            p = new Node<T>(q -> getData());
            temp -> setNext(p);
            temp = p;
            q = q -> getNext();
            size++;
        }
        tail = p;
    }
    else
    {
        tail = NULL;
        head = NULL;
        size = 0;
    }
}

template <class T>
LinkedList<T>& LinkedList<T>::operator=(const LinkedList<T>& ll)
{
    if (this == &ll) return *this;
    Node<T>* q = ll.getHead();
    Node<T>* temp;
    while (head)
    {
        temp = head -> getNext();
        delete head;
        head = temp;
    }
    if (q)
    {
        temp = new Node<T>(q -> getData());;
        Node<T>* p;
        q = q -> getNext();
        head = temp;
        size = 1;
        while (q)
        {
            p = new Node<T>(q -> getData());
            temp -> setNext(p);
            temp = p;
            q = q -> getNext();
            size++;
        }
        tail = p;
    }
    else
    {
        tail = NULL;
        head = NULL;
        size = 0;
    }
    return *this;
}
template <class T>
LinkedList<T>::~LinkedList()
{
    Node<T>* temp;
    while (head)
    {
        temp = head -> getNext();
        delete head;
        head = temp;
    }
    tail = NULL;
    size = 0;
}

template <class T>
Node<T>* LinkedList<T>::getHead() const {return head;}

template <class T>
void LinkedList<T>::setHead(Node<T>* n) {head = n;}

template <class T>
Node<T>* LinkedList<T>::getTail() const {return tail;}

template <class T>
void LinkedList<T>::setTail(Node<T>* n) {tail = n;}

template <class T>
Node<T>* LinkedList<T>::findPrev(const T& data) const
{
    Node<T>* out = NULL;
    Node<T>* p = head;
    while (p and not (p -> getData() == data)) {out = p; p = p -> getNext();}
    if (p) return out;
    else return NULL;
}

template <class T>
Node<T>* LinkedList<T>::findNode(const T& data) const
{
    Node<T>* p = head;
    while (p and not (p -> getData() == data)) {p = p -> getNext();}
    return p;
}

template <class T>
void LinkedList<T>::insertNode(Node<T>* prev, const T& data)
{
    Node<T>* p = new Node<T>(data);
    if (prev)
    {
        p -> setNext(prev -> getNext());
        prev -> setNext(p);
        if (tail == prev) tail = p;
    }
    else
    {
        p -> setNext(head);
        head = p;
        if (not tail) tail = p;
    }
    size++;
}

template <class T>
void LinkedList<T>::deleteNode(Node<T>* prevNode)
{
    if (not prevNode)
    {
        Node<T> *p = head -> getNext();
        if (head == tail) tail = p;
        delete head;
        head = p;
    }
    else
    {
        Node<T>* p = prevNode -> getNext();
        prevNode -> setNext(p -> getNext());
        if (p == tail) tail = prevNode;
        delete p;
    }
    size--;
}

template <class T>
void LinkedList<T>::clear()
{
    Node<T>* temp;
    while (head)
    {
        temp = head -> getNext();
        delete head;
        head = temp;
    }
    tail = NULL;
    size = 0;
}

template <class T>
bool LinkedList<T>::isEmpty() const {return not size;}

template <class T>
size_t LinkedList<T>::getSize() const {return size;}

/* end of your implementations*/

#endif
