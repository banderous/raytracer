#pragma once

#include "..\MathLib\MathLib.h"
#include "..\Interfaces\RayBundle.h"
#include "..\SceneLoader\SceneLoader.h"
#include "..\Camera\Camera.h"
#include "emmintrin.h"

#define CACHE_ALIGN __declspec(align(16))

using namespace SceneLoader;
using namespace CameraLib;
using namespace std;

namespace RayGeneration
{
	class RayBundleGenerator
	{
		public:
			void* operator new(size_t size);
			void operator delete(void* dealloc);

			RayBundleGenerator(Scene* scene);
			bool GetBundle(RayBundle* bundle);

			int currentBundleIndex, nbBundles, x, y, nbBundlesX, nbBundlesY;
			CACHE_ALIGN __m128 bundleOriginX, bundleOriginY, bundleOriginZ, xIncrement, yIncrement;

			__m128 rayOriginX, rayOriginY, rayOriginZ;
			__m128 transformMatrix[9];
			float bundleWidth, bundleHeight;
			Camera* camera;
			Scene* scene;

	};
}
