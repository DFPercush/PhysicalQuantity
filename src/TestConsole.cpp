
//#define NO_INLINE

#include <PhysicalQuantity.h>
#include <stdio.h>

#ifdef NO_TEXT
int main()
{
	printf(" *** testconsole: Compiled with NO_TEXT. Console not available.\n");
	return 0;
}
#else //NO_TEXT

#include <iostream>
#include <string>
#include <string.h>
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
	"/?       this help\n" \
	"Recommended for most cases: pq -i -s\n" \
	"Example: 12 mi / 10 min, *m hr \n" \
	"    ---> 115.87276799950321 km / hr\n\n"
	);
}

void runEval(const csubstr& line)
{
	PQ x = 1_C;
	PQ y = 1_N;
}

void runConvert(const char* s)
{
}

#ifdef NO_THROW
PQ::ErrorCode ecode;
#endif

char buf[1000];

void runLine(csubstr &line, bool useConvert, bool useSprint)
{
	PQ val;
	csubstr units;
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
		val = PQ::eval(evalexpr);
		if (useConvert)
		{
			double cval = val.convert(units);
			printf("%g\n", cval);
		}
		else if (useSprint)
		{
			val.sprint(buf, 1000, 15, units);
			printf("%s\n", buf);
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

	bool interactive = false;
	bool useConvert = false;
	bool useSprint = false;
	int lastOptionArg = 0;

	if (argc <= 1)
	{
		showHelp();
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
	printf("expression [ , preferred output units ] | 'sprint' | 'convert' | 'quit' | 'exit'\n");
	string line_std_string;
	while (true)
	{
		if (useConvert) { printf("\nconvert> "); }
		else if (useSprint) { printf("sprint> "); }
		else { printf(">"); }

		getline(cin, line_std_string);
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
#endif // !NO_TEXT

