// Harness.cpp : Defines the entry point for the console application.
//

#include "..\RenderEngine\RenderEngine.h"

#include <vector>
#include <iostream>
#include "windows.h"

using namespace std;
using namespace Engine;

int main(int argc, char* argv[])
{
	int ts = sizeof(char);
	int branchSize = sizeof(Tree::TreeBranch);
	printf("Tree branch is %i bytes\n", branchSize);	

	HANDLE hProcess = GetCurrentProcess();
	SetPriorityClass(hProcess, BELOW_NORMAL_PRIORITY_CLASS);

	RenderEngine e = RenderEngine(4);
	
	int frameC = 100;
	while (true)
	{
		float startTime = timeGetTime();

		for (int t = 0; t < frameC; t++)
		{
			e.Render();
		}
		float elapsed = ((float) timeGetTime()) - startTime;
		float frameRate = 1000.0f / (elapsed / (float) frameC);

		printf("%f fps\n", frameRate);
		printf("%f elapsed", elapsed);

		float numPixels = 640 * 480 * frameC;
		float mRaySec = (numPixels / 1000000.0f) / (elapsed / 1000.0f);
		printf("%f Mrays/s\n", mRaySec);
	}


	return 0;
}

