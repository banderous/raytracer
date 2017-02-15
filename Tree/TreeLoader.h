#pragma once

#include "..\MathLib\MathLib.h"
#include "..\GeometryLib\GeometryLib.h"
#include "Tree.h"

using namespace GeometryLib;
using namespace std;

namespace Tree
{
	void LoadTreeFromFile(string treeFile, string plyModelFile, TreeBranch** tree, Box** boundingBox);
}