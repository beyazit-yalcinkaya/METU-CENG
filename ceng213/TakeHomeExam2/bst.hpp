#ifndef _BIN_SEARCH_TREE_H_
#define _BIN_SEARCH_TREE_H_

#include <functional>
#include <cstddef>
#include <stack>
#include <list>
#include <ostream>
//do not add any other library
//modify parts as directed by assignment text & comments here

template <typename Key, typename Object, 
		  typename Comparator=std::less<Key> >
class BinarySearchTree 
{
	private: //do not change
		struct BinaryNode //node definition: a dependent type
		{
		  	Key key; //keys will be kept constant
		  	Object data; //objects that are referred to by keys may change
		  	BinaryNode * left;
		  	BinaryNode * right;
		  	size_t height; //height info should be updated per insert & delete
		  	BinaryNode(const Key &, const Object &, BinaryNode *, BinaryNode *, size_t =0);
		};

	public: //do not change except for your own private utility functions
		class Iterator //iterator class will encapsulate the location within the BST
		{
		  	public:
				Iterator( ); //dummy constructor for type declaration purposes
				Iterator & operator++( ); //inorder increment
	 
				Object & operator*( );  //update data
				const Object & operator*( ) const; //view data

				bool operator==(const Iterator & rhs) const; //compare iterators
				bool operator!=(const Iterator & rhs) const; //compare iterators
		  
			private:
				BinaryNode * current; //position
				const BinaryNode * root; //for error check not implemented
				std::stack<BinaryNode *> s; //will be used to conduct in order traversal if
				bool useStack; //this variable is set to true, ignored in == and !=
			
			private:
				Iterator(BinaryNode *, const BinarySearchTree &, bool =true);
				//other private utility functions can be declared by you
				BinaryNode* findSucc(BinaryNode*);
				void generateStack(BinaryNode*);

			friend class BinarySearchTree<Key, Object, Comparator>; 
		};

	public: //do not change
		BinarySearchTree( ); //empty tree
		~BinarySearchTree( ); //reclaim all dyn allocated mem

		void insert(const Key &, const Object &); //to insert new key,item
		void remove(const Key &); //remove the node with the key value (and also data) 

  	public: //do not change
		Iterator find(const Key &) const; //single item
		std::list<Iterator> find(const Key &, const Key &) const;//range queries

		Iterator begin( ) const; //inorder begin
		Iterator end( ) const; //dummy NULL iterator

  	public: //do not change
		int height( ) const; //return height of the tree
		size_t size( ) const; //return the number of items in the tree
		bool empty( ) const; //return whether the tree is empty or not
		void print(std::ostream &) const;
  
  	private: //do not change
		BinaryNode * root; //designated root
		size_t nodes; //number of nodes 
		Comparator isLessThan; //function object to compare keys

  	private:
		/* private utility functions that are implemented */
		void makeEmpty(BinaryNode * &);

		BinaryNode * find(const Key &, BinaryNode *) const;
		int height(BinaryNode *) const;
		void print(BinaryNode *, std::ostream &) const;

		template <typename T> //static utility function
		static const T & max(const T &, const T &);

		//balancing functions
		void rotateWithLeftChild(BinaryNode * &);
		void rotateWithRightChild(BinaryNode * &);
		void doubleWithLeftChild(BinaryNode * &);
		void doubleWithRightChild(BinaryNode * &);
		
		//you may add your own private utility functions down here
		BinaryNode* insertHelper(BinaryNode*, const Key&, const Object&);
		int getBalance(BinaryNode*);
		void findHelper(std::list<Iterator>&, BinaryNode*, const Key&, const Key&) const;
		BinaryNode* removeHelper(BinaryNode*, const Key&);
		BinaryNode* findPrevOfMin(BinaryNode*);


  	private: //not copiable, DO NOT IMPLEMENT or change
		BinarySearchTree(const BinarySearchTree &);
		const BinarySearchTree & operator=(const BinarySearchTree &);
};

//node constructor, implemented do not change
template <typename K, typename O, typename C>
BinarySearchTree<K,O,C>::BinaryNode::
BinaryNode(const K & _k, const O & _d,
		   BinaryNode * _l, BinaryNode * _r, size_t _h)
  : key(_k), data(_d), left(_l), right(_r), height(_h)
{
}

//default constructor, implemented do not change
template <typename K, typename O, typename C>
BinarySearchTree<K,O,C>::BinarySearchTree( )
  : root(NULL), nodes(0) 
{
}

//destructor, implemented do not change
template <typename K, typename O, typename C>
BinarySearchTree<K,O,C>::~BinarySearchTree( )
{
  makeEmpty(root);
}

