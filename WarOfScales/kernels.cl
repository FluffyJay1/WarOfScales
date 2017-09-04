#define DIAGONAL_WEIGHT 0.10355339059327376220042218105242
#define ADJACENT_WEIGHT 0.14644660940672623779957781894758
#define TRUE 1
#define FALSE 0
#define EPSILON 0.0001f

__kernel void setarrayboolean(__global char *booleanarray, __global const char *val)
{
	size_t id = get_global_id(0);
	booleanarray[id] = *val;
}
__kernel void setarraydouble(__global double *doublearray, __global const double *val)
{
	size_t id = get_global_id(0);
	doublearray[id] = *val;
}

__kernel void diffuse(__global double *outputField, __global const double *inputField, __global const double *D, __global const double *lambda, __global const char *diffusable, __global const double *width, __global const double *height)
{
	size_t id = get_global_id(0);
	int dx, dy;
	double multiplier;
	if(diffusable[id] == 1)
	{
		outputField[id] = inputField[id];
		for(dx = -1; dx <= 1; dx++)
		{
			for(dy = -1; dy <= 1; dy++)
			{
				if((dx != 0 || dy != 0) && diffusable[id + dx + dy * (int)*width] && id % (int)*width + dx < (int)*width && (int)id % (int)*width + dx >= 0 
				&& (int)id + (int)*width * dy >= 0 && id + (int)*width * dy < (int)*width * (int)*height)
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
//l1 is the set of lines we want to test, l2 is the line "map"
//1 work group per different l1, 1 work item per different l2
//1 output for every potential collision
//DON'T FORGET TO INITIALIZE OUTPUTS TO l1p2
//INITIALIZE OUTPUTCOLLIDES TO FALSE
__kernel void linecollision(__global char *outputcollides, __global double *outputx, __global double *outputy,
		__global double *l1p1x, __global double *l1p1y, __global double *l1p2x, __global double *l1p2y, __global double *l2p1x, __global double *l2p1y, __global double *l2p2x, __global double *l2p2y,
		__global double *l1slope, __global double *l2slope, __global double *l1bound_up, __global double *l1bound_down, __global double *l1bound_left, __global double *l1bound_right, __global double *l2bound_up, __global double *l2bound_down, __global double *l2bound_left, __global double *l2bound_right,
		/*__global char *l1p1right,*/ __global char *l1p1down, __global char *l1vertical, /*__global char *l2p1right,*/ __global char *l2p1down, __global char *l2vertical)
{
	size_t id = get_global_id(0);
	size_t l1id = get_group_id(0);
	size_t l2id = get_local_id(0);
	/*
	double xdiff = outputx[l1id] - l1p1x[l1id];
	double ydiff = outputy[l1id] - l1p1y[l1id];
	*/
	double retx;
	double rety;
	/*
	if(xdiff < 0) xdiff *= -1;
	if(ydiff < 0) ydiff *= -1;
	*/
	if (l1bound_left[l1id] > l2bound_right[l2id] || l1bound_right[l1id] < l2bound_left[l2id] || l1bound_up[l1id] > l2bound_down[l2id] || l1bound_down[l1id] < l2bound_up[l2id])
	{
		outputcollides[id] = FALSE;
		return;
	}
	if (l1vertical[l1id] && l2vertical[l2id]) //same slopes, both vertical
	{
		if (l1p1x[l1id] - l2p1x[l2id] <= EPSILON && l1p1x[l1id] - l2p1x[l2id] >= -EPSILON)
		{
			double topdiff = (l1bound_up[l1id] > l2bound_up[l2id] ? l1bound_up[l1id] : l2bound_up[l2id]) - l1p1y[l1id];
			double botdiff = (l1bound_down[l1id] < l2bound_down[l2id] ? l1bound_down[l1id] : l2bound_down[l2id]) - l1p1y[l1id];
			if(topdiff < 0) topdiff *= -1;
			if(botdiff < 0) botdiff *= -1;
			outputx[id] = l1p1x[l1id];
			//outputy[l1id] = l1bound_up[l1id] > l2bound_up[l2id] ? l1bound_up[l1id] : l2bound_up[l2id];
			if(topdiff < botdiff)
			{ 
				outputy[id] = l1bound_up[l1id] > l2bound_up[l2id] ? l1bound_up[l1id] : l2bound_up[l2id];
			}
			else
			{
				outputy[id] = l1bound_down[l1id] < l2bound_down[l2id] ? l1bound_down[l1id] : l2bound_down[l2id];
			}
			/*
			outputy[l1id] = abs((l1bound_up[l1id] > l2bound_up[l2id] ? l1bound_up[l1id] : l2bound_up[l2id]) - l1p1y[l1id])
				< abs((l1bound_down[l1id] < l2bound_down[l2id] ? l1bound_down[l1id] : l2bound_down[l2id]) - l1p1y[l1id])
				? (l1bound_up[l1id] > l2bound_up[l2id] ? l1bound_up[l1id] : l2bound_up[l2id]) : (l1bound_down[l1id] < l2bound_down[l2id] ? l1bound_down[l1id] : l2bound_down[l2id]);
				*/
			outputcollides[id] = TRUE;
			return;
		}
		outputcollides[id] = FALSE;
		return;
	}
	else if (l1vertical[l1id] == FALSE && l2vertical[l2id] == FALSE && l1slope[l1id] - l2slope[l2id] <= EPSILON && l1slope[l1id] - l2slope[l2id] >= -EPSILON) //same slopes
	{
		double interceptdiff = l1p1y[l1id] - l1slope[l1id] * l1p1x[l1id] - (l2p1y[l2id] - l2slope[l2id] * l2p1x[l2id]);
		if (interceptdiff <= EPSILON && interceptdiff >= -EPSILON) //same intercepts
		{
			double topdiff = (l1bound_up[l1id] > l2bound_up[l2id] ? l1bound_up[l1id] : l2bound_up[l2id]) - l1p1y[l1id];
			double botdiff = (l1bound_down[l1id] < l2bound_down[l2id] ? l1bound_down[l1id] : l2bound_down[l2id]) - l1p1y[l1id];
			if (topdiff < 0) topdiff *= -1;
			if (botdiff < 0) botdiff *= -1;
			if (topdiff < botdiff)
			{
				if (l1bound_up[l1id] > l2bound_up[l2id])
				{
					if (l1p1down[l1id])
					{
						outputx[id] = l1p2x[l1id];
						outputy[id] = l1p2y[l1id];
					}
					else
					{
						outputx[id] = l1p1x[l1id];
						outputy[id] = l1p1y[l1id];
					}
				}
				else
				{
					if (l2p1down[l2id])
					{
						outputx[id] = l2p2x[l2id];
						outputy[id] = l2p2y[l2id];
					}
					else
					{
						outputx[id] = l2p1x[l2id];
						outputy[id] = l2p1y[l2id];
					}
				}
			}
			else
			{
				if (l1bound_down[l1id] < l2bound_down[l2id])
				{
					if (l1p1down[l1id])
					{
						outputx[id] = l1p1x[l1id];
						outputy[id] = l1p1y[l1id];
					}
					else
					{
						outputx[id] = l1p2x[l1id];
						outputy[id] = l1p2y[l1id];
					}
				}
				else
				{
					if (l2p1down[l2id])
					{
						outputx[id] = l2p1x[l2id];
						outputy[id] = l2p1y[l2id];
					}
					else
					{
						outputx[id] = l2p2x[l2id];
						outputy[id] = l2p2y[l2id];
					}
				}
			}
			outputcollides[id] = TRUE;
			return;
		}
		outputcollides[id] = FALSE;
		return;
	}
	if (l1vertical[l1id])
	{
		retx = l1p1x[l1id];
		rety = l2slope[l2id] * (retx - l2p1x[l2id]) + l2p1y[l2id];
	}
	else if(l2vertical[l2id])
	{
		retx = l2p1x[l2id];
		rety = l1slope[l1id] * (retx - l1p1x[l1id]) + l1p1y[l1id];
	}
	else
	{
		retx = (l2p1y[l2id] + l1slope[l1id] * l1p1x[l1id] - l1p1y[l1id] - l2slope[l2id] * l2p1x[l2id]) / (l1slope[l1id] - l2slope[l2id]);
		rety = l1slope[l1id] * (retx - l1p1x[l1id]) + l1p1y[l1id];
	}
	if (retx >= l1bound_left[l1id] && retx >= l2bound_left[l2id] && retx <= l1bound_right[l1id] && retx <= l2bound_right[l2id]
		&& rety >= l1bound_up[l1id] && rety >= l2bound_up[l2id] && rety <= l1bound_down[l1id] && rety <= l2bound_down[l2id])
	{
		outputx[id] = retx;
		outputy[id] = rety;
		outputcollides[id] = TRUE;
		return;
	}
	outputcollides[id] = FALSE;
}