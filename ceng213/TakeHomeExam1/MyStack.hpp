#ifndef MYSTACK_HPP
#define MYSTACK_HPP
#include "Node.hpp"

/*You are free to add private/public members inside this template..*/
template <class T>
class MyStack{
    private:
    Node<T> *top;                
    public:
    /*Default constructor*/
    MyStack();                     					
    /*copy constructor*/
    MyStack(const MyStack<T>& rhs);				
    /*destructor*/
    ~MyStack(); 
    /*overloaded = operator*/
    MyStack<T>& operator=(const MyStack<T>& rhs);  	
    /*returns true if stack is empty*/
    bool isEmpty() const;
    /*push newItem to stack*/
    void push(const T& newItem);
    /*pop item from stack*/
    void pop();
    /*return top item of the stack*/
    Node<T>* Top() const;
	/*Prints the stack entries. This method was already implemented. Do not modify.*/
    void print() const;
};

template <class T>
void MyStack<T>::print() const{
    const Node<T>* node = top;
    while (node) {
        std::cout << node->getData();
        node = node->getNext();
    }
    cout<<std::endl;
}

/* TO-DO: method implementations below */

template <class T>
MyStack<T>::MyStack() {top = NULL;}

template <class T>
MyStack<T>::MyStack(const MyStack<T>& rhs)
{
    Node<T>* q = rhs.Top();
    if (q)
    {
        Node<T>* temp = new Node<T>(q -> getData());;
        Node<T>* p;
        q = q -> getNext();
        top = temp;
        while (q)
        {
            p = new Node<T>(q -> getData());
            temp -> setNext(p);
            temp = p;
            q = q -> getNext();
        }
    }
    else
    {
        top = NULL;
    }
}

template <class T>
MyStack<T>::~MyStack()
{
    Node<T>* temp;
    while (top)
    {
        temp = top -> getNext();
        delete top;
        top = temp;
    }
}

template <class T>
MyStack<T>& MyStack<T>::operator=(const MyStack<T>& rhs)
{
    if (this == &rhs) return *this;
    Node<T>* q = rhs.Top();
    Node<T>* temp;
    while (top)
    {
        temp = top -> getNext();
        delete top;
        top = temp;
    }
    if (q)
    {
        temp = new Node<T>(q -> getData());;
        Node<T>* p;
        q = q -> getNext();
        top = temp;
        while (q)
        {
            p = new Node<T>(q -> getData());
            temp -> setNext(p);
            temp = p;
            q = q -> getNext();
        }
    }
    else
    {
        top = NULL;
    }
    return *this;
}

template <class T>
bool MyStack<T>::isEmpty() const {return not top;}

template <class T>
void MyStack<T>::push(const T& newItem)
{
    Node<T>* p = new Node<T>(newItem);
    p -> setNext(top);
    top = p;
}

template <class T>
void MyStack<T>::pop()
{
    if (not top) return;
    Node<T>* temp = top -> getNext();
    delete top;
    top = temp;
}

template <class T>
Node<T>* MyStack<T>::Top() const {return top;}

#endif /* MYSTACK_HPP */
