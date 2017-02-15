#include "Helper.h"

namespace TreeCompiler
{
	namespace Helper
	{
		/*
			Determine the split plane and point for the specified triangle list.
			Returns true if advantageous to split, false otherwise
		*/
		bool DetermineSplitSuitabilityAndProperties(std::vector<Triangle*>& triangles, Box& boundingBox, int &splitAxis, float &splitPoint)
		{
			float smallestCost = boundingBox.SurfaceArea() * triangles.size() * TRIANGLETRAVERSALCOST;
			int cheapestAxis = 0;
			float cheapestSplitPoint = boundingBox.boundaries[0][0] + ((boundingBox.boundaries[0][1] - boundingBox.boundaries[0][0]) * 0.5f);

			if (false)
			{
				int numPlanes = 1;
				float planeIncrement = 1l / float (numPlanes + 1);				
				for (int t = 0; t < 3; t++)
				{
					float currentIncrement = planeIncrement;
					for (int u = 0; u < numPlanes; u++)
					{
						float potentialSplitPoint = boundingBox.boundaries[t][0] + ((boundingBox.boundaries[t][1] - boundingBox.boundaries[t][0]) * (currentIncrement));
						float potentialCost = EvaluateSplitCost(triangles, boundingBox, t, potentialSplitPoint);
						if (potentialCost < smallestCost)
						{
							cheapestAxis = t;
							cheapestSplitPoint = potentialSplitPoint;
							smallestCost = potentialCost;
						}

						currentIncrement += planeIncrement;
					}
				}
			}
			else
			{
				for (int t = 0; t < 3; t++)
				{
					for (int u = 0; u < triangles.size(); u++)
					{
						Box bb = triangles[u]->BoundingBox();
						for (int v = 0; v < 2; v++)
						{							
							float potentialSplitPoint = bb.boundaries[t][v];
							float potentialCost = EvaluateSplitCost(triangles, boundingBox, t, potentialSplitPoint);
							if (potentialCost < smallestCost)
							{
								cheapestAxis = t;
								cheapestSplitPoint = potentialSplitPoint;
								smallestCost = potentialCost;
							}
						}
					}
				}
			}

			// No point in splitting
			if (smallestCost >= boundingBox.SurfaceArea() * triangles.size() * TRIANGLETRAVERSALCOST)
				return false;

			splitAxis = cheapestAxis;
			splitPoint = cheapestSplitPoint;
			
			return true;
		}

		float EvaluateSplitCost(std::vector<Triangle*>& triangles, Box& boundingBox, int potentialSplitAxis, float potentialSplitPoint)
		{
			Box leftBox(boundingBox);
			leftBox.boundaries[potentialSplitAxis][1] = potentialSplitPoint;
			Box rightBox(boundingBox);
			rightBox.boundaries[potentialSplitAxis][0] = potentialSplitPoint;
			
			vector<Triangle*> leftPotentials, rightPotentials;
			PartitionTriangles(triangles, leftPotentials, rightPotentials, boundingBox, potentialSplitAxis, potentialSplitPoint);

			float leftArea = leftBox.SurfaceArea();
			float rightArea = rightBox.SurfaceArea();

			float leftCost = leftPotentials.size() * TRIANGLETRAVERSALCOST * leftArea;
			float rightCost = rightPotentials.size() * TRIANGLETRAVERSALCOST * rightArea;
			
			return leftCost + rightCost + SPLITTRAVERSALCOST;
		}

		/*
			Partition the given set of triangles into left and right according to the potential split parameters
		*/
		void PartitionTriangles(vector<Triangle*>& triangles, vector<Triangle*>& left, vector<Triangle*>& right, Box& boundingBox, int potentialSplitAxis, float potentialSplitPoint)
		{
			for (unsigned int t = 0; t < triangles.size(); t++)
			{
				Triangle* currentTriangle = triangles[t];
				switch (currentTriangle->AxisAlignedPlaneIntersects(potentialSplitAxis, potentialSplitPoint))
				{
					case -1: // Behind the split plane
					{
						left.push_back(currentTriangle);
					}
					break;
					case 0: // Straddling the split plane
					{
						left.push_back(currentTriangle);
						right.push_back(currentTriangle);
					}
					break;
					case 1: // In front of the split plane
					{
						right.push_back(currentTriangle);
					}
					break;
				}
			}
		}
	}
}