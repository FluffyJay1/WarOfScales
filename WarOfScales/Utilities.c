#include "Utilities.h"



BOOLEAN float_IsEqual(float left, float right)
{
	return ((left - right) < EPSILON && (left - right) > -EPSILON) ? TRUE : FALSE;
}
BOOLEAN double_IsEqual(double left, double right)
{
	return ((left - right) < EPSILON && (left - right) > -EPSILON) ? TRUE : FALSE;
}
int Round(double input)
{
	int flooredInput = input;

	if (input - flooredInput > 0.5)
	{
		return flooredInput + 1;
	}
	else
	{
		return flooredInput;
	}
}
int ParseInt(char* in)
{
	int sd = 0, i, out = 0;
	while (in[sd] >= 48 && in[sd] <= 57)
	{
		out *= 10;
		out += in[sd] - 48;
		sd++;
	}
	return out;
}
void Fuck(int** one, int* two)
{
	*one = two;
}