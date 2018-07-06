#include "typedef.hpp"
#include "lodepng.h"
#include "goodfunctions.hpp"
#include <iostream>
vectormap set_to_vectormap(tuple_set in, const int& rows, const int& cols)
{
	std::vector<int> row(cols);
	vectormap map(rows, row);
	for (tuple_int point : in)
	{
		int tempX = std::get<0>(point);
		int tempY = std::get<1>(point);
		if (in_bounds(0, rows, 0, cols, tempX, tempY) == true)
		{
			map[tempY][tempX] = 1;
		}
	}
	return map;
}

void array_img(std::vector<std::vector<int>>& array, std::vector<unsigned char>& img,
	const int& rows, const int& cols, int terrain = 0)
{
	std::vector<unsigned char> color0;
	std::vector<unsigned char> color1;
	std::vector<unsigned char> color2;
	std::vector<unsigned char> color3;
	std::vector<unsigned char> color4;
	std::vector<std::vector<unsigned char>> colors;
	//0=grass, 1=forest, 2=marsh, 3=mount, 4=water
	if (terrain == 0)
	{
		color0 = { 179,235,75,255 };
		color1 = { 42,97,24,255 };
		color2 = { 85,107,47,255 };
		color3 = { 122,106,61,255 };
		color4 = { 26,130,172,255 };
	}
	colors = { color0,color1,color2,color3,color4 };
	for (int row = 0; row < rows; ++row)
	{
		for (int col = 0; col < cols; ++col)
		{
			img[4 * row*cols + 4 * col] = colors[array[row][col]][0];
			img[4 * row*cols + 4 * col + 1] = colors[array[row][col]][1];
			img[4 * row*cols + 4 * col + 2] = colors[array[row][col]][2];
			img[4 * row*cols + 4 * col + 3] = colors[array[row][col]][3];
		}
	}
}

void checkthis(vectormap& in, int map_x, int map_y, int rows, int cols)
{
	///set up the base
	//union it. set constraints on the other terrains.  (only add the initial if inside where you want)
	std::string filename = "cplus " + std::to_string(map_x) + " , " + std::to_string(map_y) + " .png";
	std::vector<unsigned char> img(4 * rows*cols);
	//vectormap map = set_to_vectormap(in, rows, cols);
	array_img(in, img, rows, cols);
	unsigned error = lodepng::encode(filename.c_str(), img, rows, cols);
	if (error)
	{
		std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << "\n";
	}
}