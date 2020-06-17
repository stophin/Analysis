// WordLinkList.h
//

#pragma once

#include "../DataStructure/MultiLinkList.h"

typedef class WordElement{
public:
	WordElement(const char * str){
		int i;
		for (i = 0; str[i] != '\0'; i++);
		this->word = (char *)malloc((i + 1) * sizeof(char));
		memcpy(this->word, str, i);
		this->word[i] = '\0';
	}
	~WordElement() {

	}

	void final() {
		//printf("\"%s\" was deleted\n", this->str);
		delete this->word;
	}

	char * word;
}WordElement;


typedef class WordContainer : public MultiLinkListBase<WordContainer> {
public:
	WordContainer(WordElement * ref) : MultiLinkListBase<WordContainer>(5),
		ref(ref) {
	}
	~WordContainer() {

	}

	WordElement * ref;

	WordContainer * context_prev;
	WordContainer * context_next;

	void final() {
		//printf("\"%s\" was deleted\n", this->str);
	}

}WordContainer;

typedef class WordList : public MultiLinkList<WordContainer> {
public :
	WordList(int index) : MultiLinkList<WordContainer>(index){

	}
	~WordList() {

	}

	WordContainer * getWord(const char * str) {
		WordContainer * elem = this->link;
		if (elem == NULL) {
			return NULL;
		}
		do {
			if (strcmp(elem->ref->word, str) == 0) {
				return elem;
			}
			elem = this->next(elem);
		} while (elem && elem != this->link);
		return NULL;
	}
}WordList;

typedef class  WordManage : public MultiLinkListBase<WordManage> {
public :
	WordManage(const char * str) : MultiLinkListBase<WordManage>(2),
		context(0){
		elem = new WordElement(str);
	}
	~WordManage() {

	}

	void final() {
		delete this->elem;
	}
	WordElement * elem;
	WordList context;
}WordManage;

typedef class ManageLink : public MultiLinkList<WordManage> {
public:

	ManageLink(int index) : MultiLinkList<WordManage>(index){

	}
	~ManageLink() {

	}
	WordManage * getWord(const char * str) {
		WordManage * man = this->link;
		if (man == NULL) {
			return NULL;
		}
		do {
			if (strcmp(man->elem->word, str) == 0) {
				return man;
			}
			man = this->next(man);
		} while (man && man != this->link);
		return NULL;
	}
}ManageLink;