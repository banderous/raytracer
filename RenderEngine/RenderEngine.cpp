#include "RenderEngine.h"



#include <vector>
#include <iostream>
#include "windows.h"
#include "math.h"

using namespace std;
using namespace Tree;
using namespace CanvasContainer;
using namespace CameraLib;
using namespace PLYLoader;
using namespace RayGeneration;

namespace Engine
{
	static void StaticRenderFragment(RenderEngine* engine)
	{
		engine->RenderThreadStart();
	}

	RenderEngine::RenderEngine(int nbThreads)
	{
		HANDLE hProcess = GetCurrentProcess();

		SceneLoader::Scene scene("C:\\Worlds\\World.txt");

		LoadTreeFromFile(scene.kdLocation, scene.plyLocation, &tree, &boundaries);	
		triangles = PLYLoader::LoadPLY(scene.plyLocation);

		c = new Canvas("Xenya32", scene.resX, scene.resY, false);

		generator = new RayBundleGenerator(&scene);

		this->threadCount = nbThreads;
		frameStart = CreateEvent(NULL, true, false, NULL);
		frameEnd = CreateEvent(NULL, true, false, NULL);
		workerSem = CreateSemaphore(NULL, 0, threadCount, NULL);

		for (int t = 0; t < threadCount; t++)
		{
			HANDLE h = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)StaticRenderFragment, this, 0, 0); //start the thread to listen for incoming connections
			this->threadHandles.push_back(h);
		}
	}

	void RenderEngine::Render()
	{
		// c->StartRender();
		generator->currentBundleIndex = -1;

		// Close the end gate
		ResetEvent(frameEnd);

		// Open the start gate
		SetEvent(frameStart);

		// Wait for the workers to indicate they have reached the end gate
		WaitForWorkers();

		// Reset the start gate to stop the workers starting the next frame
		ResetEvent(frameStart);

		// Open the end gate
		SetEvent(frameEnd);

		// Wait for the workers to indicate they have cleared the gate
		WaitForWorkers();

		// c->FinishAndDisplayRender();
	}



	void RenderEngine::RenderThreadStart()
	{
		while (true)
		{
			WaitForSingleObject(frameStart, INFINITE);

			ThreadWork();

			ReleaseSemaphore(workerSem, 1, NULL);
			WaitForSingleObject(frameEnd, INFINITE);
			ReleaseSemaphore(workerSem, 1, NULL);
		}
	}

	void RenderEngine::WaitForWorkers()
	{
		for (int t = 0; t < threadCount; t++)
		{
			WaitForSingleObject(workerSem, INFINITE);
		}
	}

	void RenderEngine::ThreadWork()
	{
		float time = (float) timeGetTime();
		float sinMul = sinf(time / 1000.0f);
		float cosMul = cosf(time / 1000.0f);

		C3Vector light(sinMul * 50.0f, 50, cosMul * 50.0f);
		__m128 lightX = _mm_set1_ps(light.values[0]);
		__m128 lightY = _mm_set1_ps(light.values[1]);
		__m128 lightZ = _mm_set1_ps(light.values[2]);

		
		while(true)
		{
			RayBundle bundle;
			if (!generator->GetBundle(&bundle))
			{
				break;
			}

			__m128 entries, exits;
			if (boundaries->RayBundleIntersects(bundle, entries, exits))
			{
				tree->RayHits(bundle, entries, exits);

				int mask = _mm_movemask_ps(bundle.validity);
				if (mask)
				{
					__m128 intersectionsX = _mm_add_ps(bundle.rayOrigins[0], _mm_mul_ps(bundle.rayDirections[0], bundle.struckDistances));
					__m128 intersectionsY = _mm_add_ps(bundle.rayOrigins[1], _mm_mul_ps(bundle.rayDirections[1], bundle.struckDistances));
					__m128 intersectionsZ = _mm_add_ps(bundle.rayOrigins[2], _mm_mul_ps(bundle.rayDirections[2], bundle.struckDistances));

					// Now get the vector from the light to this point
					__m128 lightVecX = _mm_sub_ps(lightX, intersectionsX);
					__m128 lightVecY = _mm_sub_ps(lightY, intersectionsY);
					__m128 lightVecZ = _mm_sub_ps(lightZ, intersectionsZ);

					// Normalise by dividing by its length
					__m128 lightVecLengths = _mm_sqrt_ps(_mm_add_ps(
						_mm_add_ps(_mm_mul_ps(lightVecX, lightVecX), _mm_mul_ps(lightVecY, lightVecY)),
								_mm_mul_ps(lightVecZ, lightVecZ)));

					lightVecX = _mm_div_ps(lightVecX, lightVecLengths);
					lightVecY = _mm_div_ps(lightVecY, lightVecLengths);
					lightVecZ = _mm_div_ps(lightVecZ, lightVecLengths);

					// Now load up the surface normals for all 4 pixels
					CACHE_ALIGN float triangleIndicies[4];
					_mm_store_ps(triangleIndicies, bundle.struckIndicies);

					__m128 normalX, normalY, normalZ;
					C3Vector normalA, normalB, normalC, normalD;
					normalA = triangles[(int) triangleIndicies[0]]->trueNormal;
					normalB = triangles[(int) triangleIndicies[1]]->trueNormal;
					normalC = triangles[(int) triangleIndicies[2]]->trueNormal;
					normalD = triangles[(int) triangleIndicies[3]]->trueNormal;

					normalX = _mm_setr_ps(normalA.values[0],
						normalB.values[0],
						normalC.values[0],
						normalD.values[0]);
							
					normalY = _mm_setr_ps(normalA.values[1],
						normalB.values[1],
						normalC.values[1],
						normalD.values[1]);

					normalZ = _mm_setr_ps(normalA.values[2],
						normalB.values[2],
						normalC.values[2],
						normalD.values[2]);

					// Now take the dot product of the normalised light vector with the surface normals
					__m128 lightLevels = _mm_add_ps(_mm_add_ps(_mm_mul_ps(lightVecX, normalX), _mm_mul_ps(lightVecY, normalY)), _mm_mul_ps(lightVecZ, normalZ));
					lightLevels = _mm_max_ps(lightLevels, _mm_setzero_ps());
					__m128 colour = _mm_set1_ps(255.0f);
					__m128 shade = _mm_mul_ps(lightLevels, colour);

					CACHE_ALIGN float pixelXIndicies[4];
					_mm_store_ps(pixelXIndicies, bundle.pixelXIndicies);

					CACHE_ALIGN float pixelYIndicies[4];
					_mm_store_ps(pixelYIndicies, bundle.pixelYIndicies);

					CACHE_ALIGN float shades[4];
					_mm_store_ps(shades, shade);

					for (int pixelIndex = 0; pixelIndex < 4; pixelIndex++)
					{
						if (mask & (1 << pixelIndex))
						{
							// c->SetPixel((int) pixelXIndicies[pixelIndex], (int) pixelYIndicies[pixelIndex], ((int) shades[pixelIndex]) << 16);
						}
					}
				}
			}
		}
		
		
	}

}