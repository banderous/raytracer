#include "Tree.h"
#include "TriangleSorter.h"
#include "..\Interfaces\RayBundle.h"

namespace Tree
{
	// Macro for allocating m128d variables on the head on 16 byte aligned boundaries
	#define ALIGNED_MALLOC16(nbRegisters) (__m128*) _aligned_malloc(nbRegisters * sizeof(__m128), 16);

	/*
		Nothing much yet
	*/
	TreeLeaf::TreeLeaf(std::vector<Triangle*> members)
	{
		int nbAllocate = members.size();

		this->a = new float[nbAllocate];
		this->b = new float[nbAllocate];
		this->dN = new float[nbAllocate];
		this->k = new float[nbAllocate];
		this->m = new float[nbAllocate];
		this->n = new float[nbAllocate];
		this->o = new float[nbAllocate];
		this->planeOriginsX = new float[nbAllocate];
		this->planeOriginsY = new float[nbAllocate];
		this->planeOriginsZ = new float[nbAllocate];
		this->planeDirectionsX = new float[nbAllocate];
		this->planeDirectionsY = new float[nbAllocate];
		this->planeDirectionsZ = new float[nbAllocate];
		this->triangleXIndicies = new int[nbAllocate];
		this->triangleYIndicies = new int[nbAllocate];
		this->triangleIndicies = new int[nbAllocate];

		this->nbMembers = members.size();

		// Assumption that all our triangles are on the XY plane
		for (int t = 0; t < nbMembers; t++)
		{
			Triangle* triangle = members[t];

			int xIndex = triangle->xIndex;
			int yIndex = triangle->yIndex;
					
			float a = triangle->points[0].values[xIndex],
				   b = triangle->points[0].values[yIndex],				
				   c = triangle->points[1].values[xIndex],
				   d = triangle->points[1].values[yIndex],
				   e = triangle->points[2].values[xIndex],
				   f = triangle->points[2].values[yIndex];

			this->triangleXIndicies[t] = xIndex;
			this->triangleYIndicies[t] = yIndex;

			// Load a
			this->a[t] = triangle->points[0].values[xIndex];

			// Load b
			this->b[t] = triangle->points[0].values[yIndex];

			// Load the dot products of the plane origin vector D with the plane normal N
			float dot = triangle->points[0] * triangle->CalculateTrueNormal();
			this->dN[t] = dot;

			// Load k = 1 / (e-a)
			float k1 = 1.0f / (e - a);
			this->k[t] = k1;

						
			// Load m = 1l/(k*(c-a)*(f-b)-(d-b))
			this->m[t] = 1.0f / (k1*(c - a)*(f-b)-(d-b));

			// Load n = k(f-b)
			this->n[t] = k1 * (f - b);

			// Load o = c - a
			this->o[t] = c - a;

			// Load the planeOrigins x
			planeOriginsX[t] = triangle->points[0].values[0];

			// Load the planeOrigins y
			planeOriginsY[t] = triangle->points[0].values[1];

			// Load the planeOrigins z
			planeOriginsZ[t] = triangle->points[0].values[2];

			C3Vector normal = triangle->CalculateTrueNormal();

			// Load the planeDirections x
			planeDirectionsX[t] = normal.values[0];

			// Load the planeDirections y
			planeDirectionsY[t] = normal.values[1];

			// Load the planeDirections z
			planeDirectionsZ[t] = normal.values[2];

			// Store its index
			triangleIndicies[t] = triangle->entityIndex;
		}
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
	void TreeLeaf::RayHits(RayBundle& ray, __m128 entries, __m128 exits)
	{
		for (int t = 0; t < this->nbMembers; t++)
		{
			int xIndex = this->triangleXIndicies[t], yIndex = this->triangleYIndicies[t];

			__m128 planeX = _mm_set1_ps(planeDirectionsX[t]), planeY = _mm_set1_ps(planeDirectionsY[t]), planeZ = _mm_set1_ps(planeDirectionsZ[t]);
			__m128 sN = _mm_add_ps(_mm_add_ps(_mm_mul_ps(ray.rayDirections[0], planeX), _mm_mul_ps(ray.rayDirections[1], planeY)), _mm_mul_ps(ray.rayDirections[2], planeZ));

			__m128 backFacingCull = _mm_cmpgt_ps(sN, _mm_setzero_ps());
			if (_mm_movemask_ps(backFacingCull) == 15) // All the rays are facing back of triangle. Skip it.
			{
				continue;
			}

			__m128 oN = _mm_add_ps(_mm_add_ps(_mm_mul_ps(ray.rayOrigins[0], planeX), _mm_mul_ps(ray.rayOrigins[1], planeY)), _mm_mul_ps(ray.rayOrigins[2], planeZ));

			__m128 intersectionDistances = _mm_div_ps(_mm_sub_ps(_mm_set1_ps(dN[t]), oN), sN);
			
			__m128 rangeValidity = _mm_and_ps(_mm_cmpge_ps(intersectionDistances, entries), _mm_cmple_ps(intersectionDistances, exits));
			if (!_mm_movemask_ps(rangeValidity))
			{
				continue; // none are valid
			}

			// g and h are the coordinates of the intersection point between ray and plane
			__m128 g = _mm_add_ps(ray.rayOrigins[xIndex], _mm_mul_ps(ray.rayDirections[xIndex], intersectionDistances));
			__m128 h = _mm_add_ps(ray.rayOrigins[yIndex], _mm_mul_ps(ray.rayDirections[yIndex], intersectionDistances));

			// Now we have the coordinates of intersection on the XY plane. Determine whether the point is inside the triangle.
			// Compute v = m*((g-a)*n - h + b)
			__m128 v = _mm_mul_ps(_mm_set1_ps(m[t]),
									_mm_add_ps(_mm_sub_ps(_mm_mul_ps(_mm_sub_ps(g, _mm_set1_ps(a[t])), _mm_set1_ps(n[t])), h), _mm_set1_ps(b[t])));


			// Now compute u = k(g - vo - a)
			 __m128 u = _mm_mul_ps(_mm_set1_ps(k[t]),
									_mm_sub_ps(_mm_sub_ps(g, _mm_mul_ps(v, _mm_set1_ps(o[t]))), _mm_set1_ps(a[t])));
			 
			 __m128 uPlusD = _mm_add_ps(u, v);
			 __m128 validityFlag = _mm_and_ps
									(
										// Ensure both u and v are greater than zero and less than 1
										_mm_and_ps
										(
											_mm_and_ps
											(
											_mm_cmpge_ps(u, _mm_set1_ps(ONE_MINUS_EPSILON)),
												_mm_cmple_ps(u, _mm_set1_ps(ONE_PLUS_EPSILON))
											),											
											_mm_and_ps
											(
												_mm_cmpge_ps(v, _mm_set1_ps(ONE_MINUS_EPSILON)),
												_mm_cmple_ps(v, _mm_set1_ps(ONE_PLUS_EPSILON))
											)
										),

										_mm_and_ps
										(
											// Ensure the distances are within range
											rangeValidity
										,											
											// Ensure that u + v < 1 and distances are less than current closest
											_mm_and_ps
											(
												// Ensure that u + v < 1
												_mm_cmple_ps(uPlusD, _mm_set1_ps(ONE_PLUS_EPSILON)),

												// Ensure that distances are less than current closest
												_mm_cmplt_ps(intersectionDistances, ray.struckDistances)
											)
										)
									);

			  // Select the new closest distances where the validity flag is set
			 ray.struckDistances = _mm_xor_ps( ray.struckDistances, _mm_and_ps( validityFlag, _mm_xor_ps(intersectionDistances, ray.struckDistances ) ) );

			  // Select the ray's closest indicies to this one where appropriate
			ray.struckIndicies = _mm_xor_ps( ray.struckIndicies, _mm_and_ps( validityFlag, _mm_xor_ps( _mm_set1_ps(triangleIndicies[t]), ray.struckIndicies ) ) );
			ray.validity = _mm_or_ps(ray.validity, validityFlag);
		}
	}
}