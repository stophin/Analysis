#include "Grammar.h"
#include "Set.h"
#include "iostream"
using namespace std;

#define MAX_PATH 1000

Grammar::Grammar(void)
{
	cStart = '\0';
	P.clear();
	nVn = 0;
	nVt = 0;
	nP = 0;
	pCanEmptyTable = 0;
	pPredictTable = 0;
	iIsLL1 = 0;
}

Grammar::~Grammar(void)
{
	if (pCanEmptyTable != 0)
		delete [] pCanEmptyTable;
	if (pPredictTable != 0)
		delete [] pPredictTable;
}

void Grammar::SetVt(StringLink vt)
{
	for(unsigned int i = 0; i < vt.length(); i ++)
		Vt.Insert(vt[i]);
	nVt = Vt.Size();
}

void Grammar::SetVn(StringLink vn)
{
	for(unsigned int i = 0; i < vn.length(); i ++)
		Vn.Insert(vn[i]);
	nVn = Vn.Size();
}

void Grammar::SetStart(string start)
{
	if (Vn.Find(start))
		cStart = start;
}

void Grammar::AddPrecept(StringLink p)
{
	if (p == "#")
		return;
	StringLink strLeft, strRight;
	string cTemp;
	int iPos = 0;
	unsigned int i;
	for(i = 0; i < p.length(); i ++)
	{
		cTemp = p[i];		
		if (cTemp == "->")
		{
			iPos = i;
			break;
		}
		//strLeft.push_back(cTemp);
		strLeft += cTemp;
		
	}
	cTemp = p.at(++iPos);
	for(i = iPos; i < p.length(); i ++)
	{
		cTemp = p[i];
		//strRight.push_back(cTemp);
		strRight += cTemp;
	}
	Precept precept(strLeft,strRight);
	P.push_back(precept);
	nP = (int) P.size();
}

bool Grammar::IsGrammarLegal()
{
	bool flag = false;
	if (!Vn.Find(cStart))
		return false;
	if (P.empty())
		return false;
	int i;
	for(i = 0; i < Vn.Size(); i ++)
	{
		if(Vt.Find(Vn.GetAt(i)))
			return false;
	}
	for(i = 0; i < Vt.Size(); i ++)
	{
		if(Vn.Find(Vt.GetAt(i)))
			return false;
	}
	for(i = 0; i < P.size(); i ++)
	{
		StringLink strTest;
		strTest = P[i].GetLeft();
		unsigned int j;
		for(j = 0; j < strTest.length(); j ++)
		{
			if ((!Vn.Find(strTest[j]))&&(!Vt.Find(strTest[j]))&&(strTest[j]!="@"))
				return false;
			if ((j==0)&&(strTest[j] == cStart))
				flag = true;
		}
		strTest = P[i].GetRight();
		for(j = 0; j < strTest.length(); j ++)
		{
			if ((!Vn.Find(strTest[j]))&&(!Vt.Find(strTest[j]))&&(strTest[j]!="@"))
				return false;
		}
	}
	return flag;
}

