#pragma once

#include "..\MathLib\MathLib.h"
#include "Tree.h"

using namespace GeometryLib;
using namespace std;

namespace TreeCompiler
{
	void CompileTree(string plyPath, string outputPath);
	void CompileTree(vector<Triangle*>& triangles, string outputPath);
}