//private utility function for destructor, do not change
template <typename K, typename O, typename C>
void
BinarySearchTree<K,O,C>::makeEmpty(BinaryNode * & t)
{
  if (t != NULL)
  {
	makeEmpty(t->left);
	makeEmpty(t->right);
	delete t;
	--nodes;
  }

  t = NULL;
}

//public function to insert into BST, IMPLEMENT
template <typename K, typename O, typename C>
void BinarySearchTree<K,O,C>::insert(const K & k, const O & x)
{
	root = insertHelper(root, k, x);
}

template <typename K, typename O, typename C>
typename BinarySearchTree<K,O,C>::BinaryNode* BinarySearchTree<K,O,C>::insertHelper(BinaryNode* p, const K & k, const O & x)
{
	if (!p)
	{
		nodes++;
		p = new BinaryNode(k, x, NULL, NULL, 0);
		return p;
	}

	else if (isLessThan(p -> key, k))
	{
		p -> right = insertHelper(p -> right, k, x);		
	}

	else if (isLessThan(k, p -> key))
	{
		p -> left = insertHelper(p -> left, k, x);
	}

	else 
	{
		p -> data = x;
		return p;
	}

	p -> height = 1 + max(height(p -> left), height(p -> right));

	int balance = getBalance(p);

	if (balance > 1 && isLessThan(k, p -> left -> key))
	{
		rotateWithLeftChild(p);
	}
	
	else if (balance < -1 && isLessThan(p -> right -> key, k))
	{
		rotateWithRightChild(p);
	}

	else if (balance > 1 && isLessThan(p -> left -> key, k))
	{
		doubleWithLeftChild(p);
	}
	else if (balance < -1 && isLessThan(k, p -> right -> key))
	{
		doubleWithRightChild(p);
	}
	return p;
}

template <typename K, typename O, typename C>
int BinarySearchTree<K,O,C>::getBalance(BinaryNode* p)
{
	if (!p) return 0;
	return (height(p -> left) + 1) - (height(p -> right) + 1);
}

//public function to remove key, IMPLEMENT
template <typename K, typename O, typename C>
void BinarySearchTree<K,O,C>::remove(const K & k)
{
	root = removeHelper(root, k);
}

template <typename K, typename O, typename C>
typename BinarySearchTree<K,O,C>::BinaryNode* BinarySearchTree<K,O,C>::findPrevOfMin(BinaryNode* p)
{
	while (p && p -> left && p -> left -> left) p = p -> left;
	return p;
}

template <typename K, typename O, typename C>
typename BinarySearchTree<K,O,C>::BinaryNode* BinarySearchTree<K,O,C>::removeHelper(BinaryNode* p, const K & k)
{
	if (!p)
	{
		return p;
	}

	else if (isLessThan(p -> key, k))
	{
		p -> right = removeHelper(p -> right, k);		
	}

	else if (isLessThan(k, p -> key))
	{
		p -> left = removeHelper(p -> left, k);
	}
	else
	{
		if (p -> left && p -> right)
		{
			if (p -> right -> left)
			{
				BinaryNode* prev = findPrevOfMin(p -> right);
				BinaryNode* temp = prev -> left;
				BinaryNode* x = temp -> right;
				prev -> left = p;
				temp -> left = p -> left;
				temp -> right = p -> right;
				p -> right = x;
				p -> left = NULL;
				temp -> right = removeHelper(temp -> right, p -> key);
				p = temp;
			}
			else
			{
				BinaryNode* temp = p -> right;
				temp -> left = p -> left;
				delete p;
				p = temp;
				nodes--;
			}
		}
		else if (p -> left)
		{
			BinaryNode* temp = p -> left;
			delete p;
			p = temp;
			nodes--;
		}
		else if (p -> right)
		{
			BinaryNode* temp = p -> right;
			delete p;
			p = temp;
			nodes--;			
		}
		else
		{
			delete p;
			p = NULL;
			nodes--;
		}
	}
	if (!p) return p;

	p -> height = 1 + max(height(p -> left), height(p -> right));
	int balance = getBalance(p);

	if (balance > 1 && getBalance(p->left) >= 0)
	{
		rotateWithLeftChild(p);
	}
	else if (balance > 1 && getBalance(p->left) < 0)
	{
		doubleWithLeftChild(p); 
	}
	else if (balance < -1 && getBalance(p->right) <= 0)
	{
		rotateWithRightChild(p);		
	}
	else if (balance < -1 && getBalance(p->right) > 0)
	{
		doubleWithRightChild(p);	
	}
	return p;

}