bool Grammar::GeneratePredictTable()
{
	MakeCanReachEmptyTable();
	CalculateFirstSet();
	CalculateFollowSet();
	CalculateSelectSet();
	if(!IsLegalLL1Grammar())
	{
		return false;
	}
	MakePredictTable();
	return true;
}
/*
string Grammar::OutputHTML()
{
	char szTempPath[MAX_PATH]; 
	char szTempName[MAX_PATH]; 
	::GetTempPath(100,szTempPath);
	::GetTempFileName(szTempPath,"LL1",0,szTempName);
	
	CStdioFile out;
	CString temp;	
	out.Open(szTempName, CFile::modeCreate | CFile::modeWrite);
	out.WriteString("<html>\n");
	out.WriteString("<head>\n");
	out.WriteString("<title>Untitled Document</title>\n");
	out.WriteString("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\">\n");
	out.WriteString("</head>\n");
	out.WriteString("<body bgcolor=\"#FFFFFF\" text=\"#000000\">\n");
	out.WriteString("<p>下面是你输入的文法G：<br>\n");
	temp = "非终结符号集合为：{ ";
	for(int i = 0; i < nVn -1; i++)
	{
		//temp.AppendChar(Vn.GetAt(i));
		temp += Vn.GetAt(i);
		temp += ", ";
	}
	//temp.AppendChar(Vn.GetAt(nVn-1));
	temp += Vn.GetAt(nVn-1);
	temp += " }<br>\n";
	out.WriteString(temp);
	temp = "终结符符号集合为：{ ";
	for(i = 0; i < nVt -1; i++)
	{
		//temp.AppendChar(Vt.GetAt(i));
		temp += Vt.GetAt(i);
		temp += ", ";
	}
	//temp.AppendChar(Vt.GetAt(nVt-1));
	temp += Vt.GetAt(nVt-1);
	temp += " }<br>\n";
	out.WriteString(temp);
	temp = "G[";
	//temp.AppendChar(cStart);
	temp += cStart;
	temp += "]:<br>\n";
	out.WriteString(temp);
	for(i = 0; i < nP; i ++)
	{
		out.WriteString(P[i].GetLeft().c_str());
		out.WriteString("->");
		out.WriteString(P[i].GetRight().c_str());
		out.WriteString("<br>\n");
	}
	out.WriteString("<br>可以推出空串的非终结符集合为：{ ");
	temp = "";
	for(i = 0; i < nVn; i ++)
	{
		if(CanReachEmpty(Vn.GetAt(i)))
		{
			CString t;
			t.Format("%c, ", Vn.GetAt(i));
			temp += t;
		}
	}
	if (temp.GetLength() !=0)
	{
		temp = temp.Left(temp.GetLength()-2);
	}
	temp += " }<BR>\n";
	out.WriteString(temp);
	out.WriteString("<br>非终结符号的FIRST集合：<br>\n");
	for(i = 0; i < nVn; i ++)
	{
		out.WriteString("FIRST(");
		temp = "";
		//temp.AppendChar(Vn.GetAt(i));
		temp += Vn.GetAt(i);
		temp += ") = { ";
		for(int j = 0; j < FirstSet[i].Size() - 1; j ++)
		{
			//temp.AppendChar(FirstSet[i].GetAt(j));
			temp += FirstSet[i].GetAt(j);
			temp += ", ";
		}
		//temp.AppendChar(FirstSet[i].GetAt(FirstSet[i].Size()-1));
		temp += FirstSet[i].GetAt(FirstSet[i].Size()-1);
		temp += " }<br>\n";
		out.WriteString(temp);
	}

	out.WriteString("<br>非终结符号的FOLLOW集合：<br>\n");
	for(i = 0; i < nVn; i ++)
	{
		out.WriteString("FOLLOW(");
		temp = "";
		//temp.AppendChar(Vn.GetAt(i));
		temp += Vn.GetAt(i);
		temp += ") = { ";
		for(int j = 0; j < FollowSet[i].Size() - 1; j ++)
		{
			//temp.AppendChar(FollowSet[i].GetAt(j));
			temp += FollowSet[i].GetAt(j);
			temp += ", ";
		}
		//temp.AppendChar(FollowSet[i].GetAt(FollowSet[i].Size()-1));
		temp += FollowSet[i].GetAt(FollowSet[i].Size()-1);
		temp += " }<br>\n";
		out.WriteString(temp);
	}	
	out.WriteString("<br>各产生式的SELECT集合：<br>\n");
	unsigned int iMaxLength = 0;
	for(i = 0; i < nP; i ++)
	{
		iMaxLength = (iMaxLength >= P[i].GetRight().length())?iMaxLength:(P[i].GetRight().length());
		out.WriteString("SELECT(");
		out.WriteString(P[i].GetLeft().c_str());
		out.WriteString("->");
		out.WriteString(P[i].GetRight().c_str());
		temp = ") = { ";

		for(int j = 0; j < SelectSet[i].Size() - 1; j ++)
		{
			//temp.AppendChar(SelectSet[i].GetAt(j));
			temp += SelectSet[i].GetAt(j);
			temp += ", ";
		}
		//temp.AppendChar(SelectSet[i].GetAt(SelectSet[i].Size()-1));
		temp += SelectSet[i].GetAt(SelectSet[i].Size()-1);
		temp += " }<br>\n";
		out.WriteString(temp);
	}	

	if(IsLegalLL1Grammar())
	{
		out.WriteString("<br><b>输入的文法是一个LL(1)文法。</b><br>");	
		iMaxLength ++;
		iMaxLength *= 12;
		out.WriteString("下面是生成的预测分析表：<BR>\n");
		out.WriteString("<table border=\"1\"  cellpadding=\"0\" cellspacing=\"0\" style=\"border-collapse: collapse\" bordercolor=\"#111111\">\n");
		out.WriteString("<tr>\n");
		out.WriteString("<td nowrap width=\"25\">&nbsp;</td>\n");
		for(int i = 0; i < nVt; i ++)
		{
			temp.Format("<td nowrap width=\"25\">&nbsp;%c</td>\n", Vt.GetAt(i));
			out.WriteString(temp);
		}
		temp.Format("<td nowrap width=\"%u\">&nbsp;#</td>\n", iMaxLength);
		out.WriteString(temp);
		
		out.WriteString("</tr>\n");
		for(int x = 0; x < nVn; x ++)
		{
			out.WriteString("<tr>\n");
			temp.Format("<td nowrap width=\"25\">&nbsp;%c</td>\n", Vn.GetAt(x));
			out.WriteString(temp);
			for(int y = 0; y < nVt + 1; y ++)
			{
				temp.Format("<td nowrap width=\"%u\">&nbsp;%s</td>\n", iMaxLength, ((pPredictTable[x*(nVt+1)+y] == -1)?"&nbsp;":P[pPredictTable[x*(nVt+1)+y]].GetRight().c_str()));
				out.WriteString(temp);
			}
			out.WriteString("</tr>\n");
		}
		out.WriteString("</table>\n");
	}
	else
	{
		out.WriteString("<br><font color= \"#ff0000\"<b>输入的文法不是LL(1)文法，不能生成预测分析表<b></font><br>\n");
	}
	out.WriteString("</body>\n");
	out.WriteString("</html>");
	out.Close();
	return szTempName;
}*/

