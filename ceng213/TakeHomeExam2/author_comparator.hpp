#ifndef _author_h__
#define _author_h__

#include "book.hpp"
#include <cstring>

class AuthorComparator
{
  public:
    bool operator( ) (const Book::SecondaryKey & key1, 
                      const Book::SecondaryKey & key2) const
    {
    	std::string a = key1.getAuthor();
    	std::string b = key2.getAuthor();
    	std::string c = key1.getTitle();
    	std::string d = key2.getTitle();
    	for (std::string::size_type i = 0; i < a.size(); i++) a[i] = tolower(a[i]);
    	for (std::string::size_type i = 0; i < b.size(); i++) b[i] = tolower(b[i]);
    	for (std::string::size_type i = 0; i < c.size(); i++) c[i] = tolower(c[i]);
    	for (std::string::size_type i = 0; i < d.size(); i++) d[i] = tolower(d[i]);
    	return (a < b || (a == b && c < d));
    }

};

#endif
