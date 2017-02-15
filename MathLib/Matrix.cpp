#include "MathLib.h"
#include "Math.h"

namespace MathLib
{

CMatrix::CMatrix(float sourceValues[])
{
	for(int t = 0; t < 16; t++)
	{
		values[t] = sourceValues[t];
	}
}

CMatrix::CMatrix(C3Vector column1, C3Vector column2, C3Vector column3)
{
	ZeroMemory(values, 16 * sizeof(float));

	this->values[0] = column1.values[0];
	this->values[4] = column1.values[1];
	this->values[8] = column1.values[2];

	this->values[1] = column2.values[0];
	this->values[5] = column2.values[1];
	this->values[9] = column2.values[2];

	this->values[2] = column3.values[0];
	this->values[6] = column3.values[1];
	this->values[10] = column3.values[2];
}

CMatrix::CMatrix(C4Vector& column1, C4Vector& column2, C4Vector& column3)
{
	ZeroMemory(values, 16 * sizeof(float));

	this->values[0] = column1.values[0];
	this->values[4] = column1.values[1];
	this->values[8] = column1.values[2];

	this->values[1] = column2.values[0];
	this->values[5] = column2.values[1];
	this->values[9] = column2.values[2];

	this->values[2] = column3.values[0];
	this->values[6] = column3.values[1];
	this->values[10] = column3.values[2];
}

CMatrix CMatrix::operator *(const CMatrix& multByMatrix) const
{
	float resultVals[16];
	for(int t = 0; t < 16; t++)
	{
		//so, multiply the t % 4 column of two by the t / 4 column of matrix one. SPEED OVER ELEGANCE!
		int row = t / 4;
		int column = t % 4;
		//so add the products of these rows and columns to get the result
		int startMat1 = row * 4;
		int startMat2 = column;
		resultVals[t] = values[startMat1] * multByMatrix.values[startMat2] +
					values[startMat1+1] * multByMatrix.values[startMat2+4] +
					values[startMat1+2] * multByMatrix.values[startMat2+8] +
					values[startMat1+3] * multByMatrix.values[startMat2+12];
	}
	return CMatrix(resultVals);
}

CMatrix CMatrix::operator=(const CMatrix& multMatrix)
{
	//memcpy doesn't work, I wonder what this will do... :-)
	for(int t = 0; t < 16; t++)
		values[t] = multMatrix.values[t];
	return *this;
}

void CMatrix::MakeYRotationMatrix(float yAngle)
{
	//fill in with a matrix to rotate by the angle
	values[0] = cos(yAngle);
	values[1] = 0;
	values[2] = -sin(yAngle);
	values[3] = 0;

	values[4] = 0;
	values[5] = 1;
	values[6] = 0;
	values[7] = 0;

	values[8] = sin(yAngle);
	values[9] = 0;
	values[10] = cos(yAngle);
	values[11] = 0;
	
	values[12] = 0;
	values[13] = 0;
	values[14] = 0;
	values[15] = 1;
}

void CMatrix::MakeXRotationMatrix(float xAngle)
{
	//fill in with a matrix to rotate by the angle
	values[0] = 1;
	values[1] = 0;
	values[2] = 0;
	values[3] = 0;

	values[4] = 0;
	values[5] = cos(xAngle);
	values[6] = sin(xAngle);
	values[7] = 0;

	values[8] = 0;
	values[9] = -sin(xAngle);
	values[10] = cos(xAngle);
	values[11] = 0;
	
	values[12] = 0;
	values[13] = 0;
	values[14] = 0;
	values[15] = 1;

}

void CMatrix::MakeZRotationMatrix(float zAngle)
{
	//fill in with a matrix to rotate by the angle
	values[0] = cos(zAngle);
	values[1] = sin(zAngle);
	values[2] = 0;
	values[3] = 0;

	values[4] = -sin(zAngle);
	values[5] = cos(zAngle);
	values[6] = 0;
	values[7] = 0;

	values[8] = 0;
	values[9] = 0;
	values[10] = 1;
	values[11] = 0;
	
	values[12] = 0;
	values[13] = 0;
	values[14] = 0;
	values[15] = 1;

}

void CMatrix::MakeRotationMatrix(float xAngle, float yAngle, float zAngle)
{
	CMatrix x,y,z;
	x.MakeXRotationMatrix(xAngle);
	y.MakeYRotationMatrix(yAngle);
	z.MakeZRotationMatrix(zAngle);
	*this =  x * y * z;
}

//concatenate rotations in reverse order
void CMatrix::MakeInverselyConcatenatedRotationMatrix(float xAngle, float yAngle, float zAngle)
{
	CMatrix x,y,z;
	x.MakeXRotationMatrix(xAngle);
	y.MakeYRotationMatrix(yAngle);
	z.MakeZRotationMatrix(zAngle);
	*this =  z * y * x;
}

void CMatrix::TransformVertex(C4Vector* targetVertex)
{

	C4Vector copyVec(targetVertex);
	//multiply the vertex by the matrix
	targetVertex->values[0] = values[0]  * copyVec.values[0] +
					  values[4]  * copyVec.values[1] +
					  values[8]  * copyVec.values[2] +
					  values[12]  * copyVec.values[3];

	targetVertex->values[1] = values[1]  * copyVec.values[0] +
					  values[5]  * copyVec.values[1] +
					  values[9]  * copyVec.values[2] +
					  values[13]  * copyVec.values[3];

	targetVertex->values[2] = values[2]  * copyVec.values[0] +
					  values[6]  * copyVec.values[1] +
					  values[10] * copyVec.values[2] +
				   	  values[14] * copyVec.values[3];

	targetVertex->values[3] = values[3] * copyVec.values[0] +
					  values[7] * copyVec.values[1] +
					  values[11] * copyVec.values[2] +
					  values[15] * copyVec.values[3];
}
void CMatrix::MakeProjectionMatrix(float fov, float screenWidth, float screenHeight)
{
	float aspectRatio = (float)screenWidth / (float)screenHeight;
	ZeroMemory(values, sizeof(float) * 16);
	values[0] = (1 / (tan(fov/2) * aspectRatio))* (screenWidth / 2);
	values[5] = (1/ (tan(fov/2)) * (screenHeight / 2));
	values[10] = 1;
	values[11] = 1;
}

void CMatrix::Transpose()
{
	CMatrix copy = *this;
	float* v = copy.values;
	for (int column = 0; column < 4; column++)
	{
		for (int row = 0; row < 4; row++)
		{
			this->SetValueAt(column, row, *(v++));
		}
	}
}

float CMatrix::ValueAt(int column, int row)
{
	return this->values[row * 4 + column];
}

void CMatrix::SetValueAt(int column, int row, float value)
{
	this->values[row * 4 + column] = value;
}

void CMatrix::MakeTranslationMatrix(float x, float y, float z)
{
	(values[0]) = 1;
	(values[1]) = 0;
	(values[2]) = 0;
	(values[3]) = 0;

	(values[4]) = 0;
	(values[5]) = 1;
	(values[6]) = 0;
	(values[7]) = 0;

	(values[8])  = 0;
	(values[9])  = 0;
	(values[10]) = 1;
	(values[11]) = 0;

	(values[12]) = x;
	(values[13]) = y;
	(values[14]) = z;
	(values[15]) = 1;
}

void CMatrix::MakeLookatMatrix(C3Vector cameraLocation, C3Vector lookatPoint)
{
	float xDiff = cameraLocation.values[0] - lookatPoint.values[0];
	float yDiff = cameraLocation.values[1] - lookatPoint.values[1];
	float zDiff = cameraLocation.values[2] - lookatPoint.values[2];

	CMatrix translationMatrix;
	translationMatrix.MakeTranslationMatrix(-cameraLocation.values[0], -cameraLocation.values[1], - cameraLocation.values[2]);
	float yAngle = atan(xDiff/zDiff);
	if(cameraLocation.values[2] > lookatPoint.values[2])	//if the camera is further away than the point it looks at, be sure to add half a rotation
		yAngle += 3.14159f;		

	//calculate x angle after rotation by y
	CMatrix yRot;
	yRot.MakeYRotationMatrix(-yAngle);
	CMatrix temp = translationMatrix * yRot;
	lookatPoint = (translationMatrix * yRot) * lookatPoint;
	float xAngle;
	if(lookatPoint.values[2])
		xAngle = atan((-lookatPoint.values[1])/(-lookatPoint.values[2]));
	else
	{
		xAngle = 0;
	}

	CMatrix rotationalMatrix;
	rotationalMatrix.MakeInverselyConcatenatedRotationMatrix(xAngle, -yAngle, 0);
	C3Vector camTargetVec = cameraLocation - lookatPoint;


	//*+*-*?
	*this =  translationMatrix * rotationalMatrix;
}

void CMatrix::MakePointAtMatrix(C4Vector& location, C4Vector& lookAt)
{
	float zAngle = atan((lookAt.values[0] - location.values[0]) / (lookAt.values[1] - location.values[1]));
	float xAngle = atan((lookAt.values[2] - location.values[2]) / (lookAt.values[1] - location.values[1]));
	CMatrix z, y, x;
	z.MakeZRotationMatrix(-zAngle);
	x.MakeXRotationMatrix(xAngle);
	*this = x * z;
}

C4Vector CMatrix::operator *(C4Vector& vector)
{
	C4Vector result;
	//multiply the vertex by the matrix
	result.values[0] = values[0]  * vector.values[0] +
					  values[4]  * vector.values[1] +
					  values[8]  * vector.values[2] +
					  values[12]  * vector.values[3];

	result.values[1] = values[1]  * vector.values[0] +
					  values[5]  * vector.values[1] +
					  values[9]  * vector.values[2] +
					  values[13]  * vector.values[3];

	result.values[2] = values[2]  * vector.values[0] +
					  values[6]  * vector.values[1] +
					  values[10] * vector.values[2] +
				   	  values[14] * vector.values[3];

	result.values[3] = values[3] * vector.values[0] +
					  values[7] * vector.values[1] +
					  values[11] * vector.values[2] +
					  values[15] * vector.values[3];
	return result;
}

//TODO WARNING - Incomplete multiplication, misses final w component.
void CMatrix::PartialMultiply(C4Vector& vector)
{
	float a = values[0]  * vector.values[0] +
					  values[4]  * vector.values[1] +
					  values[8]  * vector.values[2] +
					  values[12]  * vector.values[3];

	float b = values[1]  * vector.values[0] +
					  values[5]  * vector.values[1] +
					  values[9]  * vector.values[2] +
					  values[13]  * vector.values[3];

	float c = values[2]  * vector.values[0] +
					  values[6]  * vector.values[1] +
					  values[10] * vector.values[2] +
				   	  values[14] * vector.values[3];

	vector.values[0] = a;
	vector.values[1] = b;
	vector.values[2] = c;
}

C3Vector CMatrix::operator *(C3Vector& vector)
{
	C3Vector result;
	//multiply the vertex by the matrix
	result.values[0] = values[0]  * vector.values[0] +
					  values[4]  * vector.values[1] +
					  values[8]  * vector.values[2] +
					  values[12];

	result.values[1] = values[1]  * vector.values[0] +
					  values[5]  * vector.values[1] +
					  values[9]  * vector.values[2] +
					  values[13];

	result.values[2] = values[2]  * vector.values[0] +
					  values[6]  * vector.values[1] +
					  values[10] * vector.values[2] +
				   	  values[14];
	return result;
}



C4Vector CMatrix::operator /(C4Vector& vector)
{
	C4Vector result;
	result.values[0] = -((-vector.values[0] + (vector.values[1] * values[4]) + (vector.values[2] *values[8]) + (vector.values[3] * values[12]))/values[0]);
	result.values[1] =  -(((vector.values[0] * values[1]) - vector.values[1] + (vector.values[2] * values[9]) + (vector.values[3] * values[13]))/values[5]);
	result.values[2] = -(((vector.values[0] * values[2]) + (vector.values[1] * values[6]) - (vector.values[2]) + (vector.values[3] * values[14]))/ values[10]);
	result.values[3] = 1;
	return result;
}

void CMatrix::Invert()
{
	values[0] = 1.0f / values[0];
//	values[4] = 1 / values[4];
//	values[8] = 1 / values[8];

	values[5] = 1.0f / values[5];
	//values[9] = 1 / values[9];
//	values[13] = 1 / values[13];

}

}