void Grammar::MakeCanReachEmptyTable()
{
	if (pCanEmptyTable != 0)
		delete [] pCanEmptyTable;
	pCanEmptyTable = new CanEmpty[nVn];
	int i;
	for(i = 0; i < nVn; i ++)
		pCanEmptyTable[i] = UNDEFINED;
	bool * pDeleteFlag = new bool[nP];
	for (i = 0; i< nP; i ++)
        pDeleteFlag[i] = false;
	for (i = 0; i < nP; i ++)
	{
		if(pDeleteFlag[i])
			continue;
		StringLink pl, pr;
		pl = P[i].GetLeft();
		pr = P[i].GetRight();
		if (pr == "@")
		{
			pCanEmptyTable[Vn.FindPos(pl[0])] = CANTRUE;
			pDeleteFlag[i] = true;
			int j;
			for (j = 0; j < nP; j ++)
			{
				if (pDeleteFlag[j])
					continue;
				if (P[j].GetLeft() == pl)
					pDeleteFlag[j] = true;
			}
			continue;
		}
		for(unsigned int j = 0; j < pr.length(); j ++)
		{
			if(Vt.Find(pr[j]))
			{
				pDeleteFlag[i] = true;
				bool bDeleteFlag = true;
				int k;
				for (k = 0; k < nP; k ++)
				{	
					if (!pDeleteFlag[k])
					{
						if (P[k].GetLeft() == pl)
						{
							bDeleteFlag = false;
							break;
						}
					}
				}
				if (bDeleteFlag)
				{
					pCanEmptyTable[Vn.FindPos(pl[0])] = CANFALSE;
				}
				break;
			}
		}
	}
	bool bRepeat;
	do
	{
		bRepeat = false;
		for(int i = 0; i < nP; i ++)
		{
			if (pDeleteFlag[i])
				continue;
			StringLink pl, pr;
			pl = P[i].GetLeft();
			pr = P[i].GetRight();
			Set set;
			unsigned int j;
			for(j = 0; j < pr.length(); j ++)
				set.Insert(pr[j]);
			for(j = 0; j < pr.length(); j ++)
			{
				string cN = pr[j];
				int k;
				if(Vn.Find(cN))
				{
					bool bDelete;
					switch(pCanEmptyTable[Vn.FindPos(cN)])
					{
					case CANTRUE:
						set.Delete(cN);
						if(set.IsEmpty())
						{
							pCanEmptyTable[Vn.FindPos(pl[0])] = CANTRUE;
							bRepeat = true;
							for (int k = 0; k < nP; k ++)
							{
								if (pDeleteFlag[k])
									continue;
								if (P[k].GetLeft() == pl)
									pDeleteFlag[k] = true;
							}
						}
						break;
					case CANFALSE:
						pDeleteFlag[i] = true;
						bDelete = true;
						for (k = 0; k < nP; k ++)
						{	
							if (!pDeleteFlag[k])
							{
								if (P[k].GetLeft() == pl)
								{
									bDelete = false;
									break;
								}
							}
						}
						if (bDelete)
						{
							pCanEmptyTable[Vn.FindPos(pl[0])] = CANFALSE;
							bRepeat = true;
						}
						break;
					case UNDEFINED:
						break;
					}
				}
			}
		}
	}while(bRepeat);
	delete [] pDeleteFlag;
}

