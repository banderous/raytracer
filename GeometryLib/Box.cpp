#include "GeometryLib.h"
#include <limits>
#include "Math.h"
#include <algorithm> // Min and Max functions

namespace GeometryLib
{
	using namespace std;

	Box::~Box()
	{
		delete [] this->boundaries[0];
		delete [] this->boundaries[1];
		delete [] this->boundaries[2];
		delete [] this->boundaries;
	}

	/*
		Constructs a box using 6 values that represent its' min and max boundaries on all three axes.
	*/
	Box::Box(float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
	{
		this->Initialise(minX, maxX, minY, maxY, minZ, maxZ);
	}

	Box::Box(Box& cloneParent)
	{
		this->Initialise(cloneParent.boundaries[0][0], cloneParent.boundaries[0][1], cloneParent.boundaries[1][0], cloneParent.boundaries[1][1], cloneParent.boundaries[2][0], cloneParent.boundaries[2][1]);
	}

	void Box::Initialise(float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
	{
		this->boundaries = new float*[3];

		this->boundaries[0] = new float[2];
		this->boundaries[0][0] = minX;
		this->boundaries[0][1] = maxX;

		this->boundaries[1] = new float[2];
		this->boundaries[1][0] = minY;
		this->boundaries[1][1] = maxY;

		this->boundaries[2] = new float[2];
		this->boundaries[2][0] = minZ;
		this->boundaries[2][1] = maxZ;
	}

	/*
		Implementation of MaxBaseRenderable::RayIntersects(Ray, float*)
		Intersection test uses the separating axis theorum.
		The box is defined as three sets of two minima and maxima planes; one set for each axis.
		For each set of planes, one will be that crossed as the plane enters the box, one leaves.
		If the nearest leaving plane is closer than the furthest entering plane then the ray does not intersect.
	*/
	bool Box::RayIntersects(Ray& ray, float& intersectionMin, float& intersectionMax)
	{
		intersectionMax = 99999999.0f;
		intersectionMin = -99999999.0f;

		for (int axis = 0; axis < 3; axis++)
		{
			float intersectA = (boundaries[axis][0] - ray.origin.values[axis]) / ray.direction.values[axis];
			float intersectB = (boundaries[axis][1] - ray.origin.values[axis]) / ray.direction.values[axis];

			float entering = min(intersectA, intersectB);
			float leaving  = max(intersectA, intersectB);

			intersectionMax = min(intersectionMax, leaving);
			intersectionMin = max(intersectionMin, entering);
		}

		return intersectionMin < intersectionMax;
	}

	/*
		Implementation of BaseRenderable overload
	*/
	bool Box::RayIntersects(Ray& ray, float* intersectionDistance)
	{
		float min, max;
		if (this->RayIntersects(ray, min, max))
		{
			*intersectionDistance = min;
			return true;
		}
		
		return false;
	}

	/*
		Intersection with a ray bundle.
		Returns true if one or more rays hit the box.
		IntersectionDistances set to distances or 0 where not intersecting.
	*/
	bool Box::RayBundleIntersects(RayBundle& bundle, __m128& distancesMin, __m128& distancesMax)
	{
		__m128 intersectionMin = _mm_set1_ps(-999999999.0f);
		__m128 intersectionMax = _mm_set1_ps(999999999.0f);

		for (int axis = 0; axis < 3; axis++)
		{
			__m128 boundaryA = _mm_set1_ps(boundaries[axis][0]);
			__m128 boundaryB = _mm_set1_ps(boundaries[axis][1]);

			__m128 boundaryAIntersections = _mm_mul_ps(_mm_sub_ps(boundaryA, bundle.rayOrigins[axis]), bundle.rayReciprocalDirections[axis]);
			__m128 boundaryBIntersections = _mm_mul_ps(_mm_sub_ps(boundaryB, bundle.rayOrigins[axis]), bundle.rayReciprocalDirections[axis]);

			__m128 entering = _mm_min_ps(boundaryAIntersections, boundaryBIntersections);
			__m128 leaving = _mm_max_ps(boundaryAIntersections, boundaryBIntersections);

			intersectionMax = _mm_min_ps(intersectionMax, leaving);
			intersectionMin = _mm_max_ps(intersectionMin, entering);
		}

		__m128 validityMask = _mm_cmplt_ps(intersectionMin, intersectionMax);

		distancesMin = _mm_set1_ps(99999999.0f);
		distancesMin = _mm_xor_ps( distancesMin, _mm_and_ps( validityMask, _mm_xor_ps(intersectionMin, distancesMin ) ) );

		distancesMax = _mm_set1_ps(-99999999.0f);
		distancesMax = _mm_xor_ps( distancesMax, _mm_and_ps( validityMask, _mm_xor_ps(intersectionMax, distancesMax ) ) );

		return _mm_movemask_ps(validityMask) != 0; // If one or more was valid
	}
	
	/*
		Surface area
	*/
	float Box::SurfaceArea()
	{
		return 2 * (boundaries[0][1] - boundaries[0][0]) * (boundaries[1][1] - boundaries[1][0]) + // Front and back
			   2 * (boundaries[0][1] - boundaries[0][0]) * (boundaries[2][1] - boundaries[2][0]) + // Top and bottom
			   2 * (boundaries[1][1] - boundaries[1][0]) * (boundaries[2][1] - boundaries[2][0]);  // Left and right
	}

	/*
		Volume
	*/
	float Box::Volume()
	{
		return (boundaries[0][1] - boundaries[0][0]) * (boundaries[1][1] - boundaries[1][0]) * (boundaries[2][1] - boundaries[2][0]);
	}
}