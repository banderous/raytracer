#include "MathLib.h"
#include "Math.h"

namespace MathLib
{

void C4Vector::Normalise()
{
	float length = sqrt(values[0] * values[0] + values[1] * values[1] + values[2] * values[2]);
	values[0] /= length;
	values[1] /= length;
	values[2] /= length;
}

float C4Vector::Length()
{
	float squaredLength = (values[0] * values[0]) + (values[1] * values[1]) + (values[2] * values[2]);
	_asm
	{
		sqrtss xmm0, squaredLength
		movss squaredLength, xmm0
	}
	return squaredLength;
}


C4Vector::C4Vector(C4Vector* sourceVec)
{
	values[0] = sourceVec->values[0];
	values[1] = sourceVec->values[1];
	values[2] = sourceVec->values[2];
	values[3] = sourceVec->values[3];
}

C4Vector::C4Vector(C3Vector* sourceVec)
{
	sourceVec = (C3Vector*) sourceVec;
	values[0] = sourceVec->values[0];
	values[1] = sourceVec->values[1];
	values[2] = sourceVec->values[2];
	values[3] = 1;
}

C4Vector C4Vector::operator -(const C4Vector& subtractVec) const
{
	C4Vector result;
	result.values[0] = values[0] - subtractVec.values[0];
	result.values[1] = values[1] - subtractVec.values[1];
	result.values[2] = values[2] - subtractVec.values[2];
	//assuming we ignore the w component
	return result;
}

C4Vector C4Vector::operator -(const C3Vector& subtractVec) const
{
	C4Vector result;
	result.values[0] = values[0] - subtractVec.values[0];
	result.values[1] = values[1] - subtractVec.values[1];
	result.values[2] = values[2] - subtractVec.values[2];
	//assuming we ignore the w component
	return result;
}

C4Vector C4Vector::operator -(const float subtract) const
{
	return C4Vector(values[0] - subtract, values[1] - subtract, values[2] - subtract);
}

C4Vector C4Vector::operator +(const float add) const
{
	return C4Vector(values[0] + add, values[1] + add, values[2] + add);
}

C4Vector C4Vector::operator-() const
{
	return C4Vector(-this->values[0], -this->values[1], -this->values[2]);
}

C4Vector C4Vector::operator +(const C4Vector& addVec) const
{
	C4Vector result;
	result.values[0] = values[0] + addVec.values[0];
	result.values[1] = values[1] + addVec.values[1];
	result.values[2] = values[2] + addVec.values[2];
	//assuming we ignore the w component
	return result;
}

C4Vector C4Vector::operator /(float divideBy) const
{
	C4Vector result;
	result.values[0] = values[0] / divideBy;
	result.values[1] = values[1] / divideBy;
	result.values[2] = values[2] / divideBy;
	//assuming we ignore the w component
	return result;
}

/*
C4Vector C4Vector::operator /(C4Vector& divideBy) const
{
	C4Vector result;
	result.values[0] = values[0] / divideBy.values[0];
	result.values[1] = values[1] / divideBy.values[1];
	result.values[2] = values[2] / divideBy.values[2];
	//assuming we ignore the w component
	return result;
}
*/

void C4Vector::operator =(const C4Vector& assign)
{
	//memcpy(values, assign.values, 16); //4 16 bit floats TODO WARNING HACK
	values[0] = assign.values[0];
	values[1] = assign.values[1];
	values[2] = assign.values[2];
	values[3] = assign.values[3];

//	return *this;
}

/*
void C4Vector::operator=(const C4Vector& vector)
{
	values[0] = vector.values[0];
	values[1] = vector.values[1];
	values[2] = vector.values[2];
}
*/

C4Vector C4Vector::operator =(const C3Vector& assign)
{
	values[0] = assign.values[0];
	values[1] = assign.values[1];
	values[2] = assign.values[2];

	return *this;
}

C4Vector C4Vector::Reflect(C4Vector* surfaceNormal) const
{
	float angle = *surfaceNormal * *this * 2;
	return *this - (*surfaceNormal * angle);
}


float C4Vector::operator *(C3Vector& multByVector)
{
	return values[0] * multByVector.values[0] + values[1] * multByVector.values[1] + values[2] * multByVector.values[2];
}

C4Vector C4Vector::operator *(float multBy) const
{
	C4Vector vector;
	vector.values[0] = values[0] * multBy;
	vector.values[1] = values[1] * multBy;
	vector.values[2] = values[2] * multBy;
	return vector;
}


C4Vector C4Vector::crossProduct(C4Vector* crossWith)
{
	C4Vector result;
	result.values[0] = values[1] * crossWith->values[2] - values[2] * crossWith->values[1];
    result.values[1] = values[2] * crossWith->values[0] - values[0] * crossWith->values[2];
    result.values[2] = values[0] * crossWith->values[1] - values[1] * crossWith->values[0];
	result.values[3] = 1;
	return result;
}

void C4Vector::ClipValuesToRange()
{
	for(int t = 0; t < 4; t++)
	{
		if(values[t] < 0)
			values[t] = 0;
		if(values[t] > 1.0f)
			values[t] = 1.0f;
	}
}

float C4Vector::operator *(const C4Vector& multByVector) const
{
	//return values[0] * multByVector.values[0] + values[1] * multByVector.values[1] + values[2] * multByVector.values[2];
	return  multByVector.values[0] * values[0] + multByVector.values[1] * values[1] + multByVector.values[2] * values[2];
}

}