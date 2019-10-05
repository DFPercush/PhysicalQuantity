#pragma once

// Preprocessor options:
//#define NO_INLINE        // Do not use inline functions, make all functions normal calls.

#include "PhysicalQuantity/ppOptions.h"

#ifndef INLINE_KEYWORD
#define INLINE_KEYWORD __inline
#endif

class CSubString
{
private:
	const char* str;
	int start;
	int end; // length in constructor, internally end is easier
public:
	//typedef double num; //Duplicate typedef, meh

	CSubString();
	CSubString(const char* str_arg, int start_arg = 0, int len_arg = -1);
	CSubString(const CSubString& from, int start_arg = 0, int len_arg = -1);
	CSubString& operator=(const CSubString& cp);
	bool operator== (const char* cmp) const;
	bool operator==(const CSubString& cmp) const;
	bool begins(const CSubString& test) const;
	bool ends(const CSubString& test) const;
	int at(const char* test, int start = 0) const; // where this needle appears in a haystack argument, or -1
	bool copy(char* buf, int size) const;
	int find_first_of(const CSubString& find, int startOfs = 0) const;
	int find_first_not_of(const CSubString& find, int startOfs = 0) const;
	int find_first_of(char c, int startOfs = 0) const;
	int find_first_not_of(char c, int startOfs = 0) const;
	int find_last_of(const CSubString& find, int startOfs = -1) const;
	int find_last_not_of(const CSubString& find, int startOfs = -1) const;
	// TODO: const char* inline
	int find_last_of(char c, int startOfs = -1) const;
	int find_last_not_of(char c, int startOfs = -1) const;
	int atoi();
	double atof();
	CSubString substr(int startArg = 0, int lenArg = -1) const;
	bool isnumber() const;
	bool isint() const;
	CSubString trim() const;

#ifdef NO_INLINE
	char operator[](int index) const;
	bool begins(const char* test) const;
	bool ends(const char* test) const;
	int size() const;
	int length() const;
	int find_first_of(const char* find, int startOfs = 0) const;
	int find_first_not_of(const char* find, int startOfs = 0) const;
#else
	INLINE_KEYWORD char operator[](int index) const { if (start + index >= end) { return 0; } return str[start + index];}
	INLINE_KEYWORD bool begins(const char* test) const { return begins(CSubString(test)); }
	INLINE_KEYWORD bool ends(const char* test) const { return ends(CSubString(test)); }
	INLINE_KEYWORD int size() const { return end - start; }
	INLINE_KEYWORD int length() const { return (end - start) >= 0 ? (end-start) : 0 ; }
	INLINE_KEYWORD int find_first_of(const char* find, int startOfs = 0) const { return find_first_of(CSubString(find), startOfs); }
	INLINE_KEYWORD int find_first_not_of(const char* find, int startOfs = 0) const { return find_first_not_of(CSubString(find), startOfs); }
#endif
};
typedef CSubString csubstr;
