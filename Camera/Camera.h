#if !defined(camera_h)
#define camera_h
#include "..\MathLib\MathLib.h"
#include "..\Interfaces\ISerialisable.h"
#include <fstream>
#include <string>

namespace CameraLib
{
	using namespace MathLib;
	using namespace std;

	class Camera : public Serialisation::ISerialisable
	{

	public:
		//C4Vector origin, target, upVector, rightVector;
		CMatrix rayTransform;
		CMatrix rayOriginTransform;
		C4Vector origin, upVector, target;
		C4Vector rayOrigin;


		float aspectRatio,filmY,filmX,halfFilmX,halfFilmY, maxX, minX, maxY, minY;
		C4Vector eyePoint, direction, rightVec, upVec;

		Camera(){}
		Camera(C4Vector origin, C4Vector eyePoint, C4Vector upVector, float filmX, float filmY);
		void Initialise(C4Vector origin, C4Vector target, C4Vector upVector, float filmX, float filmY);
		void Deserialise(std::fstream& stream);
		CMatrix GetRayTransform(){return this->rayTransform;}
		CMatrix GetRayOriginTransform(){return this->rayOriginTransform;}
		void SetOrientation(float xAngle, float yAngle);
		void SetFrame(int frame);
		Ray GetRayColour(float x, float y);
		void UpdateRay(Ray& ray, float x, float y);
	};

}

#endif