void Grammar::CalculateFirstSet()
{
	if(pCanEmptyTable == 0)
		MakeCanReachEmptyTable();
	for(int i = 0; i < nVn; i ++)
	{
		Set tempSet;
		FirstSet.push_back(tempSet);
	}

	bool flag;
	do
	{
		flag = false;
		StringLink strLeft, strRight;
		for(int i = 0; i< nP; i ++)
		{
			strLeft = P[i].GetLeft();
			strRight = P[i].GetRight();
			int iPos = Vn.FindPos(strLeft[0]);
			if (Vn.Find(strLeft[0]))
			{
				if ((strRight == "@")||(Vt.Find(strRight[0])))
					flag |= FirstSet[iPos].Insert(strRight[0]);
				else
				{
					unsigned int j = 0;
					do
					{
						string cChar = strRight[j];
						if (Vn.Find(cChar))
						{
							flag |= (FirstSet[iPos].Add((GetFirstSet(cChar) - Set("@"))) != 0);
							if (!CanReachEmpty(cChar))
								break;
						}
						j ++;
					}while(j < strRight.length());
					if (j == strRight.length())
						flag |= FirstSet[iPos].Insert("@");
				}
			}
		}
	}while(flag);
}

void Grammar::CalculateFollowSet()
{
	if(pCanEmptyTable == 0)
		MakeCanReachEmptyTable();
	for(int i = 0; i < nVn; i ++)
	{
		Set tempSet;
		FollowSet.push_back(tempSet);
	}
	FollowSet[Vn.FindPos(cStart)].Insert("#");
	bool flag = false;
	do
	{
		flag = false;
		for(int i = 0; i < nP; i ++)
		{
			StringLink strLeft, strRight;
			strLeft = P[i].GetLeft();
			strRight = P[i].GetRight();

			for ( unsigned int j = 0; j < strRight.length() - 1; j++)
			{
				string cChar = strRight[j];
				if (Vn.Find(cChar))
				{
					StringLink temp;
					for(unsigned int k = j + 1; k < strRight.length(); k++)
						//temp.push_back(strRight[k]);
						temp += strRight[k];
					Set firstset = GetFirstSet(temp);
					flag |= (FollowSet[Vn.FindPos(cChar)].Add(firstset - Set("@")) != 0);
					if (CanReachEmpty(temp))
					{
						flag |= (FollowSet[Vn.FindPos(cChar)].Add(GetFollowSet(strLeft[0])) != 0);
					}
				}
			}
			if (Vn.Find(strRight[strRight.length() -1]))
				flag |= (FollowSet[Vn.FindPos(strRight[strRight.length() -1])].Add(GetFollowSet(strLeft[0])) != 0);
		}
	}while(flag);
}

void Grammar::CalculateSelectSet()
{
	for(int i = 0; i < nP; i ++)
	{
		Set selectSet;
		selectSet = GetFirstSet(P[i].GetRight());
		if (CanReachEmpty(P[i].GetRight()))
			selectSet.Add(GetFollowSet(P[i].GetLeft()[0]));
		SelectSet.push_back(selectSet);
	}
}

