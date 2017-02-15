#pragma once

#include "..\MathLib\MathLib.h"
#include "..\Tree\Tree.h"
#include "..\Tree\TreeLoader.h"
#include "..\Canvas\Canvas.h"
#include "..\Camera\Camera.h"
#include "..\PLYLoader\PLYLoader.h"
#include "..\SceneLoader\SceneLoader.h"
#include "..\RayBundleGenerator\RayBundleGenerator.h"
#include <string>

using namespace std;
using namespace Tree;
using namespace CanvasContainer;
using namespace CameraLib;
using namespace PLYLoader;
using namespace RayGeneration;

namespace Engine
{
	class RenderEngine
	{
	public:
		RenderEngine(int nbThreads);
		void RenderThreadStart();
		void Render();

	private:
		HANDLE frameStart, frameEnd, workerSem;
		std::vector<HANDLE> threadHandles;
		void ThreadWork();
		void WaitForWorkers();
		Canvas* c;
		bool RUNNING;
		int threadCount;

		TreeBranch* tree;
		Box* boundaries;
		vector<Triangle*> triangles;

		RayBundleGenerator* generator;
	};
}