//public function to search elements, do not change
template <typename K, typename O, typename C>
typename BinarySearchTree<K,O,C>::Iterator
BinarySearchTree<K,O,C>::find(const K & key) const
{
  BinaryNode * node = find(key, root);

  if (node == NULL)
  {
	return end();
  }
  else
  {               //not inorder iterator
	return Iterator(node, *this, false);
  }
}

/*
 * private utility function to search elements
 * do not change
 */
template <typename K, typename O, typename C>
typename BinarySearchTree<K,O,C>::BinaryNode *
BinarySearchTree<K,O,C>::find(const K & key, BinaryNode * t) const
{
  if (t == NULL)
  {
	return NULL;
  }
  else if (isLessThan(key, t->key))
  {
	return find(key, t->left);
  }
  else if (isLessThan(t->key, key))
  {
	return find(key, t->right);
  }
  else //found
  {
	return t;
  }
}

//range queries those within range are inserted to the list
//IMPLEMENT
template <typename K, typename O, typename C>
std::list<typename BinarySearchTree<K,O,C>::Iterator> BinarySearchTree<K,O,C>::find(const K & lower, const K & upper) const
{
	std::list<Iterator> l;
	findHelper(l, root, lower, upper);
	return l;
}

template <typename K, typename O, typename C>
void BinarySearchTree<K,O,C>::findHelper(std::list<Iterator>& l, BinaryNode* p, const K& lower, const K& upper) const
{
	if (!p) return;
	if (isLessThan(p -> key, upper) && isLessThan(lower, p -> key))
	{
		findHelper(l, p -> left, lower, upper);
		l.push_back(Iterator(p, *this, false));
		findHelper(l, p -> right, lower, upper);
	}
	else if (isLessThan(p -> key, upper))
	{
		if (!isLessThan(p -> key, lower))
		{
			l.push_back(Iterator(p, *this, false));
		}
		findHelper(l, p -> right, lower, upper);
	}
	else if (isLessThan(lower, p -> key))
	{
		findHelper(l, p -> left, lower, upper);
		if (!isLessThan(upper, p -> key))
		{
			l.push_back(Iterator(p, *this, false));
		}
	}
	else
	{
		if (!isLessThan(upper, p -> key) && !isLessThan(p -> key, lower))
		{
			l.push_back(Iterator(p, *this, false));
		}
	}
}

//INORDER iterator begins at ++root, do not change
template <typename K, typename O, typename C>
typename BinarySearchTree<K,O,C>::Iterator
BinarySearchTree<K,O,C>::begin() const
{
  return Iterator(root, *this);
}

//no more increment after end() also
//returned in case of unsuccessful search
//or when no more applicance of ++ is possible
//do not change
template <typename K, typename O, typename C>
typename BinarySearchTree<K,O,C>::Iterator
BinarySearchTree<K,O,C>::end() const
{
  return Iterator(NULL, *this);
}

//public function to return height, do not change
template <typename K, typename O, typename C>
int
BinarySearchTree<K,O,C>::height( ) const
{
  return height(root);
}

/* private utility function for computing height */
//do not change
template <typename K, typename O, typename C>
int
BinarySearchTree<K,O,C>::height(BinaryNode * t) const
{
  return (t == NULL) ? -1 : t->height;
}

//public function to return number of nodes in the tree
//do not change
template <typename K, typename O, typename C>
size_t
BinarySearchTree<K,O,C>::size( ) const
{
  return nodes;
}

//public true if empty false o.w.
//do not change
template <typename K, typename O, typename C>
bool
BinarySearchTree<K,O,C>::empty( ) const
{
  return nodes == 0;
}

//public function to print keys inorder to some ostream
//do not change
template <typename K, typename O, typename C>
void
BinarySearchTree<K,O,C>::print(std::ostream & out) const
{
  print(root, out);
  out << '\n';
}

/* private utility function to print, do not change */
template <typename K, typename O, typename C>
void
BinarySearchTree<K,O,C>::print(BinaryNode * t, std::ostream & out) const
{
  if (t != NULL && t->left != NULL) 
  {
	out << '[';
	print( t->left, out );
  }
  else if (t != NULL && t->left == NULL && t->right != NULL)
  {
	out << "[";
  }

  if (t != NULL)
  {
	if (t->left == NULL && t->right == NULL)
	{
	  out << '(' << (t->key) << ')';
	}
	else if (t->left != NULL || t->right != NULL)
	{
	  out << '{' << (t->key) << ",H" << t->height << '}';
	}
  }
  
  if (t != NULL && t->right != NULL)
  {
	print( t->right, out );
	out << ']';
  }
  else if (t != NULL && t->left != NULL && t->right == NULL)
  {
	out << "]";
  }
}

/* static function to compute maximum of two elements */
//do not change
template <typename K, typename O, typename C>
template <typename T>
const T &
BinarySearchTree<K,O,C>::max(const T & el1, const T & el2)
{
  return el1 > el2 ? el1 : el2;
}

