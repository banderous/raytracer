#include "Camera.h"

namespace CameraLib
{

	//Much fiddling with coordinate systems.
	//Ultimately the camera produces rays according to the various orientation and film parameters.
	//Rays are initially defined along the coordinate system {-1,0,0}{0,1,0}{0,0,1}. We need to convert to the camera coordinate system.
	//I think there is some wierdness with handedness here, since we need to take the transpose of the transformation matrix
	//in order to end up with a ray in world coordinates. Thus unlike tangent space stuff, there are 3 transforms involved.
	//Tangent is just tangent to world, whilst this goes local ray coordinates to camera to world. Hence the transpose.
	//Additionally, the X axis of the ray needs to be negated since rays are defined according to the afforementioned system, to get them to the world space.
	//Hence the multiplied matrices.
	//READ - We initally have the camera basis vectors in world space, converting from world to camera. The transpose gives us
	//the world basis vectors in camera space and is what we want in order to get our ray fired from the camera.
	Camera::Camera(C4Vector origin, C4Vector target, C4Vector upVector, float filmX, float filmY)
	{
		this->Initialise(origin, target, upVector, filmX, filmY);
	}

	void Camera::Initialise(C4Vector origin, C4Vector target, C4Vector upVector, float filmX, float filmY)
	{
		this->origin = origin;
		this->rayOrigin = origin;
		this->target = target;
		this->upVector = upVector;
		
		this->filmX = filmX;
		this->filmY = filmY;
		this->aspectRatio = filmX / (float) filmY;

		halfFilmX = filmX / 2.0f;
		halfFilmY = filmY / 2.0f;
		
		maxX = halfFilmX;
		minX = -halfFilmX;
		maxY = halfFilmY;
		minY = -halfFilmY;

		C4Vector lookAt = target - origin;
		lookAt.Normalise();
		C4Vector rightVector = lookAt.crossProduct(&upVector);

		this->rayTransform = CMatrix(rightVector, upVector, lookAt);
		rayTransform.Transpose(); //if we do not transpose the matrix, then any ray transformed will simply end up as the equivalent in the camera coordinate system, rather than transformed. This is equivalent to making a matrix from the transformed world basis vectors (transformed by the un transposed matrix)
		this->rayOriginTransform.MakeTranslationMatrix(origin.values[0], origin.values[1], origin.values[2]);
		this->eyePoint = origin;
	}

	void Camera::SetOrientation(float xAngle, float yAngle)
	{
		//Transform to eyePoint
		this->rayOriginTransform.MakeTranslationMatrix(eyePoint.values[0], eyePoint.values[1], eyePoint.values[2]);

		rayOrigin = C4Vector(0,0,0);
		rayOrigin = rayOriginTransform * rayOrigin;

		CMatrix yRotation, xRotation;
		yRotation.MakeYRotationMatrix(yAngle);
		xRotation.MakeXRotationMatrix(xAngle);
		
		CMatrix rot = xRotation * yRotation;
		C4Vector lookAt(0,0,-1);
		direction = rot * lookAt;
	//	rot = rot * rayOriginTransform;
		C4Vector rVec(1,0,0);
		C4Vector uVec(0,1,0);

		lookAt = rot * lookAt;
		rightVec = rot * rVec;
		upVec = rot * uVec;

		this->rayTransform = CMatrix(rightVec, upVec, lookAt);
		rayTransform.Transpose(); //if we do not transpose the matrix, then any ray transformed will simply end up as the equivalent in the camera coordinate system, rather than transformed. This is equivalent to making a matrix from the transformed world basis vectors (transformed by the un transposed matrix)

	}

	Ray Camera::GetRayColour(float x, float y)
	{
		
		//remember 0,0 of film is minimum X and maximum Y
		float xPoint = minX + x * (maxX - minX);
		float yPoint = maxY + y * (minY - maxY);

		C4Vector filmPoint(xPoint, yPoint, 1);
		// filmPoint.Normalise();
		
		rayTransform.PartialMultiply(filmPoint);
		filmPoint.Normalise();
		return Ray(C3Vector(rayOrigin.values[0], rayOrigin.values[1], rayOrigin.values[2]) , C3Vector(filmPoint.values[0], filmPoint.values[1], filmPoint.values[2]), 99999999.0f);
	}

	void Camera::UpdateRay(Ray& ray, float x, float y)
	{
		//remember 0,0 of film is minimum X and maximum Y
		float xPoint = minX + x * (maxX - minX);
		float yPoint = maxY + y * (minY - maxY);

		C4Vector filmPoint(xPoint, yPoint, 1);
		filmPoint.Normalise();
		
		rayTransform.PartialMultiply(filmPoint);

		ray.direction = C3Vector(filmPoint.values[0], filmPoint.values[1], filmPoint.values[2]);
	}

	void Camera::Deserialise(std::fstream& stream)
	{
		string in;

		stream >> in;

		if (in != "ORIGIN")
		{
			throw in;
		}

		float x, y, z;
		stream >> x;
		stream >> y;
		stream >> z;

		C4Vector origin(x, y, z);

		stream >> in;

		if (in != "TARGET")
		{
			throw in;
		}

		stream >> x;
		stream >> y;
		stream >> z;

		C4Vector target(x, y, z);

		stream >> in;

		if (in != "UP")
		{
			throw in;
		}

		stream >> x;
		stream >> y;
		stream >> z;

		C4Vector upVec(x, y, z);


		stream >> in;

		if (in != "FILMX")
		{
			throw in;
		}

		float filmX;
		stream >> filmX;

		stream >> in;

		if (in != "FILMY")
		{
			throw in;
		}

		float filmY;
		stream >> filmY;

		this->Initialise(origin, target, upVec, filmX, filmY);
	}
}