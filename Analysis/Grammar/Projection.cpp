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
	projectionLoad();	//��ʽ�����ķ���
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
	
	if (all)	//�������Ϊ�������
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
	if (isLoaded)	//����Ѿ����أ�����շ���ռ䲢��ʼ���������¼���
	{
		clearMemory();
		initObject(0);
		isLoaded=true;
	}
	//���δ���أ��Ҵ����������Ӧ���ļ���Ϣ�ʹʿ���Ϣͬʱ�߱�
	if (fname!="")		//��������������ļ���Ϣ
	{
		filename=fname;
		if (iname!="")		//��������������ķ��ʿ���Ϣ
		{
			ininame=iname;

			ini.loadFile(ininame.c_str());
		}
		else
		{
			lastError="�ļ�";
			lastError+=filename;
			lastError+="û��ƥ��Ĵʿ���Ϣ";
			return lastError;
		}
	}
	if (!createPredictTable())
	{
		clearMemory();	//�������ʧ�ܣ���շ���Ŀռ䣬�´λ������¼���
		initObject(0);
		return lastError;
	}
	else
	{
		if (isLoaded)	//����Ѿ����ع��ˣ�����ʾ���¼��سɹ�
		{
			lastError="�ļ�";
			lastError+=filename;
			lastError+="�Ѿ����¼���";
		}
		isLoaded=true;	//����Ѿ�����
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
	if (in.fail())	//����ļ��򿪴����򷵻�
	{
		lastError="ָ���ļ�";
		lastError+=filename;
		lastError+="������";
		return false;
	}
	//���Ȼ�ȡ��һ�У��Ƿ���ϱ�׼
	string line;
    getline(in,line);
	if (line != "[TERMINAL]")
	{
		in.close();
		lastError="�ļ�";
		lastError+=filename;
		lastError+="���Ϸ�:[TERMINAL]";
		return false;
	}
	string terminals="";	//�ݴ��ս��
	int count=0;			//ͳ��[TERMIANL]�ε�����
	for (i=0;!in.eof();i++)
	{
		getline(in,line);
		count++;
		if (line[0]=='\0'||line=="[LL1]")	//ֱ��[LL1]
			break;
		terminals+=line;
		terminals+=' ';
	}
	if (in.eof())
	{
		in.close();
		lastError="�ļ�";
		lastError+=filename;
		lastError+="���Ϸ�:[LL1]";
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
	for (i=0;!in.eof();i++)	//Ԥ�Ȳ鿴����(�ķ�����),ע�������ļ�ָ���Ѿ�����[LL1]��
	{
		getline(in,line);
		if (line[0]=='\0')
			break;
		grammarLength++;
	}
//	cout<<grammarLength<<endl;

	in.clear();		//����״̬��־�����ڽ��ǰ���ȡ����ʱ��ȡ��EOF���ļ�ָ��ز�ȥ������
	in.seekg(0);
	for (;count>=0;count--)
		getline(in,line);	//ȥ��[TERMINAL]��

	Grammar g;
	g.SetVt(terminal);

	productTable=new StringLink[grammarLength];	//�����������ķ��ռ�
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
					for (j=0;j<terminal.length();j++)	//���Ҹ�(��)�ս���Ƿ����ս���г���
					{
						//cout<<terminal[j]<<',';
						if (terminal[j]==temp)
							break;
					}
					if (j>=terminal.length())				//����
					{
						for (j=0;j<nonterminal.length();j++)		//���Ҹ�(��)�ս���Ƿ��ڷ��ս���г���
						{
							if (nonterminal[j]==temp)
								break;
						}
						if (j>=nonterminal.length())				//���ڣ���д����ս����
							nonterminal+=temp;
					}
				}
				temp="";
				if (m==-1)	//�����"->"��j���һ��
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

	if (g.IsGrammarLegal())	//����﷨��ȷ
	{
	//	cout<<"OK"<<endl;
		if (!g.GeneratePredictTable())
		{
			in.close();
			lastError="�ļ�";
			lastError+=filename;
			lastError+="���Ǳ�׼��LL1�ķ��ļ�";
			return false;
		}
	//	g.Output();
		predictTable=new int[g.nVn*(g.nVt+1)];	//����Ԥ���ռ�
		for( i = 0; i <g.nVn*(g.nVt+1); i++)	//����Ԥ���
		{
			predictTable[i]=g.pPredictTable[i];
			if (i%(g.nVt+1)==0&&i&&g.pCanEmptyTable[(i-g.nVt-1)/(g.nVt+1)]==0)
			{
				predictTable[i-1]=-2;
			//	cout<<i<<endl;
			}
		}
		i--;
		if (g.pCanEmptyTable[(i-g.nVt)/g.nVt]==0)	//ΪԤ�����ӿɿռ�����
			predictTable[i]=-2;
	}
	else	//����﷨������ô����
	{
		in.close();
		lastError=filename;
		lastError+="�﷨����";
		return false;
	}
	tableLength=g.nVn*(g.nVt+1);
	tableHeight=g.nVn;
	tableWidth=g.nVt+1;
	in.close();
	lastError=filename;
	lastError+="���سɹ�";
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
			//��ʾ�ķ�Ԥ�������������ʽ
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
			//��ʾ�ķ�Ԥ����������ַ���ʽ
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
		lastError="�ļ�";
		lastError+=filename;
		lastError+="δ�����κ���Ϣ";
		return false;
	}
	if (""==fileout)
	{
		lastError="��������ȷ";
		return true;
	}
	ofstream out(fileout.c_str());
	if (!out.fail())	//����ļ���ʧ�ܣ���ʲôҲ����
	{
		for(int i = 0; i <grammarLength; i++)
		{
			for (int j=0;j<productTable[i].length();j++)
			{
				out<<productTable[i][j]<<(j>2?" ":"");
			}
			out<<endl;
		}
		lastError="�ѽ�";
		lastError+=filename;
		lastError+="���ķ�������ļ�";
		lastError+=fileout;
		out.close();
		return true;
	}
	else
	{
		lastError="����ļ�";
		lastError+=fileout;
		lastError+="����";
		out.close();
		return false;
	}
}

bool Projection::outPredictTableToFile(string fileout)
{
	if (!isLoaded)
	{
		lastError="�ļ�";
		lastError+=filename;
		lastError+="δ�����κ���Ϣ";
		return false;
	}
	if (""==fileout)
	{
		lastError="��������ȷ";
		return true;
	}
	ofstream out(fileout.c_str());
	if (!out.fail())	//����ļ���ʧ�ܣ���ʲôҲ����
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
		lastError="�ѽ�";
		lastError+=filename;
		lastError+="��Ԥ�������������ļ�";
		lastError+=fileout;
		out.close();
		return true;
	}
	else
	{
		lastError=filename;
		lastError+="��Ԥ�������������ļ�";
		lastError+=fileout;
		lastError+="ʱ����";
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
