//Projection.cpp
//
#include "Projection.h"

Projection::Projection() :
ini("")
{
	initObject();
}

Projection::Projection(string fname,string iname):
filename(fname), ininame(iname), ini(iname.c_str())
{
	initObject();
	projectionLoad();	//隐式加载文法表
}

Projection::~Projection()
{
	clearMemory();
}

void Projection::initObject(bool all)
{
	productTable=NULL;
	predictTable=NULL;
	terminal="";
	nonterminal="";
//	productLeft="";
	grammarLength=0;
	tableLength=0;
	tableHeight=0;
	tableWidth=0;
	start="";
	isLoaded=false;
	
	if (all)	//如果参数为否，则不清空
		lastError="";
}

void Projection::clearMemory()
{
	if (productTable)
	{
		for (int i=0;i<grammarLength;i++)
			for (int j=0;j<productTable[i].length();j++)
				productTable[i]="";
	}
	if (predictTable)
		delete []predictTable;
	productTable=NULL;
	predictTable=NULL;
}

string Projection::projectionLoad(string fname,string iname)
{
	if (isLoaded)	//如果已经加载，则清空分配空间并初始化对象，重新加载
	{
		clearMemory();
		initObject(0);
		isLoaded=true;
	}
	//如果未加载，且传入参数，则应该文件信息和词库信息同时具备
	if (fname!="")		//如果传进参数是文件信息
	{
		filename=fname;
		if (iname!="")		//如果传进参数是文法词库信息
		{
			ininame=iname;

			ini.loadFile(ininame.c_str());
		}
		else
		{
			lastError="文件";
			lastError+=filename;
			lastError+="没有匹配的词库信息";
			return lastError;
		}
	}
	if (!createPredictTable())
	{
		clearMemory();	//如果加载失败，清空分配的空间，下次还可重新加载
		initObject(0);
		return lastError;
	}
	else
	{
		if (isLoaded)	//如果已经加载过了，则显示重新加载成功
		{
			lastError="文件";
			lastError+=filename;
			lastError+="已经重新加载";
		}
		isLoaded=true;	//标记已经加载
		return lastError;
	}
}

int Projection::isSeparate(const char* s)
{
	if (s[0]=='-'&&s[1]=='>')
		return -1;
	if (s[0]==' '||s[0]=='@')
		return 1;
	return 0;
}

