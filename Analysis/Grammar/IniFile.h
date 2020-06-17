// IniFile.h
//

#pragma once

#include <Windows.h>

#define MAX_FILELEN 100
#define MAX_ATTRLEN 30

class IniFile {
public:
	IniFile(const char * name) {
		this->loadFile(name);

		this->app[0] = '\0';
		this->key[0] = '\0';
		this->def[0] = '\0';
		this->attr[0] = '\0';
		this->size = MAX_ATTRLEN;
	}

	void loadFile(const char * name) {
		int i;
		for (i = 0; name[i] != '\0' && i < MAX_FILELEN; i++) {
			this->file[i] = name[i];
		}
		this->file[i] = '\0';
	}

	int getInt(char app[], char key[]) {
		int i;
		for (i = 0; app[i] != '\0'; i++);
		memcpy(this->app, app, i + 1);
		for (i = 0; key[i] != '\0'; i++);
		memcpy(this->key, key, i + 1);
		this->aint = 0;
		this->aint = GetPrivateProfileInt(this->app, this->key, 0, this->file);
		return this->aint;
	}

	int getAttr(char app[], char key[]) {
		int i;
		for (i = 0; app[i] != '\0'; i++);
		memcpy(this->app, app, i + 1);
		for (i = 0; key[i] != '\0'; i++);
		memcpy(this->key, key, i + 1);
		this->attr[0] = '\0';
		return GetPrivateProfileString(this->app, this->key, this->def, this->attr, this->size, this->file);

	}
	void setAttr(char app[], char key[], char attr[]) {
		int i;
		for (i = 0; app[i] != '\0'; i++);
		memcpy(this->app, app, i);
		for (i = 0; key[i] != '\0'; i++);
		memcpy(this->key, key, i);
		for (i = 0; attr[i] != '\0'; i++);
		memcpy(this->key, attr, i);
		WritePrivateProfileString(this->app, this->key, this->attr, this->file);
	}

	char file[MAX_FILELEN];
	char app[MAX_ATTRLEN];
	char key[MAX_ATTRLEN];
	char def[MAX_ATTRLEN];
	char attr[MAX_ATTRLEN];
	int aint;
	int size;
};