bool Grammar::IsLegalLL1Grammar()
{
	if (iIsLL1 == 1)
		return true;
	if (iIsLL1 == 2)
		return false;
	bool * pDeleteFlag = new bool[nP];
	int i;
	for(i = 0; i < nP; i ++)
		pDeleteFlag[i] = false;
	for(i = 0; i < nP; i ++)
	{
		if (pDeleteFlag[i])
			continue;
		StringLink strLeft = P[i].GetLeft();
		Set set = SelectSet[i];
		Set setb;
		for(int j = i + 1; j < nP; j ++)
		{
			if (pDeleteFlag[i])
				continue;
			if (P[j].GetLeft() == strLeft)
			{
				setb = SelectSet[j];
				if (set.Add(setb) != setb.Size())
				{
					iIsLL1 = 2;
					delete []pDeleteFlag;
					return false;
				}
			}
		}
	}
	delete [] pDeleteFlag;
	iIsLL1 = 1;
	return true;
}



bool Grammar::CanReachEmpty(StringLink str)
{
	bool flag = true;
	if (str != "@")
	{
		for (unsigned int i = 0; i < str.length(); i++)
		{
			int iPos = Vn.FindPos(str[i]);
			if (iPos != -1)
			{
				if (pCanEmptyTable[iPos] == CANFALSE)
				{
					flag = false;
					break;
				}
			}
			else
			{
				flag = false;
				break;
			}
		}
	}
	return flag;
}

bool Grammar::CanReachEmpty(string cChar)
{
	int iPos;
	if((iPos = Vn.FindPos(cChar)) != -1)
	{
		if(pCanEmptyTable[iPos] == CANTRUE)
			return true;
		else
			return false;
	}
	else
		return false;
		
}

Set Grammar::GetFirstSet(string cChar)
{
	if(Vt.Find(cChar) || (cChar == "@"))
	{
		Set set;
		set.Insert(cChar);
		return set;
	}
	else
	{
		return FirstSet[Vn.FindPos(cChar)];	
	}
}

Set Grammar::GetFirstSet(StringLink str)
{
	Set firstset;
	if ((Vt.Find(str[0]))||(str == "@"))
	{
		firstset.Insert(str[0]);
	}
	else
	{
		unsigned int i;
		for (i = 0; i < str.length(); i++)
		{
			string cChar = str[i];
			if (Vn.Find(cChar))
			{
				firstset.Add(GetFirstSet(cChar) - Set("@"));
				if (!CanReachEmpty(cChar))
					break;
			}
			else
			{
				firstset.Insert(cChar);
				break;
			}
				
		}
		if (i == str.length())
			firstset.Insert("@");
	}
	return firstset;
}

Set Grammar::GetFollowSet(string cChar)
{
	int iPos;
	if ((iPos = Vn.FindPos(cChar)) != -1)
		return FollowSet[iPos];
	else
		return Set();
}

void Grammar::MakePredictTable()
{
	Set Select;
	if (pPredictTable!=0)
		delete []pPredictTable;
	pPredictTable = new int [nVn*(nVt+1)];
	int i;
	for(i = 0; i < nVn*(nVt+1); i++)
		pPredictTable[i] = -1;
	for(i = 0; i < nP; i++)
	{
		Select = SelectSet[i];
		for(int j = 0; j < Select.Size(); j++)
		{
			if (Select.GetAt(j) != "@")
				pPredictTable[Vn.FindPos(P[i].GetLeft()[0])*(nVt+1)+((Select.GetAt(j) == "#")?\
				(nVt):(Vt.FindPos(Select.GetAt(j))))] = i;
		}
	}
}

