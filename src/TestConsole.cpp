#include <PhysicalQuantity.h>
#include <stdio.h>

#if defined(NO_TEXT) || defined(NO_NEW) || defined(NO_STD_STRING) || defined(NO_STD_MAP)
int main()
{
	printf(" *** Test Console is not available due to the following #defines:\n"
#ifdef NO_NEW	
"	NO_NEW\n"
#endif
#ifdef NO_TEXT
"	NO_TEXT\n"
#endif
#ifdef NO_STD_STRING
"	NO_STD_STRING\n"
#endif
#ifdef NO_STD_MAP
"	NO_STD_MAP\n"
#endif
	"\n This was a library-only build.\n"
	);
	return 0;
}

#else // Master enable #defines


#include <iostream>
#include <fstream>
#include <exception>
#include <vector>
#include <string>
#include <string.h>

#if !defined(_WIN32)
#include <readline/readline.h>
#include <readline/history.h>
#include <malloc.h>
#endif

using namespace std;
typedef PhysicalQuantity PQ;
typedef PQ::CSubString csubstr;

void showHelp()
{
	printf("Options:\n" \
	"-c use convert() and print the numeric result\n" \
	"   (Units must match exactly)\n" \
	"-s use sprint() and print the text result\n" \
	"   (Output units are a suggestion)\n" \
	"-i interactive mode \n" \
	"--help\n" \
	"-?\n" \
	"/?       this help\n\n" \
	"Recommended for most cases: pq -i -s\n" \
	"\n"
	"Example: 12 mi / 10 min, *m hr \n" \
	"    ---> 115.87276799950321 km / hr\n" \
	"\n"
	"Interactive commands:\n"
	"\n"
	"sprint\n"
	"    Switches to (string) print mode, non strict output units\n"
	"convert\n"
	"    Switches to convert mode, output units must be an exact match\n"
	"\n"
	"pref [space-separated unit list]\n"
	"    sets your preferred output units so you don't have to\n"
	"    type them every time\n"
	"pref+ [space-separated unit list]\n"
	"    adds units to preferred list\n"
	"pref- [space-separated unit list]\n"
	"    removes units from the preferred list\n"
	"['slash' | 'noslash']\n"
	"    to switch between the divide sign or negative exponents\n"
	"longnames ['on'|'off']\n"
	"    switch output to use long unit names instead of symbols\n"
	"['whatis'] expression\n"
	"    Show all the units which match the result exactly\n"
	"\n"
	"[variable = ] expression\n"
	"    creates a variable that you can use in expressions later\n"
	"save [comma-separated variable list]\n"
	"    saves variable(s) to file system\n"
#ifdef __linux__
	"    (home directory)\n"
#elif _WIN32
	"    (AppData\\Roaming)\n"
#else
	"    (current directory)\n"
#endif
	"save-all\n"
	"    saves all variables currently in memory\n"
	"vars\n"
	"    lists all variables and their saved status\n"
	"rm [-s] [comma-separated variable list]\n"
	"    removes/deletes a variable from current memory,\n"
	"    and if the -s option is given, also from the save file.\n"
	"    Without -s, the variables will be loaded again when the\n"
	"    program starts or you issue a 'reload' command.\n"
	"reload\n"
	"    reloads the save file and updates vars in memory.\n"
	"    This does not delete or clear vars, but it will overwrite\n"
	"    any vars by the same name with the value from file system.\n"
	"clear [-s]\n"
	"    Clears (deletes) all variables.\n"
	"    If the -s option is given, clears the save file as well.\n"
	"\n"
	"help\n"
	"    This help\n"
	"'exit' | 'quit'\n"
	"    Exits the program\n"
	"\n"
	);
}

void condenseSpace(string& s)
{
	size_t sp2;
	while ((sp2 = s.find("  ")) != string::npos)
	{
		s = s.substr(0, sp2) + s.substr(sp2 + 1);
	}
}

#ifdef NO_THROW
PQ::ErrorCode ecode;
#endif

char buf[1000];
bool useSlash = true;
std::string pustr; // preferred units

PQVarList vars;
PQVarList savedVars;

#ifdef __linux__
string saveFilePathName = string(getenv("HOME")) + "/pq-savedvars.txt";
#elif _WIN32
#pragma warning(disable:4996)
string saveFilePathName = string(getenv("APPDATA")) + "\\pq-savedvars.txt";
#else
string saveFilePathName = "./pq-savedvars.txt";
#endif

