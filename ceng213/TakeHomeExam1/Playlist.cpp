#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Playlist.hpp"
#include "Entry.hpp"

using namespace std;


Playlist::Playlist()
{
    srand(15);
}

int Playlist::getRandomNumber(int i, int n) const
{
    int range=n-i;
    int random = rand() % range + i;
    return random;
}

void Playlist::print()
{
    cout << "[PLAYLIST SIZE=" << entries.getSize() <<"]";
    entries.print();
}
void Playlist::printHistory()
{
    cout<<"[HISTORY]";
    history.print();
}

/* TO-DO: method implementations below */

const LinkedList<Entry>& Playlist::getEntries() const {return entries;}

void Playlist::load(string fileName)
{
	ifstream f(fileName.c_str());
	string line;
	Operation x = INSERT;
	while (getline(f, line))
	{
		size_t n1, n2;
		n1 = line.find(";");
		n2 = line.rfind(";");
		Entry temp_entry(line.substr(0, n1), line.substr(n1 + 1, n2 - n1 - 1), line.substr(n2 + 1, line.size() - n2 - 1));
		HistoryRecord temp_hist(x, temp_entry);
		entries.insertNode(entries.getTail(), temp_entry);
		history.push(temp_hist);
	}
	f.close();
}

void Playlist::insertEntry(const Entry &e)
{
	Operation x = INSERT;
	HistoryRecord temp(x, e);
	entries.insertNode(entries.getTail(), e);
	history.push(temp);
}

void Playlist::deleteEntry(const string &_title)
{
	Node<Entry>* q = entries.findNode(_title);
	if (q)
	{
		Node<Entry> *p = entries.findPrev(_title);
		Operation x = DELETE;
		HistoryRecord temp(x, q -> getData());
		history.push(temp);
		entries.deleteNode(p);
	}
}

void Playlist::moveLeft(const std::string &title)
{
	Node<Entry>* h = entries.getHead();
	Node<Entry>* p = entries.getHead();
	Node<Entry>* q = NULL;
	while (p and ((p -> getData()).getTitle() != title)) {p = p -> getNext();}
	while (h and ((h -> getData()).getTitle() != title)) {q = h; h = h -> getNext();}
	if (p and q)
	{
		swapNodes(p, q);
	}
}

void Playlist::moveRight(const std::string &title)
{
	Node<Entry>* p = entries.getHead();
	while (p and ((p -> getData()).getTitle() != title)) {p = p -> getNext();}
	Node<Entry>* q = p -> getNext();
	if (p and q)
	{
		swapNodes(p, q);
	}
}

void Playlist::reverse()
{
	if (entries.getSize() > 1)
	{
		entries.setTail(reverse_helper(entries.getHead()));
		(entries.getTail()) -> setNext(NULL);
	}
	Operation x = REVERSE;
	HistoryRecord temp(x);
	history.push(temp);
}

Node<Entry>* Playlist::reverse_helper(Node<Entry>* p)
{
	if (not (p -> getNext())) {entries.setHead(p); return p;}
	reverse_helper(p -> getNext()) -> setNext(p);
	return p;
}

void Playlist::swapNodes(Node<Entry>* p, Node<Entry>* q)
{
	if (p == q) return;
	else if (p -> getNext() == q)
	{
		Node<Entry>* prep = entries.findPrev(p -> getData());
		Node<Entry>* nextq = q -> getNext();
		if (prep) prep -> setNext(q);
		p -> setNext(nextq);
		q -> setNext(p);
		if (p == entries.getHead()) entries.setHead(q);
		if (q == entries.getTail()) entries.setTail(p);		
	}
	else if (q -> getNext() == p)
	{
		Node<Entry>* preq = entries.findPrev(q -> getData());
		Node<Entry>* nextp = p -> getNext();
		if (preq) preq -> setNext(p);
		q -> setNext(nextp);
		p -> setNext(q);
		if (q == entries.getHead()) entries.setHead(p);
		if (p == entries.getTail()) entries.setTail(q);
	}
	else
	{
		Node<Entry>* prep = entries.findPrev(p -> getData());
		Node<Entry>* preq = entries.findPrev(q -> getData());
		Node<Entry>* nextp = p -> getNext();
		Node<Entry>* nextq = q -> getNext();
		if (prep) prep -> setNext(q);
		if (preq) preq -> setNext(p);
		p -> setNext(nextq);
		q -> setNext(nextp);
		if (p == entries.getHead()) entries.setHead(q);
		else if (q == entries.getHead()) entries.setHead(p);
		if (q == entries.getTail()) entries.setTail(p);
		else if (p == entries.getTail()) entries.setTail(q);
	}
}

void Playlist::sort()
{
	Node<Entry>* min;
	Node<Entry>* p;
	Node<Entry>* q;
	for (p = entries.getHead(); p; p = p -> getNext())
	{
		min = p;	
		for (q = p -> getNext(); q; q = q -> getNext())
		{
			if (((q -> getData()).getTitle()) < ((min -> getData()).getTitle()))
			{
				min = q;
			}			
		}
		swapNodes(p, min);
		p = min;
	}	
} 

void Playlist::merge(const Playlist & pl)
{
	Node<Entry>* p = entries.getHead();
	Node<Entry>* q = (pl.getEntries()).getHead();
	Node<Entry>* prep = NULL;
	while (p and q)
	{
		if ((p -> getData()).getTitle() > (q -> getData()).getTitle())
		{
			entries.insertNode(prep, (q -> getData()));
			q = q -> getNext();
			if (prep) p = prep -> getNext();
			else p = entries.getHead();
		}
		else
		{
			prep = p;
			p = p -> getNext();
		}
	}
	while (q)
	{
		entries.insertNode(prep, (q -> getData()));
		prep = prep -> getNext();
		q = q -> getNext();
	}
}

Node<Entry>* Playlist::findByIndex(int i) const
{
	Node<Entry>* p = entries.getHead();
	for (int j = 0; j < i; j++)
		p = p -> getNext();
	return p;
}

void Playlist::shuffle()
{
	Node<Entry>* p;
	Node<Entry>* q;
	int n = entries.getSize(), i, j;
	for (i = 0; i < (n - 1); i++)
	{
		p = findByIndex(i);
		j = getRandomNumber(i, n);
		q = findByIndex(j);
		swapNodes(p ,q);
	}
}

void Playlist::undo()
{
	HistoryRecord action = (history.Top()) -> getData();
	switch (action.getOperation())
	{
		case INSERT:
		{
			deleteEntry((action.getEntry()).getTitle());
			history.pop();
			break;
		}
		case DELETE:
		{
			insertEntry(action.getEntry());
			history.pop();
			break;
		}
		case REVERSE:
		{
			reverse();
			history.pop();
			break;
		}
	}
	history.pop();
}
