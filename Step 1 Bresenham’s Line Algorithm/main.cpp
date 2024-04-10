#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) 
{
	// Determine whether y is more steep than x
	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1))
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}

	// Check whether image is flip
	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	
	// Decrease the interporlation error
	int dx = x1 - x0;
	int dy = y1 - y0;
	float derror2 = std::abs(dy)*2;
	float error2 = 0;
	int y = y0;
	for (float x = x0; x <= x1; x++)
	{
		if (steep = true)
		{
			image.set(y, x, color);
		}
		else
		{
			image.set(x, y, color);
		}
		error2 += derror2;
		if (error2 > dx)
		{
			// Choose upper or lower pixel
			y += (y1 > y0 ? 1 : -1);

			// Make sure the error within certain value
			error2 -= dx * 2;
		}
	}
}

int main(int argc, char** argv) {
	TGAImage image(100, 100, TGAImage::RGB);
	line(13, 20, 80, 40, image, white);
	line(20, 13, 40, 80, image, red);
	line(80, 40, 13, 20, image, red);
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}
