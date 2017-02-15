#pragma once

#include <string>
#include "..\Interfaces\ISerialisable.h"
#include "..\Camera\Camera.h"

namespace SceneLoader
{
	using namespace std;

	class Scene
	{
		public:
			Scene(string sceneFile);

			// Members
			CameraLib::Camera* camera;
			string plyLocation, kdLocation;
			int resX, resY;
	};
}