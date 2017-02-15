#pragma once
#include "Windows.h"
#include "emmintrin.h"

namespace MathLib
{

#define INFINITY 0xfffffffffffffffe
#define EPSILON 0.0002f
#define ONE_PLUS_EPSILON 1.0002f
#define ONE_MINUS_EPSILON -0.0002f

#pragma region C3Vector
class C3Vector
{
	#pragma region Public
	public:

		#pragma region Constructors
		C3Vector();
		C3Vector(float x, float y, float z){values[0] = x; values[1] = y; values[2] = z;}
		C3Vector(C3Vector* sourceVec);
		#pragma endregion

		#pragma region OperatorOverloads
		C3Vector operator*( float multBy);
		float operator*(C3Vector& multByVector);
		C3Vector operator-(const C3Vector& subtractVec) const;
		C3Vector operator+(const C3Vector& addVec) const;
		C3Vector operator/(float divideBy) const;
		C3Vector& operator=(const C3Vector& vector);
		#pragma endregion

		#pragma region Methods
		C3Vector CrossProduct(C3Vector* crossWith);

		C3Vector Reflect(C3Vector* surfaceNormal);

		int ToRGBColour(){return (int)values[0] << 16 | (int)values[1] << 8 | (int)values[2];}
		void ClampToRGBRange();
		
		void Boundaries(float values[6]);
		float Length();
		void Normalise();
		#pragma endregion
	#pragma endregion

	#pragma region Private
	// private:
		float values[3];
	#pragma endregion
};
#pragma endregion

#pragma region Ray
class Ray
{
public:
	Ray(C3Vector origin, C3Vector direction, float magnitude);

// private:
	C3Vector origin, direction, reciprocalDirection;
	int directionSignFlags[3]; // Sign of each direction = (direction < 0) ? 0 : 1
	float magnitude;
	__m128d rayOriginComponents[3], rayDirectionComponents[3];
};
#pragma endregion

class C4Vector
	{
	public:

		C4Vector(){ZeroMemory(this->values, sizeof(float) * 4);}
		C4Vector(C4Vector* sourceVec);
		C4Vector(C3Vector* sourceVec);
		C4Vector(float x, float y, float z){values[0] = x; values[1] = y, values[2] = z; values[3] = 1;}
		void Normalise();
		C4Vector operator-(const C4Vector& subtractVec) const;
		C4Vector operator-(const C3Vector& subtractVec) const;
		C4Vector operator-(const float subtract) const;
		C4Vector operator+(const float add) const;
		C4Vector operator-() const;
		C4Vector operator+(const C4Vector& addVec) const;
		C4Vector operator/(float divideBy) const;
		//C4Vector operator/(C4Vector& divideBy) const;
		//void operator=(const C4Vector& vector);
		void operator=(const C4Vector& vector);
		C4Vector operator=(const C3Vector& vector);
		float operator*(const C4Vector& multByVector) const;
		float operator*(C3Vector& multByVector);
		C4Vector operator*(float multBy) const;
		C4Vector crossProduct(C4Vector* crossWith);
		C4Vector Reflect(C4Vector* surfaceNormal) const;

		float Length();
		void ClipValuesToRange();
		float values[4];

	};



	class CMatrix
	{
	public:
		CMatrix(){ZeroMemory(values, 64);}
		CMatrix(float values[16]);
		//CMatrix(CVector translationVector);
		CMatrix operator*(const CMatrix& multMatrix) const;
		CMatrix operator=(const CMatrix& multMatrix);
		CMatrix(C3Vector column1, C3Vector column2, C3Vector column3);
		CMatrix(C4Vector& column1, C4Vector& column2, C4Vector& column3);

		void MakeYRotationMatrix(float yAngle);
		void MakeXRotationMatrix(float xAngle);
		void MakeZRotationMatrix(float zAngle);
		void MakeRotationMatrix(float xAngle, float yAngle, float zAngle);
		void MakeInverselyConcatenatedRotationMatrix(float xAngle, float yAngle, float zAngle);

		float ValueAt(int column, int row);
		void SetValueAt(int column, int row, float value);

		void MakeProjectionMatrix(float fov, float screenWidth, float screenHeight);
		void MakeTranslationMatrix(float x, float y, float z);
		void Transpose();
		void MakeLookatMatrix(C3Vector cameraLocation, C3Vector lookatPoint);
		void MakePointAtMatrix(C4Vector& location, C4Vector& lookAt);
		//to reverse a vertex transformation, either invert() matrix and transform, or use / operator
		void Invert();
		C4Vector operator/(C4Vector& vector);	//inverse transformation of vertex

		void PartialMultiply(C4Vector& vector);

		C4Vector operator*(C4Vector& vector);

		C3Vector operator*(C3Vector& vector);
		void TransformVertex(C4Vector* vector);
		float values[16];

	};
}