#include "Tree.h"
#include "Helper.h"

using namespace TreeCompiler::Helper;

namespace TreeCompiler
{
	namespace Tree
	{
		TreeBranch::TreeBranch(vector<Triangle*>& triangles, Box& enclosingBox, unsigned int depth, unsigned int splitThreshold, unsigned int maxDepth)
		{
			// Decide split plane and point. Throw if splitting not appropriate
			if (!DetermineSplitSuitabilityAndProperties(triangles, enclosingBox, splitAxis, splitPoint))
			{
				throw -1;
			}

			std::vector<Triangle*> leftMembers, rightMembers;
			PartitionTriangles(triangles, leftMembers, rightMembers, enclosingBox, splitAxis, splitPoint);

			if (leftMembers.size() > splitThreshold && depth < maxDepth)
			{
				Box childBox(enclosingBox);
				childBox.boundaries[splitAxis][1] = splitPoint; // Update leading edge of left hand child
				try
				{
					left = new TreeBranch(leftMembers, childBox, splitThreshold, depth + 1, maxDepth);
				}
				catch (int)
				{
					left = new TreeLeaf(leftMembers);
				}
			}
			else
			{
				left = new TreeLeaf(leftMembers);
			}

			if (rightMembers.size() > splitThreshold && depth < maxDepth)
			{
				Box childBox(enclosingBox);
				childBox.boundaries[splitAxis][0] = splitPoint; // Update trailing edge of right hand child
				try
				{
					right = new TreeBranch(rightMembers, childBox, splitThreshold, depth + 1, maxDepth);
				}
				catch (int)
				{
					right = new TreeLeaf(rightMembers);
				}
			}
			else
			{
				right = new TreeLeaf(rightMembers);
			}
		}

		void TreeBranch::Serialise(std::fstream& stream)
		{
			WriteLine(stream, "BRANCH");
			WriteLine(stream, this->splitAxis);
			WriteLine(stream, this->splitPoint);

			this->left->Serialise(stream);
			this->right->Serialise(stream);
		}
	}
}