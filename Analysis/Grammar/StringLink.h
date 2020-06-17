//StringLink.h
//
#pragma once

#include <string>
using namespace std;

typedef class String
{
public:
	string elm;
	String * next;

	String():elm(""),next(NULL){}
}*PString;

class StringLink
{
public:
	StringLink();
	StringLink(const StringLink& link);
	~StringLink();
	
	PString str;
	PString last;
	string errorAdd;
	int num;
public:
	void push_back(const string& s);
	void pop_back();
	StringLink& operator +=(const string& e);
	StringLink& operator +=(const char& c);
	StringLink& operator =(const StringLink& str);
	StringLink& operator =(const string& s);
	string at(int i) const;
	string& operator [](int i);
	string operator [](int i) const;
	bool operator ==(const StringLink& e) const;
	bool operator ==(const string& s) const;
	bool operator !=(const string& s) const;
	int length() const;
	int size() const;
	void clear();
private:
	void freelink();
	void reload(const StringLink& link);
};