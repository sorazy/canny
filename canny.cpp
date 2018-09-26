#include "header_files/canny.hpp"
#include "header_files/global.hpp"

using namespace std;

// =================================== Magnitude Image ==================================
// We'll start by filling in the mask values using the Gausian 1st derivative. Next, do a
// scanning convolution on the input pic matrix. This will give us the Δy and Δx matrices
// Finally, take the sqrt of the sum of Δy^2 and Δx^2 to find the magnitude.
// =================================== Magnitude Image ==================================
void magnitude_matrix(double **pic, double **mag, double **x, double **y)
{
	int dim = 6 * sig + 1, cent = dim / 2;
	double maskx[dim][dim], masky[dim][dim];

	// Use the Gausian 1st derivative formula to fill in the mask values
	for (int p = -cent; p <= cent; p++)
	{	
		for (int q = -cent; q <= cent; q++)
		{
			maskx[p+cent][q+cent] = q * exp(-1 * ((p * p + q * q) / (2 * sig * sig)));
			masky[p+cent][q+cent] = p * exp(-1 * ((p * p + q * q) / (2 * sig * sig)));
		}
	}

	// Scanning convolution
	double sumx, sumy;
	for (int i = 0; i < height; i++)
	{ 
		for (int j = 0; j < width; j++)
		{
			sumx = 0;
			sumy = 0;

			// This is the convolution
			for (int p = -cent; p <= cent; p++)
			{
				for (int q = -cent; q <= cent; q++)
				{
					if ((i+p) < 0 || (j+q) < 0 || (i+p) >= height || (j+q) >= width)
						continue;
					
					sumx += pic[i+p][j+q] * maskx[p+cent][q+cent];
					sumy += pic[i+p][j+q] * masky[p+cent][q+cent];
				}
			}
			
			// Store convolution result in respective matrix
			x[i][j] = sumx;
			y[i][j] = sumy;
		}
	}

	// Find magnitude and maxVal, then store it in the 'mag' matrix
	double mags;
	double maxVal = 0;
	for (int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			mags = sqrt((x[i][j] * x[i][j]) + (y[i][j] * y[i][j]));

			if (mags > maxVal)
				maxVal = mags;

			mag[i][j] = mags;
		}
	}

	// Make sure all the magnitude values are between 0-255
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			mag[i][j] = mag[i][j] / maxVal * 255;

	return;
}

// ================================ Peaks Detection ================================
// The formula for slope is Δy/Δx. We have Δy and Δx from the scanning convulution
// above. We can get the slope by dividing the two. We'll store all the points that
// are greater than both it's neighbors in the direction of the slope into a vector.
// We can calculate the direction of the slope using the tan(x) function. We'll also
// store the peaks in a HashMap for O(1) searches in the recursiveDT function later.
// ================================ Peaks Detection ================================
vector<Point*> peak_detection(double **mag, HashMap *h, double **x, double **y)
{
	double slope = 0;
	vector<Point*> v;
	for (int i = 1; i < height - 1; i++)
	{
		for (int j = 1; j < width - 1; j++)
		{
			// To avoid dividing by zero
			if (x[i][j] == 0)
				x[i][j] = 0.0001;

			slope = y[i][j] / x[i][j];

			// We're only looking for the peaks. If we're at a peak, store 255 in 'peaks'
			if (slope <= tan(22.5) && slope > tan(-22.5))
			{
				if (mag[i][j] > mag[i][j-1] && mag[i][j] > mag[i][j+1])
				{
					v.push_back(new Point(i, j));
					h->insert(i, j);
				}
			}
			else if (slope <= tan(67.5) && slope > tan(22.5))
			{
				if (mag[i][j] > mag[i-1][j-1] && mag[i][j] > mag[i+1][j+1])
				{
					v.push_back(new Point(i, j));
					h->insert(i, j);
				}
			}
			else if (slope <= tan(-22.5) && slope > tan(-67.5))
			{
				if (mag[i][j] > mag[i+1][j-1] && mag[i][j] > mag[i-1][j+1])
				{
					v.push_back(new Point(i, j));
					h->insert(i, j);
				}
			}
			else
			{
				if (mag[i][j] > mag[i-1][j] && mag[i][j] > mag[i+1][j])
				{
					v.push_back(new Point(i, j));
					h->insert(i, j);
				}
			}
		}
	}

	return v;
}

// ======================== Hysteresis & Double Thresholding ========================
// The points passed into this function are coming from the peaks vector. We'll start
// by searching around the current pixel for a pixel that made it to "final". If
// found, then we'll recursively search for a "series" of pixels that are in the mid
// range and swith all those to ON in final. We'll stop as soon as all the pixels are
// either already processed or less than the 'lo' threshold.
// ======================== Hysteresis & Double Thresholding ========================
void recursiveDT(double **mag, double **final, HashMap *h, HashMap *peaks, int a, int b, int flag)
{
	// If the pixel value is < lo, out-of-bounds, or at a point we've visited before,
	// then exit the funciton.
	if (mag[a][b] < lo || a < 0 || b < 0 || a >= height || b >= width)
		return;
	if (h->contains(a, b))
		return;

	// Insert the current pixel so we know we've been here before.
	h->insert(a, b);

	// If flag = 0, that means that this is the first pixel of the "series" that
	// we're looking at. We're going to look for a pixel in "final" that's set to
	// ON. If we found one, assert the flag and break out of the loops.
	if (!flag)
	{
		for (int p = -1; p <= 1; p++)
		{
			for (int q = -1; q <= 1; q++)
			{
				if (final[a+p][b+q] == 255)
				{
					final[a][b] = 255;
					flag = 1;
					break;
				}
			}

			if (flag)
				break;
		}
	}
	
	// If flag is asserted, that means we found a pixel that's in final, all what
	// we have to do now is just search for pixels that are in the mid range.
	// Also, make sure that it's in the peaks to begin with.
	if (flag)
	{
		for (int p = -1; p <= 1; p++)
		{
			for (int q = -1; q <= 1; q++)
			{
				if (mag[a+p][b+q] >= lo && peaks->contains(a+p, b+q))
				{
					recursiveDT(mag, final, h, peaks, a+p, b+q, 1);
					final[a][b] = 255;
				}
			}
		}
	}

	return;
}
