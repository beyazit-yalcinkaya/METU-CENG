#include "bookstore.hpp"

BookStore::BookStore( ) //implemented, do not change
{
}

void
BookStore::insert(const Book & book)
{
	std::string isbn = book.getISBN();
	primaryIndex.insert(isbn, book);
	Book* book_ptr = &(*(primaryIndex.find(isbn)));
	secondaryIndex.insert(SKey(book),book_ptr);
	ternaryIndex.insert(SKey(book), book_ptr);
}

void
BookStore::remove(const std::string & isbn)
{
	BSTP::Iterator it = primaryIndex.find(isbn);
	if (it == primaryIndex.end()) return;
	Book& book_ref = *it;
	primaryIndex.remove(isbn);
	secondaryIndex.remove(SKey(book_ref));
	ternaryIndex.remove(SKey(book_ref));
}

void
BookStore::remove(const std::string & title, const std::string & author)
{
	BSTS::Iterator it = secondaryIndex.find(SKey(title, author));
	if (it == secondaryIndex.end()) return;
	std::string isbn = (*(*(secondaryIndex.find(SKey(title, author))))).getISBN();
	primaryIndex.remove(isbn);
	secondaryIndex.remove(SKey(title, author));
	ternaryIndex.remove(SKey(title, author));
}
void
BookStore::removeAllBooksWithTitle(const std::string & title)
{
	std::list<SKey> l1;
	std::list<std::string> l2;
	BSTT::Iterator it;
	std::string b = title;
	for (int i = 0; i < b.size(); i++) b[i] = tolower(b[i]);
	for (it = ternaryIndex.begin(); it != ternaryIndex.end(); ++it)
	{
		std::string a = (*(*it)).getTitle();
		for (int i = 0; i < a.size(); i++) a[i] = tolower(a[i]);
		if (a == b)
		{
			l1.push_back(SKey(*(*it)));
			l2.push_back((*(*it)).getISBN());
		}
	}
	for (std::list<SKey>::iterator i = l1.begin(); i != l1.end(); ++i)
	{
		secondaryIndex.remove(*i);
		ternaryIndex.remove(*i);
	}
	for (std::list<std::string>::iterator i = l2.begin(); i != l2.end(); ++i)
	{
		primaryIndex.remove(*i);		
	}
	l1.clear();
	l2.clear();
}

void
BookStore::makeAvailable(const std::string & isbn)
{
	BSTP::Iterator it = primaryIndex.find(isbn);
	if (it == primaryIndex.end()) return;
	Book& book_ref = *it;
	book_ref.setAvailable();
}

void
BookStore::makeUnavailable(const std::string & title, const std::string & author)
{
	BSTT::Iterator it = ternaryIndex.find(SKey(title, author));
	if (it == ternaryIndex.end()) return;
	Book& book_ref = *(*it);
	book_ref.setUnavailable();
}

void
BookStore::updatePublisher(const std::string & author, 
						   const std::string & publisher)
{
	BSTP::Iterator it;
	std::string b = author;
	for (int i = 0; i < b.size(); i++) b[i] = tolower(b[i]);
	for (it = primaryIndex.begin(); it != primaryIndex.end(); ++it)
	{
		std::string a = (*it).getAuthor();
		for (int i = 0; i < a.size(); i++) a[i] = tolower(a[i]);
		if (a == b)
		{
			(*it).setPublisher(publisher);
		}
	}
}

void
BookStore::printBooksWithISBN(const std::string & isbn1,
							  const std::string & isbn2,
							  unsigned short since) const
{
	BSTP::Iterator it;
	for (it = primaryIndex.begin(); it != primaryIndex.end(); ++it)
	{
		std::string isbn = (*it).getISBN();
		if (isbn >= isbn1 && isbn <= isbn2 && (*it).getYear() >= since)
		{
			std::cout << *it << std::endl;
		}
	}
}

void
BookStore::printBooksOfAuthor(const std::string & author,
							  const std::string & first,
							  const std::string & last) const
{
	std::string b = author;
	std::string d = first;
	std::string e = last;
	BSTS::Iterator it;
	for (int i = 0; i < b.size(); i++) b[i] = tolower(b[i]);
	for (int i = 0; i < d.size(); i++) d[i] = tolower(d[i]);
	for (int i = 0; i < e.size(); i++) e[i] = tolower(e[i]);
	for (it = secondaryIndex.begin(); it != secondaryIndex.end(); ++it)
	{
		std::string a = (*(*it)).getAuthor();
		for (int i = 0; i < a.size(); i++) a[i] = tolower(a[i]);
		if (a == b)
		{
			std::string c = (*(*it)).getTitle();
			for (int i = 0; i < c.size(); i++) c[i] = tolower(c[i]);
			if (c >= d && c <= e)
			{
				std::cout << *(*it) << std::endl;
			}
		}
	}

}

void //implemented, do not change
BookStore::printPrimarySorted( ) const
{
  BSTP::Iterator it;

  for (it=primaryIndex.begin(); it != primaryIndex.end(); ++it)
  {
	std::cout << *it << std::endl;
  }
}

void //implemented, do not change
BookStore::printSecondarySorted( ) const
{
  BSTS::Iterator it;

  for (it = secondaryIndex.begin(); it != secondaryIndex.end(); ++it)
  {
	std::cout << *(*it) << std::endl;
  }
}

void //implemented, do not change
BookStore::printTernarySorted( ) const
{
  BSTT::Iterator it;

  for (it = ternaryIndex.begin(); it != ternaryIndex.end(); ++it)
  {
	std::cout << *(*it) << std::endl;
  }
}
