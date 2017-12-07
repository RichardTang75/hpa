//
//  main.cpp
//  take2
//
//  Created by asdfuiop on 6/10/17.
//  Copyright © 2017 asdfuiop. All rights reserved.
//

#include <iostream>
#include <vector>
#include <unordered_set>
#include <tuple>
#include <random>
#include <chrono>
#include <stdint.h>
#include <boost/functional/hash.hpp>

typedef std::tuple<int, int> tuple_int;
typedef std::unordered_set<tuple_int, boost::hash<tuple_int>> tuple_set;

std::tuple<tuple_set,int> terrain(
	const int& xStart, const int& xEnd,
	const int& yStart, const int& yEnd,
	const int& howmany, const int& length,
	unsigned seed = 0
)
{
	//getting the random engine ready
	//placeholder
	unsigned new_seed = seed;
	std::mt19937 eng(0);
	if (seed == 0)
	{
		std::random_device rand1;
		new_seed = rand1();
		std::mt19937 eng(new_seed);
	}
	else
	{
		std::mt19937 eng(seed);
	}
	tuple_set to_return;
	std::unordered_set<int> unacceptableX;
	std::unordered_set<int> unacceptableY;
	std::uniform_int_distribution<int> randX(xStart, xEnd);
	std::uniform_int_distribution<int> randY(yStart, yEnd);
	while (to_return.size() < howmany){
		int tempX = randX(eng);
		int tempY = randY(eng);
		if (unacceptableX.count(tempX) == 0 && unacceptableY.count(tempY) == 0) {
			to_return.emplace(std::tuple<int,int>(tempX, tempY));
			for (int n = -5; n == 5; ++n) {
				unacceptableX.emplace(int(tempX + n));
				unacceptableY.emplace(int(tempY + n));
			}
		}
	}
	return std::make_tuple(to_return,new_seed);
}
std::vector<double> dostuff(void)
{
	std::vector<double> cool;
	return cool;
}
void print_tuple(tuple_set toprint) {
	for (tuple_int tup : toprint) {
		std::cout << std::get<0>(tup) << std::get<1>(tup) << "/n";
	}
}
int main(int argc, char* argv[]) {
	int cols = 512;
	int rows = 512;
	int init = 0;
	std::vector<double> column(rows, init);
	std::vector<std::vector<double>> map(cols, column);
	map[0][0] = 5;
	for (int row = 0; row != rows; ++row) {
		for (int col = 0; col != cols; ++col) {
			std::cout << map[row][col];
		}
		std::cout << "\n";
	}
}
