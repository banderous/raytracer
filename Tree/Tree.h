#pragma once

#include "..\MathLib\MathLib.h"
#include "..\GeometryLib\GeometryLib.h"
#include "..\Interfaces\RayBundle.h"
#include "emmintrin.h"

#define CACHE_ALIGN __declspec(align(16))

using namespace GeometryLib;
using namespace std;

namespace Tree {

	class TreeNode
	{
		public:
			virtual void Foo(){}
			// virtual void RayHits(RayBundle& ray, __m128 entries, __m128 exits) = 0;
	};

	class TreeLeaf;	

	class TreeBranch : public TreeNode
	{
		public:
			void RayHits(RayBundle& ray, __m128 entry, __m128 exit);

			union
			{
				TreeBranch* branchChildren[2];
				TreeLeaf* leafChildren[2];
			};

			float splitPoint;
			char childLeaves;
			char splitAxis;
	};	

	class SerialLeaf : public TreeNode
	{
		public:
			SerialLeaf(std::vector<Triangle*> members);
			Triangle* RayHits(Ray& ray, float entry, float exit, float& firstHitDistance);

		private:
			std::vector<Triangle*> members;
	};

	class TreeLeaf : public TreeNode
	{
		public:
			TreeLeaf(std::vector<Triangle*> members);
			void RayHits(RayBundle& ray, __m128 entry, __m128 exit);

	/*
		Triangle is defined by 3 vectors.
		1. The intersection between the ray and each triangle's plane is computed.
		A decision is then made for each triangle as to whether the intersection point lies inside the triangle.
		Each triangle is approximated to one of the major axes: XY, YZ, ZY with the point containment calculation
		done in 2D.

		Triangle = Q(a,b), R(c,d), S(e,f)
		Intersection point = P(g,h)
		Barycentric coordinates of intersection point = (u, v)
		Intersection occurs if 0 < u < 1 and 0 < v < 1 and u + v < 1

		Firstly 1.
		The triangle plane is defined by an origin D and normal N.
		Our ray is given by origin O and Direction S, with magnitude m.
		The distance t from O along S to point P of intersection with the plane is given by:
		t = D.N - O.N
			   S.N

		D.N is a constant and precomputed for each triangles plane in dN.
		The output from 1 is the distances t.

		So P(g,h) is given by the following:

		1. g = u(e-a) + v(c-a) + a
		2. h = u(f-b) + v(d-b) + b		

		We can solve these simultaneous equations to find u or v.
		If we rearrange 1 to find u we get:

		3. u = (g - v(c-a) - a)(1/(e-a))

		This computation for u will not work where e - a = 0. It is a requirement that for all our triangles e - a != 0.
		If necessary the ordering of the points in triangles should be adjusted to ensure this.

		Let k = 1 / (e-a)
		so u = k(g - a) - k(v(c-a))

		Substituting this into 2:

		h = k(g - a)(f-b) - vk(c-a)(f-b) + v(d-b) + b

		Making v the subject:
		
		v = 1/((d-b) - k(c-a)(f-b))(h - k(g - a)(f-b) - b)

		Let m = 1/((d-b) - k(c-a)(f-b))
		Let n = k(f-b)
		
		v = m(h - n(g - a) - b)

		Now with this value for v we need to compute u.
		We could use either 1 or 2 to solve this, but we'll use 1 since we have already ensured e - a != 0

		So u = k(g - v(c-a) - a)
		Let o = c - a

		Then u = k(g - vo - a)

		We now have values for v and u
	*/
	private:

		/*
			The dot products of the plane origin vector D with the plane normal N
		*/
		float *dN;
		
		/*
			The x, y, z components of the vectors making up the triangles' plane origins
		*/
		float *planeOriginsX, *planeOriginsY, *planeOriginsZ;

		/*
			The x, y, z components of the vectors making up the triangles' plane directions
		*/
		float *planeDirectionsX, *planeDirectionsY, *planeDirectionsZ;

		/*
			Constant of k = 1 / (e-a) for each triangle
		*/
		float *k;

		/*
			Constant n = k(f-b) for each triangle
		*/
		float *n;

		/*		
			Constant of m = 1/(k(c-a)(f-b)+(d-b)) for each triangle
		*/
		float *m;

		/*
			Constant of a and b for each triangle 
		*/
		float *a, *b;

		/*
			Constant o = c - a for each triangle
		*/
		float *o;

		/*
			The X and Y axes for each triangle
		*/
		int *triangleXIndicies, *triangleYIndicies;
		int *triangleIndicies;
		int nbMembers;
	};
}