bool Projection::createPredictTable()
{
	int i,j;
	ifstream in(filename.c_str());
	if (in.fail())	//如果文件打开错误，则返回
	{
		lastError="指定文件";
		lastError+=filename;
		lastError+="不存在";
		return false;
	}
	//首先获取第一行，是否符合标准
	string line;
    getline(in,line);
	if (line != "[TERMINAL]")
	{
		in.close();
		lastError="文件";
		lastError+=filename;
		lastError+="不合法:[TERMINAL]";
		return false;
	}
	string terminals="";	//暂存终结符
	int count=0;			//统计[TERMIANL]段的行数
	for (i=0;!in.eof();i++)
	{
		getline(in,line);
		count++;
		if (line[0]=='\0'||line=="[LL1]")	//直到[LL1]
			break;
		terminals+=line;
		terminals+=' ';
	}
	if (in.eof())
	{
		in.close();
		lastError="文件";
		lastError+=filename;
		lastError+="不合法:[LL1]";
		return false;
	}
	string temp;
	temp="";
	const char *pc=terminals.c_str();
	for (i=0,j=terminals.length();i<j;i++)
	{
		if (!isSeparate(pc+i))
		{
			temp+=terminals[i];
		}
		else
		{
			if (temp!="")
			{
				terminal+=temp;
			}
			temp="";
		}
	}
//	for (i=0;i<terminal.length();i++)cout<<terminal[i]<<",";cout<<endl;
	
	grammarLength=0;
	for (i=0;!in.eof();i++)	//预先查看行数(文法条数),注意这里文件指针已经到达[LL1]段
	{
		getline(in,line);
		if (line[0]=='\0')
			break;
		grammarLength++;
	}
//	cout<<grammarLength<<endl;

	in.clear();		//清流状态标志，用于解决前面获取行数时读取到EOF后文件指针回不去的问题
	in.seekg(0);
	for (;count>=0;count--)
		getline(in,line);	//去除[TERMINAL]段

	Grammar g;
	g.SetVt(terminal);

	productTable=new StringLink[grammarLength];	//按行数分配文法空间
	line="";
	temp="";
	int len;
	for (i=0;i<grammarLength;i++)
	{
		getline(in,line);
		line+=' ';
		len=line.length();
		pc=line.c_str();
		//cout<<"Line:"<<line<<endl;
		temp="";
		for (int j=0;j<len;j++)
		{
			int m=isSeparate(pc+j);
			if (!m)
			{
				temp+=line[j];
			}
			else
			{
				if (temp!="")
				{
					productTable[i]+=temp;
					int j;
					for (j=0;j<terminal.length();j++)	//查找该(非)终结符是否在终结符中出现
					{
						//cout<<terminal[j]<<',';
						if (terminal[j]==temp)
							break;
					}
					if (j>=terminal.length())				//不在
					{
						for (j=0;j<nonterminal.length();j++)		//查找该(非)终结符是否在非终结符中出现
						{
							if (nonterminal[j]==temp)
								break;
						}
						if (j>=nonterminal.length())				//不在，则写入非终结符中
							nonterminal+=temp;
					}
				}
				temp="";
				if (m==-1)	//如果是"->"则j多加一次
				{
					productTable[i]+="->";
					j++;
				}
				if (line[j]=='@')
				{
					productTable[i]+="@";
				}
			}
		}
	}
	//cout<<nonterminal.length()<<','<<endl;
	//for (i=0;i<nonterminal.length();i++)cout<<nonterminal[i]<<'\t';cout<<endl;

	g.SetVn(nonterminal);
	start=nonterminal[0];
	g.SetStart(start);
	for (i=0;i<grammarLength;i++)
	{
		g.AddPrecept(productTable[i]);
	/*	for (j=0;j<productTable[i].length();j++)
			cout<<productTable[i][j]<<" ";
		cout<<endl;*/
	}

	if (g.IsGrammarLegal())	//如果语法正确
	{
	//	cout<<"OK"<<endl;
		if (!g.GeneratePredictTable())
		{
			in.close();
			lastError="文件";
			lastError+=filename;
			lastError+="不是标准的LL1文法文件";
			return false;
		}
	//	g.Output();
		predictTable=new int[g.nVn*(g.nVt+1)];	//分配预测表空间
		for( i = 0; i <g.nVn*(g.nVt+1); i++)	//制作预测表
		{
			predictTable[i]=g.pPredictTable[i];
			if (i%(g.nVt+1)==0&&i&&g.pCanEmptyTable[(i-g.nVt-1)/(g.nVt+1)]==0)
			{
				predictTable[i-1]=-2;
			//	cout<<i<<endl;
			}
		}
		i--;
		if (g.pCanEmptyTable[(i-g.nVt)/g.nVt]==0)	//为预测表添加可空集处理
			predictTable[i]=-2;
	}
	else	//如果语法错误，那么返回
	{
		in.close();
		lastError=filename;
		lastError+="语法错误";
		return false;
	}
	tableLength=g.nVn*(g.nVt+1);
	tableHeight=g.nVn;
	tableWidth=g.nVt+1;
	in.close();
	lastError=filename;
	lastError+="加载成功";
	return true;
}

