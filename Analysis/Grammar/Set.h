#pragma once
#include "StringLink.h"
#include <vector>
using namespace std;

class Set
{
public:
	Set(void);
	~Set(void);
	Set(const Set & set);
	Set(string cChar);
	bool Insert(string cInsert);
	bool Delete(string cDelete);
	bool Find(string cFind);
	int FindPos(string cFind);
	int Add(const Set & set);
	int Sub(const Set & set);
	int Size() const;
	Set operator + (const Set & set);
	Set operator - (const Set & set);
	const Set operator = (const Set & set);
	string GetAt(int iPos);
	bool IsEmpty();
private:
	StringLink SetContent;
};
