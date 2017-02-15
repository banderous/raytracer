#include "Tree.h"

namespace Tree
{
	SerialLeaf::SerialLeaf(std::vector<Triangle*> members)
	{
		this->members = members;
	}

	Triangle* SerialLeaf::RayHits(Ray& ray, float entry, float exit, float& firstHitDistance)
	{
		float closestHit = 9999999.0f;
		Triangle* struck = NULL;

		for (unsigned int t = 0; t < members.size(); t++)
		{
			float f;
			if (members[t]->RayIntersects(ray, &f))
			{
				if (f < closestHit)
				{
					closestHit = f;
					struck = members[t];
				}
			}
		}

		firstHitDistance = closestHit;
		return struck;
	}
}