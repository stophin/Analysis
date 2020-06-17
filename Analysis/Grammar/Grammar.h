#pragma once
#include "Set.h"
#include "Precept.h"
#include "StringLink.h"

class Grammar
{
public:
	Grammar(void);
	~Grammar(void);
	Grammar(const Grammar & g);
	const Grammar operator = (const Grammar & g);
	void SetVt(StringLink vt);
	void SetVn(StringLink vn);
	void SetStart(string start);
	void AddPrecept(StringLink p);
	bool GeneratePredictTable();

	bool IsGrammarLegal();
	//string OutputHTML();
	//void Output(char * pFilename);
	void Output();
	bool IsInVn(string cChar);
	bool IsInVt(string cChar);
	string GetStart();
	Precept GetToDo(string vn, string vt);
	bool IsLegalLL1Grammar();
//private:
	enum CanEmpty { CANTRUE, CANFALSE, UNDEFINED };

	Set Vt;
	Set Vn;
	string cStart;
	vector <Precept> P;
	CanEmpty * pCanEmptyTable;
	int * pPredictTable;
	vector <Set> FirstSet;
	vector <Set> FollowSet;
	vector <Set> SelectSet;
	int nVt;
	int nVn;
	int nP;
	int iIsLL1;

	void MakeCanReachEmptyTable();
	void CalculateFirstSet();
	void CalculateFollowSet();
	void CalculateSelectSet();
	
	void MakePredictTable();

	bool CanReachEmpty(StringLink str);
	bool CanReachEmpty(string cChar);
	Set GetFirstSet(string cChar);
	Set GetFirstSet(StringLink str);
	Set GetFollowSet(string cChar);

};
