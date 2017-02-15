#pragma once

#include "Tree.h"
#include <vector>
#include "TreeTriangle.h"

namespace TreeCompiler
{
	namespace Helper
	{
		using namespace MathLib;
		using namespace std;
		using namespace Tree;
		
		#define SPLITTRAVERSALCOST 1l
		#define TRIANGLETRAVERSALCOST (SPLITTRAVERSALCOST * 20)
	
		/*
			Determine the split plane and point for the specified triangle list
		*/
		bool DetermineSplitSuitabilityAndProperties(std::vector<Triangle*>& triangles, Box& boundingBox, int &splitAxis, float &splitPoint);

		float EvaluateSplitCost(std::vector<Triangle*>& triangles, Box& boundingBox, int potentialSplitAxis, float potentialSplitPoint);

		void PartitionTriangles(vector<Triangle*>& triangles, vector<Triangle*>& left, vector<Triangle*>& right, Box& boundingBox, int potentialSplitAxis, float potentialSplitPoint);
	}
}