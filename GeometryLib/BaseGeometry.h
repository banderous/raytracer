#pragma once

#include "..\MathLib\MathLib.h"
#include "..\Interfaces\RayBundle.h"

namespace GeometryLib
{
	using namespace MathLib;

	class Box;

	class BaseRenderable
	{
	public:
		virtual bool RayIntersects(Ray& ray, float* intersectionDistance) = 0;

		/*
			Plane axes are 0 = x, 1 = y, 2 = z
			Plane normal is positive direction
			Return values:
			-1: Completely behind plane
			 0: Intersecting plane
			 1: Completely in front of plane
		*/
		virtual int AxisAlignedPlaneIntersects(int planeAxis, float planePoint) = 0;

		/*
			-1: No intersection with box
			 0: Partial intersection with box
			 1: Complete containment in box
		*/
		virtual int BoxIntersects(Box& box) = 0;

		/*
			Must return the bounding box of the object
		*/
		virtual GeometryLib::Box BoundingBox() = 0;
	};
}