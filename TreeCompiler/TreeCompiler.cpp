#include "TreeCompiler.h"
#include "Helper.h"
#include "..\PLYLoader\PLYLoader.h"
#include "Tree.h"
#include "..\GeometryLib\GeometryLib.h"

namespace TreeCompiler
{
	using namespace PLYLoader;
	using namespace Helper;
	using namespace Tree;

	void CompileTree(string plyPath, string outputPath)
	{
		vector<GeometryLib::Triangle*> triangles = PLYLoader::LoadPLY(plyPath);
		CompileTree(triangles, outputPath);
	}

	void CompileTree(vector<GeometryLib::Triangle*>& triangles, string outputPath)
	{
		fstream out(outputPath.c_str(), ios::out);
		if (!out.is_open())
		{
			throw string("Unable to open file for writing");
		}

		// Save the bounding box
		Box boundaries = GetBoundaries(triangles);
		WriteLine(out, boundaries.boundaries[0][0]);
		WriteLine(out, boundaries.boundaries[0][1]);
		WriteLine(out, boundaries.boundaries[1][0]);
		WriteLine(out, boundaries.boundaries[1][1]);
		WriteLine(out, boundaries.boundaries[2][0]);
		WriteLine(out, boundaries.boundaries[2][1]);
		

		vector<Triangle*> converted;
		for (unsigned int t = 0; t < triangles.size(); t++)
		{			
			converted.push_back(new Triangle(triangles[t], t));
		}	

		BaseTree* tree = new TreeBranch(converted, boundaries, 0, 1, 999);
		tree->Serialise(out);

		out.close();
	}
}