void Projection::projectionView(int mode)
{
	if (isLoaded)
	{
		int i=0,j=0;
		if (mode&PRODUCT)
		{
			for (i=0;i<grammarLength;i++)
			{
				for (j=0;j<productTable[i].length();j++)
					cout<<productTable[i][j];
				cout<<' ';
			}
			cout<<endl;
		}
		if (mode&TERMINAL)
		{
			for (i=0;i<terminal.length();i++)cout<<terminal[i]<<' ';cout<<endl;
		}
		if (mode&NONTERMINAL)
		{
			for (i=0;i<nonterminal.length();i++)cout<<nonterminal[i]<<' ';cout<<endl;
		}
		if (mode&NUMTABLE)
		{
			//显示文法预测分析表，数字形式
			cout<<"------";for (i=0;i<tableWidth-1;i++)cout<<"-----";cout<<endl<<"\t|";
			for (i=0;i<tableWidth-1;i++)cout<<terminal[i]<<" ";cout<<'@'<<endl;
			j=0;
			for (i = 0; i <tableLength; i++)
			{
				if ((i)%tableWidth==0)
					cout<<nonterminal[j++]<<"\t|";
				if (predictTable[i]==-1)
					cout<<"-   ";
				else
					cout<<predictTable[i]<<"  ";
				if (predictTable[i]>=0&&predictTable[i]<10)
					cout<<" ";
				if ((i+1)%tableWidth==0)
					cout<<endl;
			}
			if (!(mode&PROTABLE))
				cout<<"------";for (i=0;i<tableWidth-1;i++)cout<<"-----";cout<<endl;
		}
		if (mode&PROTABLE)
		{
			//显示文法预测分析表，首字符形式
			j=0;
			for (i=0;i<tableWidth-1;i++)cout<<"------";cout<<endl<<"\t|";
			for (i=0;i<tableWidth-1;i++)cout<<terminal[i]<<" ";
			cout<<'@'<<endl;
			for (i = 0; i <tableLength; i++)
			{
				if ((i)%tableWidth==0)
					cout<<nonterminal[j++]<<"\t|";
				if (predictTable[i]>=0)
				{
					cout<<productTable[predictTable[i]][0];
					for (int k=productTable[predictTable[i]][0].length();k<5;k++)
						cout<<' ';
				}
				else if (predictTable[i]==-2)
					cout<<"@    ";
				else
					cout<<"-    ";
				if ((i+1)%tableWidth==0)
					cout<<endl;
			}
			for (i=0;i<tableWidth-1;i++)cout<<"------";cout<<endl;
		}
	}
}

bool Projection::outProductionToFile(string fileout)
{
	if (!isLoaded)
	{
		lastError="文件";
		lastError+=filename;
		lastError+="未生成任何信息";
		return false;
	}
	if (""==fileout)
	{
		lastError="参数不正确";
		return true;
	}
	ofstream out(fileout.c_str());
	if (!out.fail())	//如果文件打开失败，则什么也不做
	{
		for(int i = 0; i <grammarLength; i++)
		{
			for (int j=0;j<productTable[i].length();j++)
			{
				out<<productTable[i][j]<<(j>2?" ":"");
			}
			out<<endl;
		}
		lastError="已将";
		lastError+=filename;
		lastError+="的文法输出至文件";
		lastError+=fileout;
		out.close();
		return true;
	}
	else
	{
		lastError="输出文件";
		lastError+=fileout;
		lastError+="出错";
		out.close();
		return false;
	}
}

bool Projection::outPredictTableToFile(string fileout)
{
	if (!isLoaded)
	{
		lastError="文件";
		lastError+=filename;
		lastError+="未生成任何信息";
		return false;
	}
	if (""==fileout)
	{
		lastError="参数不正确";
		return true;
	}
	ofstream out(fileout.c_str());
	if (!out.fail())	//如果文件打开失败，则什么也不做
	{
		for(int i = 0; i <tableLength; i++)
		{
			if (predictTable[i]==-2)
			{
				out<<"@  \t";
			}
			else if (predictTable[i]!=-1)
			{
				int k=predictTable[i];
				int len=0;
				for (int j=0;j<productTable[k].length();j++)
				{
					out<<productTable[k][j]<<(j>=2?" ":"");
					len+=productTable[k][j].length();
					j>=2?len++:len;
				}
				if (len<8)
					out<<'\t';
			}
			else
				out<<"----\t";
			out<<"\t";
			if ((i+1)%tableWidth==0)
				out<<endl;
		}
		lastError="已将";
		lastError+=filename;
		lastError+="的预测分析表输出至文件";
		lastError+=fileout;
		out.close();
		return true;
	}
	else
	{
		lastError=filename;
		lastError+="的预测分析表输出到文件";
		lastError+=fileout;
		lastError+="时出错";
		out.close();
		return false;
	}
}

int Projection::findPos(const string& s,bool mode) const
{
	int j;
	if (mode)
	{
		for (j=0;j<terminal.length();j++)
			if (s==terminal[j])
				break;
		if (j<terminal.length())
		{
			return j;
		}
		return -1;
	}
	else
	{
		for (j=0;j<nonterminal.length();j++)
			if (s==nonterminal[j])
				break;
		if (j<nonterminal.length())
		{
			return j;
		}
		return -1;
	}
}
