#pragma once

#include "..\GeometryLib\GeometryLib.h"

namespace TreeCompiler
{
	namespace Tree
	{
		class Triangle : public GeometryLib::Triangle
		{
		public:
			Triangle(GeometryLib::Triangle* clone, int index) : GeometryLib::Triangle(clone->points[0], clone->points[1], clone->points[2]){this->index = index;}
			int index;
		};
	}
}