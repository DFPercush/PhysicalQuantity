#include <csubstr.h>
#include <string.h>

CSubString::CSubString(const char* str_arg, int start_arg, int len_arg)
{
	str = str_arg;
	start = (int)start_arg;
	if (len_arg == -1)
	{
		end = (int)strlen(str_arg);
	}
	else
	{
		end = (int)(start + len_arg);
	}
}

bool CSubString::operator== (const char* cmp) const
{
	if ((end - start == 0) && cmp[0] != 0) { return false; }
	for (int i = start; (i < end) && (*cmp != 0) && (str[i] != 0); i++)
	{
		if (str[i] != *cmp) { return false; }
		if (str[i] == 0) { return true; }
		if (*cmp == 0) { return false; }
		cmp++;
	}
	return true;
}

bool CSubString::operator==(const CSubString& cmp) const
{
	if (length() != cmp.length()) { return false; }
	int iMe = start;
	int iCmp = cmp.start;
	while (iMe < end && iCmp < cmp.end)
	{
		if (str[start + iMe] != cmp[cmp.start + iCmp]) { return false; }
		iMe++;
		iCmp++;
	}
	return true;
}

bool CSubString::ends(const CSubString& test) const
{
	int iTest = 0;
	for (int i = start; i < end; i++)
	{
		if (str[i] != test[iTest]) { return false; }
		if (test[iTest] == 0) { return false; }
		iTest++;
	}
	return true;
}
bool CSubString::begins(const CSubString& test) const
{
	int iTest = 0;
	int tlen = test.length(); // (int)strlen(test);
	if (tlen < (end - start)) { return false; }
	int ti = tlen - (end - start);
	for (int i = start; i < end; i++)
	{
		if (str[i] != test[ti]) { return false; }
		if (str[i] == 0) { return true; }
		ti++;
	}
	return true;
}

bool CSubString::copy(char* buf, int size) const
{
	int len = end - start;
	if (size <= len) { return false; }
	memcpy(buf, &str[start], len);
	buf[len] = 0;
	return true;
}

int CSubString::at(const char* test, int startArg) const
{
	bool match;
	int mylen = end - start;
	for (int iStart = startArg; test[iStart] != 0; iStart++)
	{
		if (test[iStart] == 0) { return -1; }
		if (test[iStart] == str[start])
		{
			match = true;
			for (int i = 0; i < mylen; i++)
			{
				if (test[iStart + i] != str[start + i])
				{
					match = false;
					break;
				}
				if (str[start + i] == 0) { return iStart; }
				if (test[iStart + i] == 0) { return -1; }
			}
			if (match) { return iStart; }
		}
	}
	return -1;
}

CSubString& CSubString::operator=(const CSubString& cp)
{
	str = cp.str;
	start = cp.start;
	end = cp.end;
	return *this;
}

CSubString::CSubString()
{
	str = nullptr;
	start = 0;
	end = 0;
}

int CSubString::find_first_of(const CSubString& find, int startOfs) const
{
	for (int iMe = start + startOfs; iMe < end; iMe++)
	{
		for (int iFind = 0; find[iFind]; iFind++)
		{
			if (str[iMe] == find[iFind]) { return iMe - start; }
		}
	}
	return -1;
}

int CSubString::find_first_not_of(const CSubString& find, int startOfs) const
{
	int ret = -1;
	bool found;
	for (int iMe = start + startOfs; iMe < end; iMe++)
	{
		found = false;
		for (int iFind = 0; find[iFind]; iFind++)
		{
			if (str[iMe] == find[iFind]) 
			{
				found = true;
				break; 
			}
		}
		if (!found) { return iMe - start; }
	}
	return -1;
}

int CSubString::find_first_of(char c) const
{
	for (int iMe = start; iMe < end; iMe++)
	{
		if (str[iMe] == c) { return iMe - start; }
	}
	return -1;
}

int CSubString::find_first_not_of(char c) const
{
	for (int iMe = start; iMe < end; iMe++)
	{
		if (str[iMe] != c) { return iMe - start; }
	}
	return -1;
}

