//StringLink.cpp
//

#include "StringLink.h"
#include <iostream>
using namespace std;

StringLink::StringLink():num(0),str(NULL),last(NULL),errorAdd("")
{
}

StringLink::StringLink(const StringLink& link):num(0),str(NULL),last(NULL),errorAdd("")
{
	if (&link==this)
		return;
	for (int i=0;i<link.length();i++)
	{
		PString t=link.str;
		for (int j=i;t&&j;j--,t=t->next);
		if (t)
		{
			PString newstring=new String();
			if (!newstring)
				break;
			newstring->elm=t->elm;
			if (!str)
				str=newstring;
			if (last)
				last->next=newstring;
			last=newstring;
			num++;
		}
	}
}

StringLink::~StringLink()
{
	freelink();
}

void StringLink::freelink()
{
	PString p=str;
	PString q;
	while(NULL!=p)
	{
		q=p->next;
		delete p;
		p=q;
	}
	str=NULL;
	last=NULL;
	errorAdd="";
	num=0;
	return;
}

void StringLink::clear()
{
	freelink();
}

void StringLink::reload(const StringLink& link)
{
	if (&link==this)
		return;	
	freelink();
	for (int i=0;i<link.length();i++)
	{
		PString t=link.str;
		for (int j=i;t&&j;j--,t=t->next);
		if (t)
		{
			PString newstring=new String();
			if (!newstring)
				break;
			newstring->elm=t->elm;
			if (!str)
				str=newstring;
			if (last)
				last->next=newstring;
			last=newstring;
			num++;
		}
	}
}

void StringLink::push_back(const string& s)
{
	PString newstring=new String();
	if (!newstring)
		return;
	newstring->elm=s;
	if (last)
		last->next=newstring;
	if (!str)
		str=newstring;
	last=newstring;
	num++;
	return;
}

void StringLink::pop_back()
{
	if (!last)
		return;
	PString t=last;
	if (num==1)
	{
		last=NULL;
		str=NULL;
		delete t;
		num--;
		return;
	}
	else
	{
		for (last=str;last->next!=t;last=last->next);
		delete t;
		last->next=NULL;
		num--;
	}
}

StringLink& StringLink::operator +=(const string& e)
{
	push_back(e);
	return *this;
}

StringLink& StringLink::operator +=(const char& c)
{
	string e("");
	e+=c;
	push_back(e);
	return *this;
}

StringLink& StringLink::operator =(const StringLink& str)
{
	if (&str==this)
		return *this;
	reload(str);
	return *this;
}

StringLink& StringLink::operator =(const string& s)
{
	freelink();
	if (s!="")
		(*this)+=s;
	return *this;
}

string StringLink::at(int i) const
{
	return (*this)[i];
}


string& StringLink::operator [](int i)
{
	if (i<0)
		return errorAdd;
	PString t;
	for (t=str;t&&i;i--,t=t->next);
	if (t)
		return t->elm;
	return errorAdd;
}

string StringLink::operator [](int i) const
{
	if (i<0)
		return "";
	PString t;
	for (t=str;t&&i;i--,t=t->next);
	if (t)
		return t->elm;
	return "";
}

bool StringLink::operator ==(const StringLink& e) const
{
	if (num!=e.num)
		return false;
	int i;
	for (i=0;i<e.num;i++)
	{
		if (e[i]!=(*this)[i])
			break;
	}
	if (i<e.num)
		return false;
	return true;
}

bool StringLink::operator ==(const string& s) const
{
	if (num!=1)
		return false;
	if (s==(*this)[0])
		return true;
	return false;
}

bool StringLink::operator !=(const string& s) const
{
	if ((*this)==(s))
		return false;
	return true;
}

int StringLink::length() const
{
	return num;
}

int StringLink::size() const
{
	return num;
}