/* ROTATIONS, do not change */
template <typename K, typename O, typename C>
void 
BinarySearchTree<K,O,C>::rotateWithLeftChild(BinaryNode * & k2)
{
  BinaryNode *k1 = k2->left;
  k2->left = k1->right;
  k1->right = k2;
  k2->height = BinarySearchTree<K,O,C>::max(height(k2->left), 
											height(k2->right)) + 1;
  k1->height = BinarySearchTree<K,O,C>::max(height(k1->left), 
											height(k2)) + 1;
  k2 = k1;
}

template <typename K, typename O, typename C>
void 
BinarySearchTree<K,O,C>::rotateWithRightChild(BinaryNode * & k1)
{
  BinaryNode *k2 = k1->right;
  k1->right = k2->left;
  k2->left = k1;
  k1->height = BinarySearchTree<K,O,C>::max(height(k1->left), 
											height(k1->right)) + 1;
  k2->height = BinarySearchTree<K,O,C>::max(height(k2->right), 
											height(k1)) + 1;
  k1 = k2;
}

template <typename K, typename O, typename C>
void 
BinarySearchTree<K,O,C>::doubleWithLeftChild(BinaryNode * & k3)
{
  rotateWithRightChild(k3->left);
  rotateWithLeftChild(k3);
}

template <typename K, typename O, typename C>
void 
BinarySearchTree<K,O,C>::doubleWithRightChild(BinaryNode * & k1)
{
  rotateWithLeftChild(k1->right);
  rotateWithRightChild(k1);
}

/* ITERATOR functions */

// dummy constructor, do not change
template <typename K, typename O, typename C>
BinarySearchTree<K,O,C>::Iterator::Iterator( ) 
  : current(NULL), root(NULL), useStack(false)
{
}

// dereferencing operator non-const version, IMPLEMENT
template <typename K, typename O, typename C>
O &
BinarySearchTree<K,O,C>::Iterator::operator*( )
{
	return current -> data;
}

// dereferencing operator const version, IMPLEMENT
template <typename K, typename O, typename C>
const O & 
BinarySearchTree<K,O,C>::Iterator::operator*( ) const
{
	return current -> data;
}

// compare Iterators ignoring useStack var, do not change
template <typename K, typename O, typename C>
bool 
BinarySearchTree<K,O,C>::Iterator::
operator==(const Iterator & rhs) const
{ 
  return current == rhs.current && 
		 root == rhs.root; 
}

// compare Iterators ignoring useStack var, do not change
template <typename K, typename O, typename C>
bool 
BinarySearchTree<K,O,C>::Iterator::
operator!=(const Iterator & rhs) const
{ 
  return !(*this == rhs); 
}

// increment Iterator to point to the inorder next
// node of then-current node, in case that no further
// advances are possible return an Iterator that is
// equal to end( ) , IMPLEMENT
template <typename K, typename O, typename C>
typename BinarySearchTree<K,O,C>::Iterator &
BinarySearchTree<K,O,C>::Iterator::operator++( )
{
	if (useStack) current = findSucc(current);
	else current = NULL;
	return *this;
}

/* real Iterator constructor will be invoked by 
 * BST member function only. if no inorder iterator
 * is required by the computation designer should 
 * explicitly set useStack variable to false, o.w.
 * it will be assumed to be true. IMPLEMENT
 */
template <typename K, typename O, typename C>
BinarySearchTree<K,O,C>::Iterator::
Iterator(BinaryNode * p, const BinarySearchTree & rhs, bool stk)
{
	useStack = stk;
	root = rhs.root;
	current = p;
	if (useStack)
	{
		if (p)
		{
			generateStack(p);
			if (!s.empty()) current = s.top();
		}
	}
}

template <typename K, typename O, typename C>
typename BinarySearchTree<K,O,C>::BinaryNode* BinarySearchTree<K,O,C>::Iterator::findSucc(BinaryNode* p)
{
	if (!s.empty())
	{
		BinaryNode* temp = NULL;
		BinaryNode* succ = NULL;
		temp = s.top();
		s.pop();
		if (temp == p)
		{
			if (!s.empty()) succ = s.top();
			else succ = NULL;
			s.push(temp);
		}
		else
		{
			succ = findSucc(p);
			s.push(temp);
		}
		return succ;
	}
	else
	{
		return NULL;
	}
}

template <typename K, typename O, typename C>
void BinarySearchTree<K,O,C>::Iterator::generateStack(BinaryNode* p)
{
	if (!p) return;
	generateStack(p -> right);
	s.push(p);
	generateStack(p -> left);
}

#endif
