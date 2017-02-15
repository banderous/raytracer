#include "MathLib.h"
#include "math.h"

namespace MathLib
{

	C3Vector::C3Vector()
	{
		this->values[0] = 0;
		this->values[1] = 0;
		this->values[2] = 0;
	}

	C3Vector::C3Vector(C3Vector* sourceVec)
	{
		values[0] = sourceVec->values[0];
		values[1] = sourceVec->values[1];
		values[2] = sourceVec->values[2];
	}

	C3Vector C3Vector::operator -(const C3Vector& subtractVec) const
	{
		C3Vector result;
		result.values[0] = values[0] - subtractVec.values[0];
		result.values[1] = values[1] - subtractVec.values[1];
		result.values[2] = values[2] - subtractVec.values[2];
		//assuming we ignore the w component
		return result;
	}

	C3Vector C3Vector::operator +(const C3Vector& addVec) const
	{
		C3Vector result;
		result.values[0] = values[0] + addVec.values[0];
		result.values[1] = values[1] + addVec.values[1];
		result.values[2] = values[2] + addVec.values[2];
		//assuming we ignore the w component
		return result;
	}

	C3Vector C3Vector::operator /(float divideBy) const
	{
		C3Vector result;
		result.values[0] = values[0] / divideBy;
		result.values[1] = values[1] / divideBy;
		result.values[2] = values[2] / divideBy;
		//assuming we ignore the w component
		return result;
	}

	C3Vector& C3Vector::operator =(const C3Vector& assign)
	{
		values[0] = assign.values[0];
		values[1] = assign.values[1];
		values[2] = assign.values[2];
		return *this;
	}


	float C3Vector::operator *(C3Vector& multByVector)
	{
		return values[0] * multByVector.values[0] + values[1] * multByVector.values[1] + values[2] * multByVector.values[2];
	}

	C3Vector C3Vector::operator *(float multBy)
	{
		C3Vector vector;
		vector.values[0] = values[0] * multBy;
		vector.values[1] = values[1] * multBy;
		vector.values[2] = values[2] * multBy;
		return vector;
	}

	C3Vector C3Vector::CrossProduct(C3Vector* crossWith)
	{
		C3Vector result;
		result.values[0] = values[1] * crossWith->values[2] - values[2] * crossWith->values[1];
		result.values[1] = values[2] * crossWith->values[0] - values[0] * crossWith->values[2];
		result.values[2] = values[0] * crossWith->values[1] - values[1] * crossWith->values[0];
		return result;
	}

	C3Vector C3Vector::Reflect(C3Vector* surfaceNormal)
	{
		float angle = *surfaceNormal * *this * 2;
		return *this - (*surfaceNormal * angle);
	}

	void C3Vector::Boundaries(float values[6])
	{
		values[0] = this->values[0];
		values[1] = this->values[0];

		values[2] = this->values[1];
		values[3] = this->values[1];

		values[4] = this->values[2];
		values[5] = this->values[2];
	}

	float C3Vector::Length()
	{
		return sqrt(values[0] * values[0] + values[1] * values[1] + values[2] * values[2]);
	}

	void C3Vector::Normalise()
	{
		float reciprocallength = 1L / this->Length();
		values[0] *= reciprocallength;
		values[1] *= reciprocallength;
		values[2] *= reciprocallength;
	}
}