int saveVars()
{
	ofstream svf;
	svf.open(saveFilePathName);
	if (!svf.is_open())
	{
		fprintf(stderr, "Warning: vars file could not be saved.\n");
		return 0;
	}
	int ret = 0;
	for (auto kv : savedVars)
	{
		svf << kv.first << "=" << kv.second.toString() << endl;
		ret++;
	}
	svf.close();
	return ret;
}

void loadVars(bool clear = false)
{
	ifstream svf;
	svf.open(saveFilePathName);
	if (!svf.is_open())
	{
		fprintf(stderr, "Warning: Could not open saved vars file.\n");
		return;
	}
	if (clear) { savedVars.clear(); }
	string line;
	size_t eq;
	int nErrors = 0;
	while (!svf.eof())
	{
		getline(svf, line);
		eq = line.find("=");
		if (eq == string::npos) { continue; }
		string varname = line.substr(0, eq);
		try
		{
			savedVars.set(varname, PQ(line.substr(eq + 1)));
		}
		catch (const exception& err)
		{
			fprintf(stderr, "Error loading var '%s': %s\n", varname.c_str(), err.what());
			nErrors++;
		}
	}
	svf.close();
	if (nErrors > 0)
	{
		fprintf(stderr, "Warning: %d vars could not be loaded and may be overwritten if you save.\n", nErrors);
	}
}

