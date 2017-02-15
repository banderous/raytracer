#include "SceneLoader.h"

namespace SceneLoader
{
	Scene::Scene(string sceneFile)
	{
		fstream in(sceneFile.c_str(), ios::in);
		if (!in.is_open())
		{
			throw string("Unable to open scene file for reading");
		}

		in >> this->plyLocation;
		in >> this->kdLocation;
		in >> this->resX;
		in >> this->resY;

		string next;
		in >> next;
		while (!in.eof())
		{
			if (next == "CAMERA")
			{
				this->camera = new CameraLib::Camera();
				this->camera->Deserialise(in);
			}

			in >> next;
		}
	}
}