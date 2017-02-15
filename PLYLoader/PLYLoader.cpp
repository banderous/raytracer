#include "PLYLoader.h"
#include "..\MathLib\MathLib.h"
#include <fstream>
#include <string>


namespace PLYLoader
{
	using namespace std;
	using namespace MathLib;

	vector<Triangle*> LoadPLY(string plyPath)
	{
		fstream in(plyPath.c_str(), ios::in);
		if (!in.is_open())
		{
			throw string("Unable to open ply file for reading");
		}

		string lineA;
		std::getline(in, lineA);

		float scale = 1.0f;
		if (lineA != "ply")
		{
			scale = (float) atof(lineA.c_str());
			if (scale == 0) // Not a valid scale
			{
				throw string("Invalid ply file");
			}

			std::getline(in, lineA);
			if (lineA != "ply")
			{
				throw string("Invalid ply file");
			}
		}

		std::getline(in, lineA);

		std::getline(in, lineA);
		std::getline(in, lineA);

		int numVertices = atoi(lineA.substr(15, lineA.length()).c_str());
		std::getline(in, lineA);
		std::getline(in, lineA);
		std::getline(in, lineA);
		std::getline(in, lineA);
		std::getline(in, lineA);

		std::getline(in, lineA);
		int numFaces = atoi(lineA.substr(13, lineA.length()).c_str());
		std::getline(in, lineA);
		std::getline(in, lineA);

		vector<C3Vector> vertices;
		for (int vertexNum = 0; vertexNum < numVertices; vertexNum++)
		{
			float x, y, z;
			in >> x;
			in >> y;
			in >> z;

			vertices.push_back(C3Vector(x * scale, y * scale, z * scale));
			
			// Junk the other 2 components and the endline
			in >> lineA; in >> lineA;

		}

		int index = 0;

		vector<Triangle*> result;
		for (int faceNum = 0; faceNum < numFaces; faceNum++)
		{
			int index1, index2, index3;
			in >> lineA; // Junk the number of elements
			in >> index3;
			in >> index2;
			in >> index1;


			Triangle* tri;
			try
			{
				tri = new Triangle(vertices[index1], vertices[index2], vertices[index3]);
			}
			catch (int)
			{
				try
				{
					tri = new Triangle(vertices[index3], vertices[index1], vertices[index2]);
				}
				catch (string)
				{
					tri = new Triangle(vertices[index2], vertices[index3], vertices[index1]);
				}
			}

			tri->entityIndex = faceNum;

			result.push_back(tri);
		}

		return result;
	}
}