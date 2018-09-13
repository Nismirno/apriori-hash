/* ===========================================================================
   $File: association_rules.cpp $
   $Date: 11 September 2018. $
   $Revision: $
   $Author: Nikita Smirnov $
   =========================================================================== */
#include <iostream>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <chrono>
#include <boost/program_options.hpp>

#include "apriori.h"
namespace po = boost::program_options;

//static const char* fileName = "data/association_rules.data";
static const char* fileName = "data/test.data";

struct timer {
	typedef std::chrono::high_resolution_clock clock;
	clock::time_point start_;
	
	timer() : start_(clock::now()) {}
	~timer()
	{
		std::cout
		<< ((double)std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - start_).count() / 1000.)
		<< "ms" << std::endl;
	}
};

void parseCommandLine(int argc, char *argv[]) {
	// TODO: Parse arguments
}

std::string vecToStr(std::vector<uint32> vec) {
	std::string output = "{";
	auto iter = vec.begin();
	for (; iter != vec.end(); iter++) {
		output += std::to_string(*iter);
		if (iter < vec.end() - 1) {
			output += ", ";
		}
	}
	output += "}";
	return output;
}

matrixDimension getMatrixSize(const char *file) {
	std::ifstream in(file, std::ifstream::binary);
	std::string line;

	in >> line;
	auto columns = line.length();

	in.seekg(0, std::ifstream::end);
	auto size = in.tellg();
	in.close();

	// NOTE: Need to figure out number of rows (lines)
	// fileSize counts newline characters but `data >> line` omits newline character
	// Need to increase length of line by one to get correct number of lines
	uint32 rows = size / (columns + 1);

	matrixDimension dim = {};
	dim.rows = rows;
	dim.columns = columns;
	return dim;
}

std::vector<std::vector<uint32>> getMatrix(const char *file) {
	matrixDimension dim = getMatrixSize(file);
	std::ifstream in(file, std::ifstream::binary);

	std::vector<std::vector<uint32>> matrix;
	matrix.resize(dim.rows);
	for (uint32 i = 0; i < dim.rows; i++) {
		for (uint32 j = 0; j < dim.columns; j++) {
			uint8 ch;
			in >> ch;
			// NOTE: Save actual "ID" of item instead
			if (ch == '1') {
				matrix[i].push_back(j);
			}
		}
	}
	in.close();

	return matrix;
}

int main(int argc, char *argv[]) {
	// NOTE: Temporary performance measurement
	timer t;

	// TODO: Parse command-line arguments with boost
	parseCommandLine(argc, argv);

	// 2D array
	auto transactions = getMatrix(fileName);
	uint32 maxItems = 100;
	// NOTE(myself): Benchmark 5.152% - all items
//	double minSupport = 0.3;
	double minSupport = 2.0 / 7.0 * 100.0;
	apriori::AprioriAlg apriori(transactions, minSupport, maxItems);
	apriori.runAlg();
	apriori.printRules(20, Order::DESC);

	return 0;
}