CSubString::CSubString(const CSubString& from, int start_arg, int len_arg)
{
	str = from.str;
	start = from.start + (int)start_arg;
	if ((len_arg == -1) || (start + len_arg > (from.end)))
	{
		end = from.end;
	}
	else
	{
		end = start + (int)len_arg;
	}
}

double CSubString::atof()
{
	// If the source string is in ROM you can't write a temporary
	// null terminator, so I guess I'll have to implement it myself.
	double ret = 0.0;
	int i;
	char c = 0;
	bool neg = false;
	bool haveSeenFirst = false;
	bool pastDecimal = false;
	bool pastExp = false;
	bool expNeg = false;
	int decimalPlace = 0;
	int exp = 0;
	double digitMul = 1.0;
	char prev;
	for (i = start; i < end; i++)
	{
		prev = c;
		c = str[i];
		if (c == ' ' || c == '\t')
		{
			if (haveSeenFirst) { break; }
		}
		else if (pastExp)
		{
			exp = substr(i - start).atoi();
			break;
		}
		else if (c == '.')
		{
			pastDecimal = true;
		}
		else if (c >= '0' && c <= '9')
		{
			if (pastDecimal)
			{
				digitMul /= 10.0;
				ret += ((c - '0') * digitMul);
			}
			else
			{ 
				ret *= 10;
				ret += (c - '0');
			}
			haveSeenFirst = true;
		}
		else if (c == '-')
		{
			if (haveSeenFirst) { break; }
			else 
			{
				if (pastExp) { expNeg = true; }

				neg = true; 
			}
			haveSeenFirst = true;
		}
		else if (c == '+')
		{
		}
		else if (c == 'e' || c == 'E')
		{
			pastExp = true;
		}
		else { break; }
	}

	// TODO: Is there a faster way of directly setting the exponent?
	// Might involve logarithmic base conversion, not to mention endianness, bleh
	while (exp > 0)
	{
		if (exp >= 100) 
		{
			ret *= 1e100;
			exp -= 100;
		}
		else if (exp >= 10)
		{
			ret *= 1e10;
			exp -= 10;
		}
		else
		{
			ret *= 10;
			exp -= 1;
		}
	}
	while (exp < 0)
	{
		if (exp <= -100)
		{
			ret /= 1e100;
			exp += 100;
		}
		else if (exp <= -10)
		{
			ret /= 1e10;
			exp += 10;
		}
		else
		{
			ret /= 10;
			exp += 1;
		}
	}
	if (neg) { ret *= -1; }
	return ret;
}

int CSubString::atoi()
{
	int ret = 0;
	int i;
	char c;
	bool neg = false;
	bool haveSeenFirst = false;
	for (i = start; i < end; i++)
	{
		c = str[i];
		if (c == ' ')
		{
			if (haveSeenFirst) { break; }
		}
		else if (c >= '0' && c <= '9')
		{
			ret *= 10;
			ret += (c - '0');
			haveSeenFirst = true;
		}
		else if (c == '-')
		{
			if (haveSeenFirst) { break; }
			else { neg = true; }
			haveSeenFirst = true;
		}
		else if (c == '+')
		{
			if (haveSeenFirst) { break; }
			haveSeenFirst = true;
		}
		else { break; }
	}
	if (neg) { ret *= -1; }
	return ret;
}

CSubString CSubString::substr(int startArg, int lenArg)
{
	return CSubString(*this, startArg, lenArg);
}


// Separate CSubString for later maybe
#ifdef NO_INLINE
char CSubString::operator[](int index) const { if (start + index >= end) { return 0; } return str[start + index];}
bool CSubString::begins(const char* test) const { return begins(CSubString(test)); }
bool CSubString::ends(const char* test) const { return ends(CSubString(test)); }
int CSubString::size() const { return end - start; }
int CSubString::length() const { return (end - start) >= 0 ? (end-start) : 0 ; }
int CSubString::find_first_of(const char* find, int startOfs) const { return find_first_of(CSubString(find), startOfs); }
int CSubString::find_first_not_of(const char* find, int startOfs) const { return find_first_not_of(CSubString(find), startOfs); }
#endif //#ifdef NO_INLINE
