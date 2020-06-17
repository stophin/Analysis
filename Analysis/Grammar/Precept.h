#pragma once
#include "StringLink.h"
#include <string>
using namespace std;

class Precept
{
public:
	Precept(void);
	Precept(StringLink left, StringLink right);
	~Precept(void);
	StringLink GetLeft();
	StringLink GetRight();
private:
	StringLink strLeft;
	StringLink strRight;
public:
	void Clear(void);
};

