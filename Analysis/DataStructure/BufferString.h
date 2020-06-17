// BufferString.h
//

#pragma once

#include <string.h>

#include "MultiLinkList.h"

class BufferString : public MultiLinkListBase<BufferString> {
public:
	BufferString(const char * str) :
		MultiLinkListBase<BufferString>(10) {
			int i;
			for (i = 0; str[i] != '\0'; i++);
			this->str = (char *)malloc((i + 1) * sizeof(char));
			memcpy(this->str, str, i);
			this->str[i] = '\0';
		}

	BufferString(char str) :
		MultiLinkListBase<BufferString>(2) {
			int i = 1;
			this->str = (char *)malloc((i + 1) * sizeof(char));
			this->str[0] = str;
			this->str[i] = '\0';
		}

	BufferString(char str1, char str2) :
		MultiLinkListBase<BufferString>(2) {
			int i = 2;
			this->str = (char *)malloc((i + 1) * sizeof(char));
			this->str[0] = str1;
			this->str[1] = str2;
			this->str[i] = '\0';
		}
	~BufferString() {
	}

	void final() {
		//printf("\"%s\" was deleted\n", this->str);
		delete this->str;
	}

	char * str;
};