void Grammar::Output()
{
	int i,j;
	for( i = 0; i < nVt; i++)
	{
		cout<<Vt.GetAt(i)<<',';
		
	}
	cout<<endl;
	for(i = 0; i < nVn; i++)
	{
		cout<<Vn.GetAt(i)<<',';
	}
	cout<<endl;
	for(i = 0; i < nP; i++)
	{
		StringLink t;
		t=P[i].GetLeft();
		for (j=0;j<t.length();j++)
			cout<<t[j]<<',';
		cout<<"->";
		t=P[i].GetRight();
		for (j=0;j<t.length();j++)
			cout<<t[j]<<',';
		cout<<endl;
	}
	cout<<"CanReachEmptySet:"<<endl;
	for(i = 0; i < nVn; i ++)
	{
		if(CanReachEmpty(Vn.GetAt(i)))
		{
			cout<<Vn.GetAt(i)<<',';
		}
	}
	cout<<endl;
	cout<<"FirstSet:"<<endl;
	for(i = 0; i < nVn; i ++)
	{
		cout<<Vn.GetAt(i)<<'\t';
		for(int j = 0; j < FirstSet[i].Size() - 1; j ++)
		{
			cout<<FirstSet[i].GetAt(j)<<'\t';
		}
		cout<<FirstSet[i].GetAt(FirstSet[i].Size()-1)<<endl;
	}
	cout<<"FollowSet:"<<endl;
	for(i = 0; i < nVn; i ++)
	{
		cout<<Vn.GetAt(i)<<'\t';
		for(int j = 0; j < FollowSet[i].Size() - 1; j ++)
		{
			cout<<FollowSet[i].GetAt(j)<<'\t';
		}
		cout<<FollowSet[i].GetAt(FollowSet[i].Size()-1)<<endl;
	}
	cout<<"SelectSet:"<<endl;
	for(i = 0; i < nP; i ++)
	{
		StringLink t;
		t=P[i].GetLeft();
		for (j=0;j<t.length();j++)
			cout<<t[j]<<',';
		cout<<"->";
		t=P[i].GetRight();
		for (j=0;j<t.length();j++)
			cout<<t[j]<<',';
		cout<<endl;
		for(int j = 0; j < SelectSet[i].Size() - 1; j ++)
		{
			//t.AppendChar(SelectSet[i].GetAt(j));
			cout<<SelectSet[i].GetAt(j)<<'\t';
		}
		cout<<SelectSet[i].GetAt(SelectSet[i].Size()-1)<<endl;
	}
}
/*
void Grammar::Output(char * pFilename)
{
	if (pFilename == 0)
		return;
	FILE* OutFile;
	OutFile.fopen(pFilename, CFile::modeCreate | CFile::modeWrite);
	CString t;
	OutFile.WriteString("[Terminator]\n");
	for(int i = 0; i < nVt; i++)
	{
		t.Format("%c\n", Vt.GetAt(i));
		OutFile.WriteString(t);
		
	}
	OutFile.WriteString("\n[NonTerminator]\n");
	for(i = 0; i < nVn; i++)
	{
		t.Format("%c\n", Vn.GetAt(i));
		OutFile.WriteString(t);
	}
	OutFile.WriteString("\n[Starter]\n");
	t.Format("%c", cStart);
	t += "\n\n[Precept]\n";
	OutFile.WriteString(t);
	for(i = 0; i < nP; i++)
	{
		t.Format("%s->%s\n", P[i].GetLeft().c_str(), P[i].GetRight().c_str());
		OutFile.WriteString(t);
	}
	OutFile.WriteString("\n[CanReachEmptySet]\n");
	for(i = 0; i < nVn; i ++)
	{
		if(CanReachEmpty(Vn.GetAt(i)))
		{
			t.Format("%c\n", Vn.GetAt(i));
			OutFile.WriteString(t);
		}
	}
	OutFile.WriteString("\n[FirstSet]\n");
	for(i = 0; i < nVn; i ++)
	{
		t.Format("%c = ",Vn.GetAt(i));
		for(int j = 0; j < FirstSet[i].Size() - 1; j ++)
		{
			//t.AppendChar(FirstSet[i].GetAt(j));
			t += FirstSet[i].GetAt(j);
			t += ", ";
		}
		//t.AppendChar(FirstSet[i].GetAt(FirstSet[i].Size()-1));
		t += FirstSet[i].GetAt(FirstSet[i].Size()-1);
		t += " \n";
		OutFile.WriteString(t);
	}
	OutFile.WriteString("\n[FollowSet]\n");
	for(i = 0; i < nVn; i ++)
	{
		t.Format("%c = ",Vn.GetAt(i));
		for(int j = 0; j < FollowSet[i].Size() - 1; j ++)
		{
			//t.AppendChar(FollowSet[i].GetAt(j));
			t += FollowSet[i].GetAt(j);
			t += ", ";
		}
		//t.AppendChar(FollowSet[i].GetAt(FollowSet[i].Size()-1));
		t += FollowSet[i].GetAt(FollowSet[i].Size()-1);
		t += " \n";
		OutFile.WriteString(t);
	}
	OutFile.WriteString("\n[SelectSet]\n");
	for(i = 0; i < nP; i ++)
	{
		t.Format("%s->%s = ",P[i].GetLeft().c_str(),P[i].GetRight().c_str());
		for(int j = 0; j < SelectSet[i].Size() - 1; j ++)
		{
			//t.AppendChar(SelectSet[i].GetAt(j));
			t += SelectSet[i].GetAt(j);
			t += ", ";
		}
		//t.AppendChar(SelectSet[i].GetAt(SelectSet[i].Size()-1));
		t += SelectSet[i].GetAt(SelectSet[i].Size()-1);
		t += " \n";
		OutFile.WriteString(t);
	}
	OutFile.WriteString("\n");
	if(IsLegalLL1Grammar())
	{
		OutFile.WriteString("[PredictTable]\n");
		for(int x = 0; x < nVn; x ++)
		{
			for(int y = 0; y < nVt + 1; y ++)
			{
				if (pPredictTable[x*(nVt+1)+y] != -1)
				{
					t.Format("%c, %c = %s\n", Vn.GetAt(x), ((y != nVt)?Vt.GetAt(y):'#'), P[pPredictTable[x*(nVt+1)+y]].GetRight().c_str());
					OutFile.WriteString(t);
				}
			}

		}
	}
	OutFile.Close();
}*/

