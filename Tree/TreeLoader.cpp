#include "TreeLoader.h"
#include "..\PLYLoader\PLYLoader.h"
#include <fstream>

namespace Tree
{
	using namespace PLYLoader;

	int largestLeaf, leafCount, leafMemberSum;

	TreeNode* LoadTreeFromStream(fstream& stream, vector<Triangle*>& modelTriangles)
	{
		string line;
		stream >> line;

		// Branch consists of axis of orientation followed by split point then two children
		if (line == "BRANCH")
		{
			string junk;
			TreeBranch* result = new TreeBranch();
			result->childLeaves = 0;

			int axis;
			stream >> axis;
			result->splitAxis = (char) axis;

			stream >> result->splitPoint;

			TreeNode* children[2];
			children[0] = LoadTreeFromStream(stream, modelTriangles);
			children[1] = LoadTreeFromStream(stream, modelTriangles);

			for (int t = 0; t < 2; t++)
			{
				TreeLeaf* asLeaf = dynamic_cast<TreeLeaf*>(children[t]);
				if (asLeaf)
				{
					result->childLeaves = result->childLeaves | (1 << t);
					result->leafChildren[t] = asLeaf;
				}
				else
				{
					TreeBranch* asBranch = dynamic_cast<TreeBranch*>(children[t]);

					result->branchChildren[t] = asBranch;
				}
			}

			return result;
		}
		else if (line == "LEAF")
		{
			int numChildren;
			stream >> numChildren;	

			std::vector<Triangle*> members;
			for (int t = 0; t < numChildren; t++)
			{
				int triangleIndex;
				stream >> triangleIndex;
				members.push_back(modelTriangles[triangleIndex]);
			}			

			if (numChildren > largestLeaf)
			{
				largestLeaf = numChildren;
			}

			leafCount++;
			leafMemberSum += numChildren;
			
			return new TreeLeaf(members);
		}
		
		throw string("Invalid tree file");
	}

	void LoadTreeFromFile(string treeFile, string plyModelFile, TreeBranch** tree, Box** boundingBox)
	{
		vector<Triangle*> triangles = LoadPLY(plyModelFile);

		fstream in(treeFile.c_str(), ios::in);
		if (!in.is_open())
		{
			throw string("Unable to open tree file for reading");
		}

		float minX, maxX, minY, maxY, minZ, maxZ;
		in >> minX;
		in >> maxX;
		in >> minY;
		in >> maxY;
		in >> minZ;
		in >> maxZ;

		*boundingBox = new Box(minX, maxX, minY, maxY, minZ, maxZ);

		largestLeaf = 0, leafCount = 0, leafMemberSum = 0;
		*tree = (TreeBranch*) LoadTreeFromStream(in, triangles);

		printf("Total leaf count: %i\n", leafCount);
		printf("Mean leaf size: %i\n", (int) ((float) leafMemberSum / (float) leafCount));
		printf("Largest leaf size: %i\n", largestLeaf);

		for (unsigned int t = 0; t < triangles.size(); t++)
		{
			delete triangles[t];
		}
	}
}