
//#define NO_INLINE

#include <stdio.h>
#include <iostream>
#include <PhysicalQuantity.h>
using namespace std;
typedef PhysicalQuantity PQ;
typedef PQ::CSubString csubstr;

void showHelp()
{
	printf("Options:\n" \
	"-c use convert() and print the numeric result\n" \
	"-s use sprint() and print the text result\n" \
	"-i interactive mode \n" \
	"--help\n" \
	"-?\n" \
	"/?       this help\n");
}

void runEval(const csubstr& line)
{
}

void runConvert(const char* s)
{
}

int main(int argc, char** argv)
{
	if (!PQHeaderOptionsMatch) 
	{
		printf("Error: library code was compiled with different header options.\n");
		return 1;
	}

	if (argc <= 1)
	{
		showHelp();
		return 0;
	}

	bool interactive = false;
	bool useConvert = false;
	bool useSprint = false;
	int lastOptionArg = 0;
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

	if (!interactive) { return 0; }
	if (!useSprint && !useConvert)
	{
		printf("Warning: You are in interactive mode without setting any output method.\n");
	}
	printf("expression [ , unitsOut ] | 'sprint' | 'convert' | 'quit' | 'exit'\n");
	string line_std_string;
	csubstr line;
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

		PQ val;
		csubstr units;
		int commapos = line.find_first_of(',');
		char buf[1000];

		if (commapos == -1) 
		{
			units = "";
		}
		else 
		{
			units = line.substr(commapos + 1);
		}
		csubstr evalexpr = line.substr(0, commapos);
		try
		{
			val = PQ::eval(evalexpr);
			if (useConvert)
			{
				double cval = val.convert(units);
				printf("%g\n", cval);
			}
			else if (useSprint)
			{
				val.sprint(buf, 1000, units);
				printf("%s\n", buf);
			}
			else
			{
				printf("(parsed ok)\n>");
			}
		}
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
		catch (const exception& err)
		{
			printf("General error: %s\n", err.what());
		}
	}
	return 0;
}

