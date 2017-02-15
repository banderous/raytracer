#include "RayBundleGenerator.h"

namespace RayGeneration
{
	RayBundleGenerator::RayBundleGenerator(Scene *scene)
	{
		// TODO as params
		float filmWidth = 1.333f;
		float filmHeight = 1.0f;

		this->camera = scene->camera;
		float xMul = filmWidth / (float) scene->resX;
		float yMul = filmHeight / (float) scene->resY;
		this->currentBundleIndex = -1;
		this->nbBundles = (scene->resX * scene->resY) / 4; // Each bundle contains 4 rays
		this->scene = scene;
		this->nbBundlesX = scene->resX / 2;
		this->bundleWidth = xMul * 2.0f;
		this->bundleHeight = yMul * 2.0f;

		CACHE_ALIGN float bundleFilmX[4];
		CACHE_ALIGN float bundleFilmY[4];
		CACHE_ALIGN float bundleFilmZ[4];

		float z = 1;
		int rayIndex = 0;
		for (int x = 0; x < 2; x++)
		{
			for (int y = 0; y < 2; y++)
			{
				float fx = x * xMul;
				float fy = y * yMul;

				// Top left is the origin
				bundleFilmX[rayIndex] = (filmWidth / -2.0f) + fx;
				bundleFilmY[rayIndex] = (filmHeight / 2.0f) - fy;
				bundleFilmZ[rayIndex] = z;

				rayIndex++;
			}
		}

		this->bundleOriginX = _mm_load_ps(bundleFilmX);
		this->bundleOriginY = _mm_load_ps(bundleFilmY);
		this->bundleOriginZ = _mm_load_ps(bundleFilmZ);

		this->rayOriginX = _mm_set1_ps(scene->camera->origin.values[0]);
		this->rayOriginY = _mm_set1_ps(scene->camera->origin.values[1]);
		this->rayOriginZ = _mm_set1_ps(scene->camera->origin.values[2]);

		this->xIncrement = _mm_set1_ps(xMul);
		this->yIncrement = _mm_set1_ps(yMul);

		this->transformMatrix[0] = _mm_set1_ps(scene->camera->rayTransform.values[0]);
		this->transformMatrix[1] = _mm_set1_ps(scene->camera->rayTransform.values[4]);
		this->transformMatrix[2] = _mm_set1_ps(scene->camera->rayTransform.values[8]);

		this->transformMatrix[3] = _mm_set1_ps(scene->camera->rayTransform.values[1]);
		this->transformMatrix[4] = _mm_set1_ps(scene->camera->rayTransform.values[5]);
		this->transformMatrix[5] = _mm_set1_ps(scene->camera->rayTransform.values[9]);

		this->transformMatrix[6] = _mm_set1_ps(scene->camera->rayTransform.values[2]);
		this->transformMatrix[7] = _mm_set1_ps(scene->camera->rayTransform.values[6]);
		this->transformMatrix[8] = _mm_set1_ps(scene->camera->rayTransform.values[10]);

	}

	bool RayBundleGenerator::GetBundle(RayBundle* bundle)
	{
		int bundleToRender = InterlockedIncrement((long*)&currentBundleIndex);

		// Nothing to render
		if (bundleToRender >= nbBundles)
		{
			return false;
		}

		int bundleIndexX = bundleToRender % this->nbBundlesX;
		int bundleIndexY = bundleToRender / this->nbBundlesX;

		float bundleXOffset = bundleIndexX * this->bundleWidth;
		float bundleYOffset = bundleIndexY * this->bundleHeight;

		__m128 bundleFilmX = _mm_add_ps(this->bundleOriginX, _mm_set1_ps(bundleXOffset));
		__m128 bundleFilmY = _mm_sub_ps(this->bundleOriginY, _mm_set1_ps(bundleYOffset));
		__m128 bundleFilmZ = this->bundleOriginZ;

		// Now to transform this film point according to the camera matrix. See the camera class for an understanding of this.

		__m128 transformedX = _mm_add_ps(_mm_mul_ps(transformMatrix[0], bundleFilmX), _mm_add_ps(_mm_mul_ps(transformMatrix[1], bundleFilmY),
														_mm_mul_ps(transformMatrix[2], bundleFilmZ)));

		__m128 transformedY = _mm_add_ps(_mm_mul_ps(transformMatrix[3], bundleFilmX), _mm_add_ps(_mm_mul_ps(transformMatrix[4], bundleFilmY),
												_mm_mul_ps(transformMatrix[5], bundleFilmZ)));

		__m128 transformedZ = _mm_add_ps(_mm_mul_ps(transformMatrix[6], bundleFilmX), _mm_add_ps(_mm_mul_ps(transformMatrix[7], bundleFilmY),
												_mm_mul_ps(transformMatrix[8], bundleFilmZ)));			  
				  

		__m128 one = _mm_set1_ps(1.0f);

		bundle->rayReciprocalDirections[0] = _mm_div_ps(one, transformedX);
		bundle->rayReciprocalDirections[1] = _mm_div_ps(one, transformedY);
		bundle->rayReciprocalDirections[2] = _mm_div_ps(one, transformedZ);

		bundle->rayOrigins[0] = this->rayOriginX;
		bundle->rayOrigins[1] = this->rayOriginY;
		bundle->rayOrigins[2] = this->rayOriginZ;

		bundle->validity = _mm_setzero_ps();
		bundle->struckIndicies = _mm_setzero_ps();
		bundle->struckDistances = _mm_set1_ps(99999999.0f);

		/*
		// For any given axis, the sign of the directions must be the same
		int maskX = _mm_movemask_ps(bundle->rayDirections[0]);
		int maskY = _mm_movemask_ps(bundle->rayDirections[1]);
		int maskZ = _mm_movemask_ps(bundle->rayDirections[2]);

		
		// If any directions are not the same then skip this bundle
		if (!((maskX == 0 || maskX == 15) && (maskY == 0 || maskY == 15) && (maskZ == 0 || maskZ == 15)))
		{
			bundle = NULL;
			return;
		}
		*/

		bundle->directionSignFlags[0] = !_mm_movemask_ps(transformedX) & 1;
		bundle->directionSignFlags[1] = !_mm_movemask_ps(transformedY) & 1;
		bundle->directionSignFlags[2] = !_mm_movemask_ps(transformedZ) & 1;

		bundle->rayDirections[0] = transformedX;
		bundle->rayDirections[1] = transformedY;
		bundle->rayDirections[2] = transformedZ;

		int topLeftX = bundleIndexX * 2;
		int topLeftY = bundleIndexY * 2;

		CACHE_ALIGN float bundleIndicies[4];
		bundleIndicies[0] = (float) topLeftX;
		bundleIndicies[1] = (float) topLeftX;
		bundleIndicies[2] = (float) topLeftX + 1;
		bundleIndicies[3] = (float) topLeftX + 1;

		bundle->pixelXIndicies = _mm_load_ps(bundleIndicies);

		bundleIndicies[0] = (float) topLeftY;
		bundleIndicies[1] = (float) topLeftY + 1;
		bundleIndicies[2] = (float) topLeftY;
		bundleIndicies[3] = (float) topLeftY + 1;
		bundle->pixelYIndicies = _mm_load_ps(bundleIndicies);

		return true;
	}

	
	void* RayBundleGenerator::operator new(size_t size)
	{
		return _aligned_malloc(size, 16);
	}

	void RayBundleGenerator::operator delete(void* address)
	{
		if (address)
		{
			_aligned_free(address);
		}
	}
}