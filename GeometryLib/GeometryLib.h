#pragma once

#include "BaseGeometry.h"
#include <vector>

namespace GeometryLib
{
	using namespace MathLib;

#pragma region Box
class Box
{
public:
	~Box();
	Box(Box& clone);
	Box(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);

	#pragma region BaseRenderable
	bool RayIntersects(Ray& ray, float* intersection);
	bool RayIntersects(Ray& ray, float& intersectionMin, float& intersectionMax);
	bool RayBundleIntersects(RayBundle& bundle, __m128& intersectionsMin, __m128& intersectionsMax);
	float SurfaceArea();
	float Volume();
	#pragma endregion

	float **boundaries;

private:
	void Initialise(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);
};
#pragma endregion

class Triangle : public BaseRenderable
{
	public:
		Triangle(C3Vector first, C3Vector second, C3Vector third);
		C3Vector CalculateTrueNormal();
		#pragma region BaseRenderable
		bool RayIntersects(Ray& ray, float* intersection);
		int AxisAlignedPlaneIntersects(int planeAxis, float planePoint);
		int BoxIntersects(Box& box);
		Box BoundingBox();
		#pragma endregion

		C3Vector points[3], trueNormal;

		float dN;
		
		/*
			Constant of k = 1 / (e-a)
		*/
		float k;

		/*
			Constant n = k(f-b)
		*/
		float n;

		/*		
			Constant of m = 1/(k(c-a)(f-b)+(d-b))
		*/
		float m;

		/*
			Constant of a and b
		*/
		float a, b;

		/*
			Constant of o = c - a
		*/
		float o;

		int xIndex, yIndex;

		// The index of this triangle for use by collections
		int entityIndex;
private:
	    int MajorAxis();
};

Box GetBoundaries(std::vector<Triangle*> renderables);

}