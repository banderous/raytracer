#pragma once

#include "..\GeometryLib\GeometryLib.h"
#include <vector>
#include <iostream>

using namespace GeometryLib;

/*
	Sort the list of triangles by their major axis.
	Axis precendence: XY, YZ, XZ
*/
void SortTriangles(std::vector<Triangle*>& triangles, int& nbXY, int& nbYZ, int& nbXZ);

/*
	Determine the major axis of the triangle.
	That is, the axis that makes the smallest angle with the triangle's normal.
	0 = XY, 1 = YZ, 2 = XZ
*/
int TriangleMajorAxis(Triangle* triangle);