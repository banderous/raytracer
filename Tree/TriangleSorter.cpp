#include "TriangleSorter.h"
#include <algorithm>
#include "math.h"

using namespace std;
using namespace GeometryLib;


bool SortTwoTriangles(Triangle* a, Triangle* b)
{
	return TriangleMajorAxis(a) < TriangleMajorAxis(b);
}

/*
	See header
*/
int TriangleMajorAxis(Triangle* triangle)
{
	C3Vector xyNormal(0, 0, 1);
	C3Vector yzNormal(1, 0, 0);
	C3Vector xzNormal(0, 1, 0);

	C3Vector triangleNormal = triangle->CalculateTrueNormal();
	float xyAngle = fabs(triangleNormal * xyNormal);
	float yzAngle = fabs(triangleNormal * yzNormal);
	float xzAngle = fabs(triangleNormal * xzNormal);

	int smallestAngleAxis = 0;
	float smallestAngle = xyAngle;
	if (yzAngle > smallestAngle)
	{
		smallestAngleAxis = 1;
		smallestAngle = yzAngle;
	}
	if (xzAngle > smallestAngle)
	{
		smallestAngleAxis = 2;
		smallestAngle = xzAngle;
	}

	return smallestAngleAxis;
}

void SortTriangles(std::vector<Triangle*>& triangles, int& nbXY, int& nbYZ, int& nbXZ)
{
	std::sort(triangles.begin(), triangles.end(), &SortTwoTriangles);

	nbXY = 0;
	nbYZ = 0;
	nbXZ = 0;

	for (unsigned int t = 0; t < triangles.size(); t++)
	{
		switch (TriangleMajorAxis(triangles[t]))
		{
			case 0:
				nbXY++;
			break;
			case 1:
				nbYZ++;
			break;
			case 2:
				nbXZ++;
			break;
		}
	}
}