Grammar::Grammar(const Grammar & g)
{
	if (&g == this)
		return;
	Vt = g.Vt;
	Vn = g.Vn;
	cStart = g.cStart;
	P = g.P;
	FirstSet = g.FirstSet;
	FollowSet = g.FollowSet;
	SelectSet = g.SelectSet;
	nVt = g.nVt;
	nVn = g.nVn;
	nP = g.nP;
	if (g.pCanEmptyTable !=0)
	{
		if (pCanEmptyTable!=0)
			delete[] pCanEmptyTable;
		pCanEmptyTable = new CanEmpty[nVn];
		for (int i = 0; i < nVn; i ++)
			pCanEmptyTable[i] = g.pCanEmptyTable[i];
	}
	if(g.pPredictTable != 0)
	{
		if (pPredictTable!=0)
			delete []pPredictTable;
		pPredictTable = new int[nVn * (nVt + 1)];
		for (int i = 0; i < nVn * (nVt + 1); i ++)
			pPredictTable[i] = g.pPredictTable[i];
	}
}

const Grammar Grammar::operator = (const Grammar & g)
{
	if (&g == this)
		return * this;
	Vt = g.Vt;
	Vn = g.Vn;
	cStart = g.cStart;
	P = g.P;
	FirstSet = g.FirstSet;
	FollowSet = g.FollowSet;
	SelectSet = g.SelectSet;
	nVt = g.nVt;
	nVn = g.nVn;
	nP = g.nP;
	if (g.pCanEmptyTable !=0)
	{
		if (pCanEmptyTable!=0)
			delete []pCanEmptyTable;
		pCanEmptyTable = new CanEmpty[nVn];
		for (int i = 0; i < nVn; i ++)
			pCanEmptyTable[i] = g.pCanEmptyTable[i];
	}
	if(g.pPredictTable != 0)
	{
		if (pPredictTable!=0)
			delete []pPredictTable;
		pPredictTable = new int[nVn * (nVt + 1)];
		for (int i = 0; i < nVn * (nVt + 1); i ++)
			pPredictTable[i] = g.pPredictTable[i];
	}
	return *this;
}

bool Grammar::IsInVn(string  cChar)
{
	return Vn.Find(cChar);
}

bool Grammar::IsInVt(string  cChar)
{
	return Vt.Find(cChar);
}

string Grammar::GetStart()
{
	return cStart;
}

Precept Grammar::GetToDo(string vn, string vt)
{
	int i;
	if ((i = pPredictTable[Vn.FindPos(vn) * (nVt + 1) + ((vt == "#")?nVt:Vt.FindPos(vt))]) != -1)
		return P[i];
	else
	{
		return Precept();
	}
}