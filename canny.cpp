// Soliman Alnaizy, UCFID: 3715450
// CAP 4453, Dr. Lobo, Fall 2018
// ========================================================
// ASSIGNMENT #1.2: C A N N Y   E D G E   D E T E C T I O N
// ========================================================

#include <cmath>
#include <fstream>
#include <iostream>

using namespace std;

// Global type, height, width, intensity, hi, lo, and sig variables
char type[10];
int height;
int width;
int intensity;
int hi;
int lo;
double sig;

// An attempt to solve the canny thresholding using recursion
int recursiveDT(int **mag, int **peaks, int **final, int i, int j)
{
	if (mag[i][j] < lo || i < 0 || j < 0 || i >= height || j >= height)
		return 0;

	for (int p = -1; p <= 1; p++)
	{
		for (int q = -1; q <= 1; q++)
		{
			if (mag[i+p][j+q] >= lo && mag[i+p][j+q] < hi)
			{
					cout << "WE ARE NOW EXITING :LSINVPOWNIBO" << endl;

				final[i+p][j+q] = 255;
				if (recursiveDT(mag, peaks, final, i + p, j + q))
					return 0;
			}
		}
	}

	return 0;
}

int main(int argc, char **argv)
{
	// Exit program if proper arguments are not given by user
	if (argc != 4)
	{
		cout << "Proper syntax: ./a.out <input_filename> <high_threshold> <sigma_value>" << endl;
		return 0;
	}

	// Exit program if file doesn't open
	ifstream infile(argv[1], ios::binary);
	if (!infile.is_open())
	{
		cout << "File " << argv[1] << " not found in directory." << endl;
		return 0;
	}

	// Extract the input file name to name the output files accordingly
	

	// Output files
	ofstream img1("./output_images/canny_mag.pgm", ios::binary);
	ofstream img2("./output_images/canny_peaks.pgm", ios::binary);		
	ofstream img3("./output_images/canny_final.pgm", ios::binary);

	::hi = stoi(argv[2]);
	::lo = .35 * hi;
	::sig = stoi(argv[3]);
	int dim = 6 * sig + 1, cent = dim / 2;

	// Storing header information and copying into the new ouput images
	infile >> ::type >> ::width >> ::height >> ::intensity;
	img1 << type << endl << width << " " << height << endl << intensity << endl;
	img2 << type << endl << width << " " << height << endl << intensity << endl;
	img3 << type << endl << width << " " << height << endl << intensity << endl;

	// These matrices will hold the integer values of the input image and masks
	// dynamically allocating arrays to be able to pass them into functions
	double **pic = new double*[height], **mag = new double*[height];
	double **peaks = new double*[height], **final = new double*[height];
	double **x = new double*[height], **y = new double*[height];
	double **maskx = new double*[dim], **masky = new double*[dim];

	for (int i = 0; i < height; i++)
	{
		pic[i] = new double[width];
		mag[i] = new double[width];
		peaks[i] = new double[width];
		final[i] = new double[width];
		x[i] = new double[width];
		y[i] = new double[width];
	}

	for (int i = 0; i < dim; i++)
	{
		maskx[i] = new double[dim];
		masky[i] = new double[dim];
	}

	// Reading in the input image as integers
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			pic[i][j] = (int)infile.get();

	// Use the Gausian 1st derivative formula to fill in the mask values
	for (int p = -cent; p <= cent; p++)
	{	
		for (int q = -cent; q <= cent; q++)
		{
			maskx[p+cent][q+cent] = q * exp(-1 * ((p * p + q * q) / (2 * sig * sig)));
			masky[p+cent][q+cent] = p * exp(-1 * ((p * p + q * q) / (2 * sig * sig)));
		}
	}

	// ================================= Magnitude Image ================================
	// Preform a scanning convolution on the input picture
	double sumx, sumy, maxx = 0, maxy = 0;
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
					if ((i + p) < 0 || (j + q) < 0 || (i + p) >= height || (j + q) >= width)
						continue;
					
					sumx += pic[i+p][j+q] * maskx[p+cent][q+cent];
					sumy += pic[i+p][j+q] * masky[p+cent][q+cent];
				}
			}
			
			if (sumx > maxx)
				maxx = sumx;
			if (sumy > maxy)
				maxy = sumy;

			// Store convolution result in respective matrix
			x[i][j] = sumx;
			y[i][j] = sumy;
		}
	}

	// Make sure all the convolution values are between 0-255
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			x[i][j] = x[i][j] / maxx * 255;
			y[i][j] = y[i][j] / maxy * 255;
		}		
	}

	// Find magnitude and maxval, then store it in the 'mag' matrix
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

	// Outputting the 'mag' matrix to img1. It's very important to cast it to a char.
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			img1 << (char)((int)mag[i][j]);

	// ================================ Peaks Detection ================================
	double slope = 0;
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
					peaks[i][j] = 255;
			}
			else if (slope <= tan(67.5) && slope > tan(22.5))
			{
				if (mag[i][j] > mag[i-1][j-1] && mag[i][j] > mag[i+1][j+1])
					peaks[i][j] = 255;
			}
			else if (slope <= tan(-22.5) && slope > tan(-67.5))
			{
				if (mag[i][j] > mag[i+1][j-1] && mag[i][j] > mag[i-1][j+1])
					peaks[i][j] = 255;
			}
			else
			{
				if (mag[i][j] > mag[i-1][j] && mag[i][j] > mag[i+1][j])
					peaks[i][j] = 255;
			}
		}
	}

	// Outputting the 'peaks' matrix to img2
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			img2 << (char)peaks[i][j];

	// ======================== Hysteresis & Double Thresholding ========================
	// To get rid of all the garbage 'snow' in the peaks image, we'll first look at
	// the peaks matrix. If it's a 255 then look back at the 'mag' matrix. If mag is hi,
	// then automatically store in the final. If lo, then reject. We'll deal with the
	// middle values after this
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (peaks[i][j] == 255)
			{
				if (mag[i][j] >= hi)
				{
					final[i][j] = 255;
					peaks[i][j] = 0;
				}
				else if (mag[i][j] < lo)
				{
					final[i][j] = 0;
					peaks[i][j] = 0;
				}
			}
		}
	}

	// I'm honestly not 100% certain what's happening here. This is bascially
	// copied from "cannyparts3and4online1b.pptx" on the course website 
	int flag = 1;
	while (flag == 1)
	{
		flag = 0;
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (peaks[i][j] == 255)
				{
					for (int p = -1; p <= 1; p++)
					{
						for (int q = -1; q <= 1; q++)
						{
							if (final[i+p][j+q] == 255)
							{
								peaks[i][j] = 0;
								final[i][j] = 255;
								flag = 1;
							}
						}
					}
				}
			}
		}
	}

	// Output the 'final' matrix to img1
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			img3 << (char)final[i][j];		

	return 0;
}