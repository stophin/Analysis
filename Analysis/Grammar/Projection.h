//Projection.h
//
#include <conio.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include "Grammar.h"
#include "Precept.h"
#include "Set.h"
#include "StringLink.h"

#include "IniFile.h"

//���岻ͬ���ķ����������ʾ��ʽ
#define PRODUCT		0x0001
#define TERMINAL	0x0002
#define NONTERMINAL	0x0004
#define NUMTABLE	0x0008
#define PROTABLE	0x0010

class Projection
{
public:
	Projection();
	Projection(string fname,string iname);
	~Projection();
public:
	bool isLoaded;				//����Ƿ��Ѿ����أ���ֹ�ظ�����
	string filename;			//�ķ��ļ�
	string ininame;				//����ķ������Ĵʵ�ini�ļ�
	int grammarLength;			//�ķ�����
	StringLink terminal;		//�ս��
	StringLink nonterminal;		//���ս��
	StringLink *productTable;	//�ķ���
	string start;				//�ķ���ʼ��
	int *predictTable;			//Ԥ�������	
	int tableLength;			//Ԥ�������ĳ���
	int tableHeight;			//Ԥ�������ĸ߶�
	int tableWidth;				//Ԥ�������Ŀ��
	string lastError;			//���һ�δ���
private:
	bool createPredictTable();		//��ȡԤ���
	void clearMemory();				//��շ���Ŀؼ�
	void initObject(bool all=true);	//��ʼ��������������
	int isSeparate(const char* s);	//���طָ�������
public:	
	//����ӳ�䣬�������ʱδ�����κ��ļ���Ϣ�������򲻵��ô˺���
	//��������κ�һ�����ι��캯�������ڵ�һ�������ض����ķ����ļ���Ϣ��
	//���ʱ���Ե��ô˺��������ķ���
	//�ⲿ����ֻ����޲ι��캯��֮��ĺ������ã��вι���֮�󽫲�����Ӧ�ò���
	string projectionLoad(string fname="",string iname="");
	//��ӡ���ķ����������Ϣ�����ڳ���ȫ�Ժ��ȥ
	void projectionView(int mode=NUMTABLE);
	//��Ԥ�������������ļ�
	bool outPredictTableToFile(string fileout);
	//��ӳ�����ʽ������ļ�
	bool outProductionToFile(string fileout);

	IniFile ini;

	int findPos(const string& s,bool mode=true) const;
};