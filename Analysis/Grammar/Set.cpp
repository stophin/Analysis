
#include "set.h"

Set::Set(void)
{
	SetContent.clear();
}

Set::~Set(void)
{
}

Set::Set(const Set & set)
{
	SetContent = set.SetContent;
}

Set::Set(string cChar)
{
	SetContent.push_back(cChar);
}

bool Set::Insert(string cInsert)
{
	bool bFind = Find(cInsert);
	if (!bFind)
		SetContent.push_back(cInsert);
	return (!bFind);
}

bool Set::Delete(string cDelete)	
{
	int iPos = FindPos(cDelete);
	if (iPos != -1)
	{
		for (unsigned int i = iPos; i < SetContent.size() - 1; i ++)
		{
			SetContent[i] = SetContent[i+1];
		}
		SetContent.pop_back();
	}
	return (iPos != -1);
}

int Set::FindPos(string cFind)
{
	bool bFind = false;
	unsigned int iPos;
	for (iPos = 0; iPos < SetContent.size(); iPos ++)
	{
		if (SetContent[iPos] == cFind)
		{
			bFind = true;
			break;
		}
	}
	if (bFind)
		return iPos;
	else
		return -1;
}

bool Set::Find(string cFind)
{
	bool bFind = false;
	for (unsigned int iPos = 0; iPos < SetContent.size(); iPos ++)
	{
		if (SetContent[iPos] == cFind)
		{
			bFind = true;
			break;
		}
	}
	return bFind;
}

int Set::Add(const Set & set)
{
	int iCount = 0;
	for(int iPos = 0; iPos < set.Size(); iPos ++)
	{
		if(Insert(set.SetContent[iPos]))
			iCount ++;
	}
	return iCount;
}

int Set::Sub(const Set & set)
{
	int iCount = 0;
	for(int iPos = 0; iPos < set.Size(); iPos ++)
	{
		if(Delete(set.SetContent[iPos]))
			iCount ++;
	}
	return iCount;

}

int Set::Size() const
{
	return (int) SetContent.length();
}

Set Set::operator + (const Set & set)
{
	Set newSet = * this;
	newSet.Add(set);
	return newSet;
}

Set Set::operator - (const Set & set)
{
	Set newSet = * this;
	newSet.Sub(set);
	return newSet;
}

const Set Set::operator = (const Set & set)
{
	SetContent = set.SetContent;
	return * this;
}

string Set::GetAt(int iPos)
{
	if (iPos < (int) SetContent.size())
		return SetContent[iPos];
	else
		return "";
}

bool Set::IsEmpty()
{
	return (Size() == 0);
}
