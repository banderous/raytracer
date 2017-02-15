#pragma once

#include "..\GeometryLib\GeometryLib.h"
#include <string>
#include <vector>

namespace PLYLoader
{
	using namespace GeometryLib;
	using namespace std;

	vector<Triangle*> LoadPLY(string plyPath);
}