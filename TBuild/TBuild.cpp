#include <iostream>
#include <string>
#include "..\TreeCompiler\TreeCompiler.h"

using namespace std;

int main(int argc, char* args[])
{
	if (argc == 3)
	{
		try
		{
			TreeCompiler::CompileTree(string(args[1]), string(args[2]));
		}
		catch (string s)
		{
			cout << s;
		}
	}
	else
	{
		cout << "Usage: plyInput kdOutput";
	}
}