vector<string> split(string s, string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	string token;
	vector<string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

bool useLongNames = false;

void runLine(csubstr &line, bool useConvert, bool useSprint)
{
	PQ val;
	csubstr units;
	bool whatis = false;

	// Commands
	if (line.substr(0, 6) == "whatis")
	{
		whatis = true;
		line = line.substr(6);
	}
	else if (line.substr(0, 5) == "pref " || (line == "pref"))
	{
		string putmp = line.substr(5).trim().toStdString();
		if (putmp == "")
		{
			printf("%s\n", pustr.c_str());
			return;
		}
		else if (putmp == "clear")
		{
			pustr = "";
			printf("Preferred units cleared.\n");
			return;
		}
		pustr = putmp;
		printf("%s\n", pustr.c_str());
		return;
	}
	else if (line.substr(0, 6) == "pref+ ")
	{
		pustr = line.substr(6).toStdString() + " " + pustr;
		condenseSpace(pustr);
		printf("%s\n", pustr.c_str());
		return;
	}
	else if (line.substr(0, 6) == "pref- ")
	{
		csubstr torm = line.substr(6).trim();
		auto rmpos = pustr.find(torm);
		if (rmpos != string::npos)
		{
			// Make sure it's a whole word
			if ((rmpos == 0 && (int)torm.length() == (int)pustr.length() )
			 || (rmpos == 0 && pustr[torm.length()] == ' ')
			 || (pustr[rmpos - 1] == ' ' && (int)rmpos + (int)torm.length() == (int)pustr.length())
			 || (pustr[rmpos - 1] == ' ' && pustr[rmpos + torm.length()] == ' ')
			)
			{
				pustr = pustr.substr(0, rmpos) + pustr.substr(rmpos + torm.length());
			}
			else { goto prefMinusNotFound; }
			condenseSpace(pustr);
			printf("'%s' removed from preferred units list.\n%s\n", torm.toStdString().c_str(), pustr.c_str());
		}
		else
		{
			prefMinusNotFound:
			printf("'%s' was not in preferred units list anyway.\n%s\n", torm.toStdString().c_str(), pustr.c_str());
		}
		return;
	}
	else if (line.substr(0, 5) == "slash")
	{
		useSlash = true;
		printf("Set slash = true\n");
		return;
	}
	else if (line.substr(0, 7) == "noslash")
	{
		useSlash = false;
		printf("Set slash = false\n");
		return;
	}
	else if (line == "save-all")
	{
		int saveOldCount = 0;
		int saveNewCount = 0;
		int currentCount = 0;
		for (auto kv : vars)
		{
			if (savedVars.contains(kv.first))
			{
				if (savedVars[kv.first] != kv.second)
				{
					saveOldCount++;
					savedVars.set(kv.first, kv.second);
				}
				else
				{
					currentCount++;
				}
			}
			else
			{
				saveNewCount++;
				savedVars.set(kv.first, kv.second);
			}
		}
		printf("Saved %d vars. %d updated, %d new, %d current\n", saveVars(), saveOldCount, saveNewCount, currentCount);
		return;
	}
	if (line == "save")
	{
		printf("Did you mean save-all?\n");
	}
	else if (line.substr(0, 5) == "save ")
	{
		int nNewSaved = 0;
		int nUpdated = 0;
		int nCurrent = 0;
		int nBadName = 0;
		auto varsToSave = split(line.substr(5), ",");
		for (auto varNameStdStr : varsToSave)
		{
			csubstr varname = csubstr(varNameStdStr).trim();
			if (vars.contains(varname))
			{
				if (savedVars.contains(varname))
				{
					if (savedVars.get(varname) == vars.get(varname))
					{
						nCurrent++;
					}
					else
					{
						savedVars.set(varname, vars.get(varname));
						nUpdated++;
					}
				}
				else
				{
					savedVars.set(varname, vars.get(varname));
					nNewSaved++;
				}
			}
			else
			{
				printf("Not found: %s\n", varname.toStdString().c_str());
				nBadName++;
			}
		}
		int nSavedToFile = saveVars();
		printf("Saved %d variables to file. %d new, %d updated, %d current, %d errors\n",
			nSavedToFile, nNewSaved, nUpdated, nCurrent, nBadName);
		return;

		//csubstr svname = line.substr(5);
		//svname = svname.trim();
		//if (vars.contains(svname))
		//{
		//	bool saveAlreadyContainsVar = savedVars.contains(svname);
		//	savedVars.set(svname, vars.get(svname));
		//	if (saveVars() > 0)
		//	{
		//		if (saveAlreadyContainsVar) { printf("Updated var '%s'.\n", svname.toStdString().c_str()); }
		//		else { printf("Saved new var '%s'\n", svname.toStdString().c_str()); }
		//	}
		//	else
		//	{
		//		printf("Error: Save failed!\n");
		//	}
		//}
		//return;
	}
	else if (line == "vars")
	{
		// TODO: sort
		for (auto kv : vars)
		{
			bool insave = savedVars.contains(kv.first);
			bool current = (insave && (savedVars[kv.first] == kv.second));
			printf("%s %s = %s\n",
				current ? " (saved) " : ( insave ? "(changed)" : "(session)"),
				kv.first.c_str(), kv.second.toString((useSlash ? PQ::SPRINT_SLASH : 0)).c_str());
		}
		printf("Total: %d vars in session.\n", (int)vars.size());
		return;
	}
	else if (line.substr(0, 3) == "rm ")
	{
		csubstr rmliststr;
		bool rmsaved = false;
		if (line.substr(0, 6) == "rm -s ")
		{
			rmsaved = true;
			rmliststr = line.substr(6).trim();
		}
		else
		{
			rmliststr = line.substr(3).trim();
		}
		auto tormstrv = split(rmliststr.toStdString(), ",");
		int nRmSaved = 0;
		int nKeptSaved = 0;
		int nRmVars = 0;
		for (auto item : tormstrv)
		{
			string varname = csubstr(item).trim().toStdString();
			int rmThisIter = (int)vars.erase(varname);
			nRmVars += rmThisIter;
			if (rmsaved) { nRmSaved += (int)savedVars.erase(varname); }
			else if ((rmThisIter > 0) && savedVars.contains(varname)) { nKeptSaved++; }
		}
		printf(" %d session vars and %d saved vars were deleted. %d were unloaded but still in save file.\n", nRmVars, nRmSaved, nKeptSaved);
		if ((nKeptSaved != 0) && (!rmsaved))
		{
			printf("(Use 'rm -s' to delete the saved copy as well.)\n");
		}
		saveVars();
		return;
	}
	else if (line == "reload")
	{
		loadVars();
		int before = (int)vars.size();
		for (auto item : savedVars)
		{
			//vars.insert_or_assign(item.first, item.second);
			vars[item.first] = item.second;
		}
		printf("Reloaded %d vars, %d were not present\n", (int)savedVars.size(), (int)vars.size() - before);
		return;
	}
	else if (line == "clear")
	{
		int nVarsCleared = (int)vars.size();
		vars.clear();
		printf(" %d vars cleared. (Use -s to clear save file as well.)\n", nVarsCleared);
		return;
	}
	else if (line == "clear -s")
	{
		int nVarsCleared = (int)vars.size();
		vars.clear();
		int nSavesCleared = (int)savedVars.size();
		savedVars.clear();
		saveVars();
		printf(" %d session vars and %d saved vars were cleared.\n", nVarsCleared, nSavesCleared);
		return;
	}
	else if (line == "longnames")
	{
#ifdef NO_LONG_NAMES
		printf(" Command unavailable, compiled with NO_LONG_NAMES\n");
#else
		printf(" Long names set to %s\n", (useLongNames ? "ON" : "OFF"));
#endif
		return;
	}
	else if (line.substr(0, 10) == "longnames ")
	{
#ifdef NO_LONG_NAMES
	printf(" Command unavailable, compiled with NO_LONG_NAMES\n");
#else

		string aftersp = line.substr(10).trim();
		if (aftersp == "on" || aftersp == "ON" || aftersp == "On") { useLongNames = true; }
		else if (aftersp == "off" || aftersp == "OFF" || aftersp == "Off") { useLongNames = false; }
		else
		{
			printf("'longnames' expects 'on' or 'off'");
			return;
		}
		printf(" Long names set to %s\n", (useLongNames ? "ON" : "OFF"));
#endif
		return;
	}
	// End commands

	//=================================================================================
	// Variable assignment
	const char nullstr[] = {0};
	csubstr varname(nullstr,0,0);
	int eq = line.find_first_of('=');
	if (eq != -1)
	{
		varname = line.substr(0, eq).trim();
		if (varname.find_first_of(' ') == -1)
		{
			line = line.substr(eq + 1);
		}
		else
		{
			varname.clear();
		}
		if (!PQVarList::isLegalName(varname))
		{
			printf("Warning: Invalid variable name.\n");
			varname.clear();
		}
	}
	// End variable name scanning, will be assigned later

	//====================================================================
	// Evaluation
	int commapos = line.find_first_of(',');

	if (commapos == -1)
	{
		units = "";
	}
	else
	{
		units = line.substr(commapos + 1);
	}
	csubstr evalexpr = line.substr(0, commapos);

#ifndef NO_THROW
	try
#else
	PQ::SetErrorHandler(
		[](void* ctx, PQ::ErrorCode err)
		{
			ecode = err;
		});
#endif // #ifndef NO_THROW
	{
		val = PQ::eval(evalexpr, vars);
		if (varname.length() > 0)
		{
			vars.set(varname, val);
		}

		if (useConvert)
		{
			double cval = val.convert(units);
			printf("%g\n", cval);
		}
		else if (useSprint)
		{
			//val.sprint(buf, 1000, 15, (pustr + " " + units.toStdString()).c_str(), useSlash);
			val.sprint(buf, 1000, 15, (units.toStdString() + " " + pustr).c_str(),
				(useSlash ? PQ::SPRINT_SLASH : 0) | (useLongNames ? PQ::SPRINT_LONG_NAMES : 0));
			printf("%s\n", buf);
			if (whatis)
			{
				signed char d[(int)PQ::QuantityType::ENUM_MAX];
				val.getDim(d, sizeof(d));
				printf("Mass^%d Distance^%d Time^%d Temperature^%d Current^%d\n", d[0], d[1], d[2], d[3], d[4]);

#ifdef NO_LONG_NAMES
				bool comma = false;
#endif
				for (int i = 0; i < PQ::KnownUnitsLength; i++)
				{
					if (!memcmp(d, PQ::KnownUnits[i].dim, sizeof(d)))
					{
#ifndef NO_LONG_NAMES
						printf("%s, %s, %s\n", PQ::KnownUnits[i].symbol, PQ::KnownUnits[i].longName, PQ::KnownUnits[i].plural);
#else
						printf("%s%s", (comma ? ", " : ""), PQ::KnownUnits[i].symbol);
						comma = true;
#endif
					}
				}
#ifdef NO_LONG_NAMES
				printf("\n");
#endif
				printf("[");
				for (int di = 0; di < PQ::ND; di++)
				{
					if (di != 0) printf(",");
					printf("%d", (int)d[di]);
				}
				printf("]\n");

				fflush(stdout);
			}
		}
		else
		{
			printf("(parsed ok)\n>");
		}
	}
#ifndef NO_THROW
	catch (const PQ::InvalidExpressionException& err)
	{
		printf("Invalid expression error: %s\n", err.what());
	}
	catch (const PQ::UnitMismatchException& err)
	{
		printf("Unit mismatch error: %s\n", err.what());
	}
	catch (const logic_error& err)
	{
		printf("Logic error: %s\n", err.what());
	}
	catch (const overflow_error& err)
	{
		printf("Overflow error: %s\n", err.what());
	}
	catch (const exception& err) // runtime_error ?
	{
		printf("General error: %s\n", err.what());
	}
#else
	switch (ecode)
	{
	case PQ::E_SUCCESS:
		break;
	case PQ::E_HEADER_CONFIG:
		printf("E_HEADER_CONFIG\n");
		break;
	case PQ::E_INVALID_EXPRESSION:
		printf("E_INVALID_EXPRESSION\n");
		break;
	case PQ::E_LOGIC_ERROR:
		printf("E_LOGIC_ERROR\n");
		break;
	case PQ::E_MEMORY:
		printf("E_MEMORY\n");
		break;
	case PQ::E_OVERFLOW:
		printf("E_OVERFLOW\n");
		break;
	case PQ::E_UNIT_MISMATCH:
		printf("E_UNIT_MISMATCH\n");
		break;
	default:
		printf("Unknown error\n");
	}
	ecode = PQ::E_SUCCESS;
#endif  // #ifndef NO_THROW
}

int main(int argc, char** argv)
{
	if (!InitLibPQ())
	{
		printf("Error: InitLibPQ() failed.\n");
		return 1;
	}
	if (!PQHeaderOptionsMatch) 
	{
		printf("Error: library code was compiled with different header options.\n");
		return 1;
	}

	loadVars(true);
	vars = savedVars;

	bool interactive = false;
	bool useConvert = false;
	bool useSprint = false;
	int lastOptionArg = 0;

	if (argc <= 1)
	{
		//showHelp();
		printf("Physical Quantity calculator by DFPercush.\n"
			"https://github.com/DFPercush/PhysicalQuantity \n"
			"Type a calculation, 'help' for a guide, or 'quit' to exit this program.\n");
		interactive = true;
		useSprint = true;
	}

	for (int iArg = 1; iArg < argc; iArg++)
	{
		if (!strcmp(argv[iArg], "test"))
		{
			//RunTestSuite();
			lastOptionArg = iArg;
		}
		else if (!strcmp(argv[iArg], "-i"))
		{
			interactive = true; 
		}
		else if (!strcmp(argv[iArg], "-c")) 
		{
			lastOptionArg = iArg;
			if (useSprint) 
			{
				printf("Error: -s and -c are mutually exclusive.\n"); 
				return 1;
			}
			useConvert = true; 
		}			
		else if (!strcmp(argv[iArg], "-s")) 
		{
			lastOptionArg = iArg;
			if (useConvert)
			{
				printf("Error: -s and -c are mutually exclusive.\n");
				return 1;
			}
			useSprint = true; 
		} 
		else if (!strcmp(argv[iArg], "--help") || !strcmp(argv[iArg], "-?") || !strcmp(argv[iArg], "/?"))
		{
			lastOptionArg = iArg;
			showHelp(); 
		} 
		else
		{
			break;
			//printf("Unknown option: %s", argv[iArg]);
			//return 1;
		}
	}

	string arg;
	for (int i = lastOptionArg; i < argc; i++)
	{
		if (i != lastOptionArg) arg += " ";
		arg += argv[i];
	}
	csubstr line;
	if (!interactive)
	{
		line = arg.c_str();
		runLine(line, useConvert, useSprint);
		return 0; 
	}

	//=================================================================
	// Begin interactive mode
	if (!useSprint && !useConvert)
	{
		printf("Warning: You are in interactive mode without setting any output method.\n");
	}
	printf("expression [ , preferred output units ] | 'help'\n");
	string line_std_string;
	char* line_cstr_malloc = nullptr;
	while (true)
	{
		if (useConvert) { printf("\nPQ::convert> "); }
		else if (useSprint) { printf("PQ::sprint> "); }
		else { printf(">"); }

#if defined _WIN32
		getline(cin, line_std_string);
#else
		// GNU readline for editing capability
		line_cstr_malloc = readline("");
		line_std_string = line_cstr_malloc;
		free(line_cstr_malloc);
#endif
		line = line_std_string.c_str();
		line = line.trim();
		if (line_std_string == "quit" || line_std_string == "exit") { break; }
		else if (line_std_string == "help") { showHelp(); continue; }
		else if (line_std_string == "sprint")
		{
			useSprint = true;
			useConvert = false;
			continue;
		}
		else if (line_std_string == "convert")
		{
			useConvert = true;
			useSprint = false;
			continue;
		}

		runLine(line, useConvert, useSprint);
	}
	return 0;
}
#endif // Master enable #defines

