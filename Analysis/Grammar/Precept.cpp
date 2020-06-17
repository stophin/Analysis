#include "Precept.h"

Precept::Precept(void)
{
	strLeft = "";
	strRight = "";
}

Precept::~Precept(void)
{
}

Precept::Precept(StringLink left, StringLink right)
{
	strLeft = left;
	strRight = right;
}

StringLink Precept::GetLeft()
{
	return strLeft;
}

StringLink Precept::GetRight()
{
	return strRight;
}

void Precept::Clear(void)
{
	strLeft = "";
	strRight = "";
}
