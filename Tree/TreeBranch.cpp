#include "Tree.h"

namespace Tree
{
	using namespace std;

	#define TRAVERSAL_COMPLETE(a) (_mm_movemask_ps(a) == 15)
	#define TRAVERSE_CHILD(childIndex, ray, entry, exit){	\
															if (this->childLeaves & (1 << childIndex))	\
																leafChildren[childIndex]->RayHits(ray, entry, exit);	\
															else\
																branchChildren[childIndex]->RayHits(ray, entry, exit);	\
														}
	/*
		Main tree traversal function.
		Decide based on our splitting point which child we should traverse first.
		1. Intersect the ray with our splitting plane to determine sD, the distance from ray origin to plane intersection
		2. Decide which way to traverse based on sD relative to entry and exit:
		There are three possibilities given the assertion entry <= exit:
		   i. sD < entry
		    The ray has intersected our split plane before it entered our bounding box.

	      ii. entry < sD < exit
		    The ray strikes both our children.

	     iii. exit < sD
		    The ray has left our bounding box before intersecting our splitting plane.
  
	*/
	void TreeBranch::RayHits(RayBundle &ray, __m128 entries, __m128 exits)
	{
		__m128 splitRow = _mm_set1_ps(this->splitPoint);
		__m128 distancesToSplitPlane = _mm_mul_ps(_mm_sub_ps(splitRow, ray.rayOrigins[this->splitAxis]), ray.rayReciprocalDirections[this->splitAxis]);

		
		// If, for all rays, the intersection with split plane occured after leaving then traverse only left
		if (_mm_movemask_ps(_mm_cmpgt_ps(distancesToSplitPlane, exits)) == 15)
		{
			TRAVERSE_CHILD(!ray.directionSignFlags[splitAxis], ray, entries, exits);
			return;
		}
		// Otherwise if, for all rays, the intersection with split plane occured before entry then traverse only right
		else if (_mm_movemask_ps(_mm_cmplt_ps(distancesToSplitPlane, entries)) == 15)
		{
			TRAVERSE_CHILD(ray.directionSignFlags[splitAxis], ray, entries, exits);
			return;
		}
		else // Traverse both if necessary
		{
			TRAVERSE_CHILD(!ray.directionSignFlags[splitAxis], ray, entries, distancesToSplitPlane);
			
			if (!TRAVERSAL_COMPLETE(ray.validity))
			{
				TRAVERSE_CHILD(ray.directionSignFlags[splitAxis], ray, distancesToSplitPlane, exits);
			}
		}		
	}
}