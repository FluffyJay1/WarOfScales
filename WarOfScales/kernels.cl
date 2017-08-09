#define DIAGONAL_WEIGHT 0.10355339059327376220042218105242
#define ADJACENT_WEIGHT 0.14644660940672623779957781894758

__kernel void diffuse(__global double *outputField, __global const double *inputField, __global const double *D, __global const double *lambda, __global const char *diffusable, __global const double *width, __global const double *height)
{
	int id = get_global_id(0);
	int dx, dy;
	double multiplier;
	if(diffusable[id] == 1)
	{
		outputField[id] = inputField[id];
		for(dx = -1; dx <= 1; dx++)
		{
			for(dy = -1; dy <= 1; dy++)
			{
				if((dx != 0 || dy != 0) && diffusable[id + dx + dy * (int)*width] && id % (int)*width + dx < width && id % (int)*width + dx >= 0 && id + (int)*width * dy >= 0 && id + (int)*width * dy < (int)*width * (int)*height)
				{
					if (dx * dy == 0)
					{
						multiplier = ADJACENT_WEIGHT;
					}
					else
					{
						multiplier = DIAGONAL_WEIGHT;
					}
					outputField[id] += multiplier * D[id] * (inputField[id + dx + dy * (int)*width] - inputField[id]);
				}
			}
		}
		outputField[id] *= lambda[id];
	}
}