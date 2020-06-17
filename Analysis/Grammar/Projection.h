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

//定义不同的文法产生表的显示方式
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
	bool isLoaded;				//标记是否已经加载，防止重复加载
	string filename;			//文法文件
	string ininame;				//与该文法关联的词典ini文件
	int grammarLength;			//文法个数
	StringLink terminal;		//终结符
	StringLink nonterminal;		//非终结符
	StringLink *productTable;	//文法表
	string start;				//文法开始符
	int *predictTable;			//预测分析表	
	int tableLength;			//预测分析表的长度
	int tableHeight;			//预测分析表的高度
	int tableWidth;				//预测分析表的宽度
	string lastError;			//最后一次错误
private:
	bool createPredictTable();		//获取预测表
	void clearMemory();				//清空分配的控件
	void initObject(bool all=true);	//初始化对象所有内容
	int isSeparate(const char* s);	//返回分隔符类型
public:	
	//加载映射，如果构造时未传递任何文件信息参数，则不调用此函数
	//如果调用任何一个含参构造函数，由于第一个参数必定是文法表文件信息，
	//则此时可以调用此函数加载文法表
	//外部调用只针对无参构造函数之后的后续调用，有参构造之后将不再响应该操作
	string projectionLoad(string fname="",string iname="");
	//打印该文法表的所有信息，将在程序健全以后除去
	void projectionView(int mode=NUMTABLE);
	//将预测分析表输出到文件
	bool outPredictTableToFile(string fileout);
	//将映射产生式输出到文件
	bool outProductionToFile(string fileout);

	IniFile ini;

	int findPos(const string& s,bool mode=true) const;
};