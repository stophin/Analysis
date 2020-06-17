// Analysis.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <conio.h>

#include "./DataStructure/StackQueueList.h"
#include "./DataStructure/BufferString.h"

#include "./WordLink/WordLinkList.h"

#include "./Grammar/Projection.h"

typedef StackQueueList<BufferString> BufferStringList;

#include "ICTCLAS30.h"
#pragma comment(lib, "ICTCLAS30.lib") //ICTCLAS30.lib库加入到工程中

// Segmentation using ICTCLAS
void doSegmentation(char * buffer, BufferStringList & list) {

	int i, j;

	int count;
	const result_t * result = ICTCLAS_ParagraphProcessA(buffer, &count);
	char * segment;
	char segment_c;
	for (i = 0; i < count; i++) {
		segment = &buffer[result->start + result->length];
		segment_c = *segment;
		*segment = '\0';
		//printf("%s => ", (buffer + result->start));
		list.add(new BufferString(buffer + result->start));
		*segment = segment_c;
		result++;
	}
	/*
	for (i = 0; i < list.link.linkcount; i++){
		printf("%s -> ", list.get(i)->str);
	}
	printf("\n");
	*/
}

int doAnalyasis(Projection & proj, BufferStringList & list, BufferStringList & command) {
	if (!proj.isLoaded) {
		return 0;
	}
	command.clear();
	BufferStringList stack(3);
	BufferString * sig;
	BufferString * top;
	StringLink predict;

	cout << "语库:" << proj.ininame << endl;
	cout << "属性号\t关键字\t属性\t栈\t\t\t\t\t产生式" << endl;
	int i, j;
	int terpos, nterpos;
	int index;
	stack.push(new BufferString(proj.start.c_str()));
	for (i = 0; i < list.link.linkcount && !stack.empty(); i++) {
		sig = list.get(i);

		if (proj.ini.getInt(sig->str, "MARK") == 0) {
			break;
		}
		if (proj.ini.getAttr(sig->str, "ATTR0") == 0) {
			break;
		}
		cout << proj.ini.aint << "\t" << sig->str << "\t" << proj.ini.attr << "\t";
		for (j = 0; j < stack.link.linkcount; j++) {
			cout << stack.get(i)->str << ">";
		}
		for (; j < 20; j++) {
			cout << " ";
		}
		top = stack.peek();
		
		terpos = proj.findPos(top->str);
		if (terpos != -1) {
			if (strcmp(top->str, proj.ini.attr)) {
				stack.pop();

			}
		}
		terpos = proj.findPos(top->str, false);
		nterpos = proj.findPos(proj.ini.attr);
		if (terpos == -1){
			cout << terpos << "," << nterpos << ", will exit" << endl;
			break;
		}

		index = proj.predictTable[terpos * proj.tableWidth + nterpos];
		if (abs(index) > proj.grammarLength) {
			cout << index << ", will exit" << endl;
			break;
		}
		if (index == -1) {

		}
		else if (index == -2) {
			stack.pop();
		}
		else {
			predict = proj.productTable[index];
			for (j = 0; j<predict.length(); j++)
				cout << predict[j] << (j>2 ? " " : "");
			cout << endl;

			if (predict[2] == "@")
			{
				stack.pop();
				i--;
			}
			else{
				if (predict[2] == proj.terminal[terpos])
				{
					stack.pop();
					for (j = predict.length() - 1; j > 2; j--)
					{
						stack.push(new BufferString(predict[j].c_str()));
					}
				}
				else {

					stack.pop();
					for (j = predict.length() - 1; j >= 2; j--)
					{
						stack.push(new BufferString(predict[j].c_str()));
					}
					i--;
				}
			}
		}


		cout << proj.findPos(top->str) << endl;
	}
	cout << "栈剩余:" << stack.link.linkcount << endl;
	while (!stack.empty()) {
		cout << stack.pop()->str << "->";
	}
	cout << endl;
	stack.clear();

	return 0;
}

// Print the whole context
void testWordLink(ManageLink & words, BufferStringList & list) {
	WordManage * word;
	WordContainer *prev, *next, *context;
	int count, counter;

	word = words.link;
	if (word) {
		count = 0;
		do {
			context = word->context.link;
			printf("word(%d/%d):%s\n", count++, words.linkcount, word->elem->word);
			if (context) {
				counter = 0;
				do {
					printf("context(%d/%d):", counter++, word->context.linkcount);
					prev = context;
					do {
						next = prev;
						prev = prev->context_prev;
					} while (prev && prev != context);
					prev = next;
					if (prev) {
						do {
							printf("%s-", prev->ref->word);
							prev = prev->context_next;
						} while (prev && prev != next);
					}
					printf("\n");

					context = word->context.next(context);
				} while (context && context != word->context.link);
			}

			word = words.next(word);
		} while (word && word != words.link);
	}
}

// Check the whole context is exsiting or not
int subContext(ManageLink & words, BufferStringList & list, WordList & wdlist) {
	BufferString * sig, *seg;
	WordManage * word;
	WordContainer *prev, *next, *context;
	WordList dellist(2);

	wdlist.clearLink();
	sig = list.link.link;
	if (sig) {
		word = words.getWord(sig->str);
		if (word == NULL) {
			return 0;
		}
		context = word->context.link;
		if (context) {
			// First add all the related context header into list
			// And delete the context header if not matched
			do {
				wdlist.insertLink(context);

				context = word->context.next(context);
			} while (context && context != word->context.link);
			context = wdlist.link;
			if (context) {
				// For every context, check match
				do {
					prev = context;
					seg = sig;
					do {
						// Not equal, delete
						if (strcmp(prev->ref->word, seg->str)) {
							dellist.insertLink(prev);
							break;
						}

						prev = prev->context_next;
						seg = list.link.next(seg);

						// Conditions for exiting loop 
						if (!(prev && prev != context && seg && seg != sig)) {
							// If input is more than context, delete
							if (seg && seg != sig) {
								dellist.insertLink(prev);
							}
							break;
						}
					} while (1);

					context = wdlist.next(context);
				} while (context && context != wdlist.link);
			}
		}
	}

	return wdlist.linkcount;
}

