#include "GeometryLib.h"
#include <limits>

namespace GeometryLib
{
	using namespace std;

	Box GetBoundaries(std::vector<Triangle*> renderables)
	{
		float infinity = 999999999.0f;
		float minX = infinity, maxX = -infinity, minY = infinity, maxY = -infinity, minZ = infinity, maxZ = -infinity;
		for (unsigned int t = 0; t < renderables.size(); t++)
		{
			Triangle* renderable = renderables[t];
			minX = min(minX, min(renderable->points[0].values[0], min(renderable->points[1].values[0], renderable->points[2].values[0])));
			maxX = max(maxX, max(renderable->points[0].values[0], max(renderable->points[1].values[0], renderable->points[2].values[0])));

			minY = min(minY, min(renderable->points[0].values[1], min(renderable->points[1].values[1], renderable->points[2].values[1])));
			maxY = max(maxY, max(renderable->points[0].values[1], max(renderable->points[1].values[1], renderable->points[2].values[1])));

			minZ = min(minZ, min(renderable->points[0].values[2], min(renderable->points[1].values[2], renderable->points[2].values[2])));
			maxZ = max(maxZ, max(renderable->points[0].values[2], max(renderable->points[1].values[2], renderable->points[2].values[2])));
		}

		return Box(minX, maxX, minY, maxY, minZ, maxZ);
	}
}