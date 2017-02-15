#pragma once
#include <vector>
#include "TreeTriangle.h"
#include <fstream>

namespace TreeCompiler
{
	namespace Tree
	{
		using namespace std;
		using namespace GeometryLib;

		#define WriteLine(a, b) a << b << '\n';

		class BaseTree
		{
			public:
				void Serialise(string outputFile)
				{
					fstream out(outputFile.c_str(), ios::out);
					if (!out.is_open())
					{
						throw "Unable to open file for writing";
					}

					Serialise(out);
					out.close();
				}

				virtual void Serialise(fstream& stream) = 0;
		};

		class TreeBranch : public BaseTree
		{
			public:
				TreeBranch(vector<Triangle*>& triangles, Box& enclosingBox, unsigned int depth, unsigned int splitThreshold, unsigned int maxDepth);
				int splitAxis;
				float splitPoint;
				BaseTree* left, *right;

			private:
				void Serialise(fstream& stream);
		};

		class TreeLeaf : public BaseTree
		{
			public:
				TreeLeaf(vector<Triangle*> triangles);
				vector<Triangle*> members;

			private:
				void Serialise(fstream& stream);
		};
	}
}