// Create word link
void doCreateWordLink(ManageLink & words, BufferStringList & list) {
	BufferString * sig = list.link.link;
	if (sig == NULL) {
		return;
	}
	WordManage * word, *word_next;
	WordContainer *prev, *next, *context;
	WordList wdlist(1);

	// If the input context is already existed, print the context related
	if (subContext(words, list, wdlist)) {
		// The related context link list
		context = wdlist.link;
		if (context) {
			printf("-----------------------------------------\n");
			do {
				// Get to the very start of the context
				prev = context;
				do {
					next = prev;
					prev = prev->context_prev;
				} while (prev && prev != context);
				prev = next;
				// Print each word of the conext
				if (prev) {
					do {
						printf("%s->", prev->ref->word);

						prev = prev->context_next;
					} while (prev && prev != next);
				}
				printf("\n");
				
				// Next context from the context link list
				context = wdlist.next(context);
			} while (context && context != wdlist.link);
			printf("-----------------------------------------\n");
		}
		// Need to clear link
		// Otherwise the word in the list will be destoryed
		// by the deconstructor of the link
		// when this function exited
		// which will destory the word(the same object) in the word link
		wdlist.clearLink();
		return;
	}

	// Find/Create new word header
	word = words.getWord(sig->str);
	if (word == NULL) {
		word = new WordManage(sig->str);
		words.insertLink(word);
	}
	// Create new word container
	prev = new WordContainer(word->elem);
	// Put the container into the current word(header)
	word->context.insertLink(prev);
	// Get the next word(after header) from the input
	sig = list.link.next(sig);
	// Not a single input word
	if (sig && sig != list.link.link) {
		do {
			// Find/Create new word next
			word_next = words.getWord(sig->str);
			if (word_next == NULL) {
				word_next = new WordManage(sig->str);
				words.insertLink(word_next);
			}
			// Create new word next container
			next = new WordContainer(word_next->elem);
			// Put the container into the current word
			word_next->context.insertLink(next);
			// Set context link
			prev->context_next = next;
			next->context_prev = prev;
			// Exchange context
			prev = next;
			word = word_next;

			// Next input word
			sig = list.link.next(sig);
		} while (sig && sig != list.link.link);
	}

	//testWordLink(words, list);

}

int _tmain(int argc, _TCHAR* argv[])
{
	system("color AC");

	cout << "初始化分词组建..." << endl;
	if (!ICTCLAS_Init()) {
		cout << "初始化分词组建失败!" << endl;
		return -1;
	}
	cout << "开始导入用户词典..." << endl;
	int count = ICTCLAS_ImportUserDict("userdict.txt");
	cout << "导入了" << count << "个用户词" << endl;
	//设置词性标注集(
	//0 计算所二级标注集，
	//1 计算所一级标注集，
	//2 北大二级标注集，
	//3 北大一级标注集)
	cout << "设置词性标注集为计算机二级标注" << endl;
	ICTCLAS_SetPOSmap(0);

	cout << "加载并生成文法表" << endl;
	Projection proj;
	cout << proj.projectionLoad("./Product/LL1Product.txt", "./Product/Product.ini") << endl;	//加载第一张文法表	
	//	proj.outProductionToFile("t2out.txt");
	//	cout<<proj.lastError<<endl;
	proj.projectionView(PROTABLE);
	Projection projt1("./Product/LL1Test1.txt", "./Product/Test1.ini");//加载第一张文法表
	cout << projt1.lastError << endl;
	projt1.projectionView();
	Projection projt2("./Product/LL1Test2.txt", "./Product/Test2.ini");//加载第二张文法表
	cout << projt2.lastError << endl;
	projt2.projectionView();
	Projection projt3("./Product/LL1Test3.txt", "./Product/Product.ini");//加载第二张文法表
	cout << projt3.lastError << endl;
	projt3.projectionView(PROTABLE);
	//	projt1.outPredictTableToFile("projout.txt");
	//	projt1.outProductionToFile("proj1out.txt");
	//	cout<<ini<<projt1.lastError<<endl;

	char buffer[100];
	BufferStringList stack(0);
	BufferStringList queue(1);
	BufferStringList list(2);
	BufferStringList command(3);
	BufferString * bufferString;

	ManageLink words(0);

	while (true) {
		printf("<<");
		buffer[0] = '\0';
		scanf_s("%99[^\n]", buffer, 100);
		fflush(stdin);

		if (strcmp(buffer, "exit") == 0) {
			break;
		}
		else if (strcmp(buffer, "hist") == 0) {
			while (!stack.empty()) {
				bufferString = stack.pop();
				printf("%s\n", bufferString->str);
				delete bufferString;
			}
		}
		else if (strcmp(buffer, "list") == 0) {
			while (!queue.empty()) {
				bufferString = queue.dequeue();
				printf("%s\n", bufferString->str);
				delete bufferString;
			}
		}
		else {
			bufferString = new BufferString(buffer);
			stack.push(bufferString);
			queue.inqueue(bufferString);

			doSegmentation(buffer, list);

			//doAnalyasis(proj, list, command);

			doCreateWordLink(words, list);

			list.clear();
		}


	}

	printf("Press any key to exit");

	_getch();

	ICTCLAS_Exit();

	return 0;
}



