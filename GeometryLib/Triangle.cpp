#include "GeometryLib.h"
#include <algorithm>
#include "math.h"

namespace GeometryLib
{
	using namespace std;

	Triangle::Triangle(C3Vector first, C3Vector second, C3Vector third)
	{
		this->points[0] = first;
		this->points[1] = second;
		this->points[2] = third;

		this->trueNormal = this->CalculateTrueNormal();

		/*
			Our major axis is that whose normal most closely matches that of the triangle
		*/
		switch (MajorAxis())
		{
			// XY axis
			case 0:
			{
				xIndex = 0;
				yIndex = 1;
			}
			break;
			// YZ axis
			case 1: 
			{
				xIndex = 2;
				yIndex = 1;
			}
			break;
			// XZ axis
			case 2:
			{
				xIndex = 0;
				yIndex = 2;
			}
			break;
		}

		float a = points[0].values[xIndex], b = points[0].values[yIndex], c = points[1].values[xIndex], d = points[1].values[yIndex], e = points[2].values[xIndex], f = points[2].values[yIndex];

		this->dN = points[0] * trueNormal;

		if (e - a == 0)
		{
			throw -1;
		}

		this->k = (float) (1.0f / (float)(e - a));
		this->n = k*(f-b);
		this->m = 1.0f/(k*(c-a)*(f-b)-(d-b));
		this->o = c - a;

		this->a = points[0].values[xIndex];
		this->b = points[0].values[yIndex];
	}

		
	/*
		Plane axes are 0 = x, 1 = y, 2 = z
		Plane normal is positive direction
		Return values:
		-1: Completely behind plane
		 0: Intersecting plane
		 1: Completely in front of plane
	*/
	int Triangle::AxisAlignedPlaneIntersects(int planeAxis, float planePoint)
	{
		int result = 0;
		for (int t = 0; t < 3; t++)
		{
			result += ((points[t].values[planeAxis] > planePoint) ? 1 : -1); // If in front of add 1, if behind subtract
		}

		switch (result)
		{
			case -3: // All points behind the plane
				return -1;
			case 3:  // All point in front of the plane
				return 1;
			default: // Must be intersecting the plane
				return 0;
		}
	}

	/*
		-1: No intersection with box
		 0: Partial intersection with box
		 1: Complete containment in box
	*/
	int Triangle::BoxIntersects(Box& box)
	{
		return -1;
	}

	Box Triangle::BoundingBox()
	{
		float minX = min(points[0].values[0], min(points[1].values[0], points[2].values[0]));
		float maxX = max(points[0].values[0], max(points[1].values[0], points[2].values[0]));

		float minY = min(points[0].values[1], min(points[1].values[1], points[2].values[1]));
		float maxY = max(points[0].values[1], max(points[1].values[1], points[2].values[1]));

		float minZ = min(points[0].values[2], min(points[1].values[2], points[2].values[2]));
		float maxZ = max(points[0].values[2], max(points[1].values[2], points[2].values[2]));

		return Box(minX, maxX, minY, maxY, minZ, maxZ);
	}

	/*
		Calculate the normal of this triangle
	*/
	C3Vector Triangle::CalculateTrueNormal()
	{
		C3Vector vecA = points[1] - points[0];
		C3Vector vecB = points[2] - points[0];
		C3Vector cross = vecB.CrossProduct(&vecA);
		cross.Normalise();
		return cross;
	}

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

		Let k = 1 / (e-a)
		u = k(g - a) - kv(c-a)

		Putting into 2:

		h = (k(g - a) - kv(c-a))(f-b) + v(d-b) + b

		h = k(g - a)(f-b) - kv(c-a)(f-b) + v(d-b) + b

		h = k(g - a)(f-b) - v(k(c-a)(f-b) - (d-b)) + b

		Rearranging to find v:

		v = (k(g - a)(f-b) + b - h)(1/(k(c-a)(f-b) - (d-b)))

		let m = 1/(k(c-a)(f-b) - (d-b))

		then v = (k(g - a)(f-b) + b - h)m

		let n = k(f-b)

		then v = ((g - a)n + b - h)m

		Now use 1 to solve for u

		 u = (g - v(c-a) - a)k

		let o = c - a

		then u = (g - vo - a)k
	*/
	bool Triangle::RayIntersects(Ray& ray, float* intersection)
	{
		float intersectionDistance = ((this->points[0] * trueNormal) - (trueNormal * ray.origin)) / (trueNormal * ray.direction);
		
		if (intersectionDistance > 0 && intersectionDistance < 10000)
		{
			float g = ray.origin.values[xIndex] + ray.direction.values[xIndex] * intersectionDistance;
			float h = ray.origin.values[yIndex] + ray.direction.values[yIndex] * intersectionDistance;
			
			float v = m*((g-a)*n - h + b);
			float u = k*(g - v*o - a);

			if (u > 0.0f && v > 0.0f && u + v < 1.0f)
			{
				*intersection = intersectionDistance;
				return true;
			}
		}

		return false;
	}

	/*
		Determine which major axis this triangle is on
	*/
	int Triangle::MajorAxis()
	{
		C3Vector xyNormal(0, 0, 1);
		C3Vector yzNormal(1, 0, 0);
		C3Vector xzNormal(0, 1, 0);

		C3Vector triangleNormal = CalculateTrueNormal();
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
}