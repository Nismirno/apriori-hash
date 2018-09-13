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

bool parseCommandLine(int argc, char *argv[],
                      std::string &fileName,
                      double &support, uint32 &nRules, Order &order) {
	// TODO: Parse arguments
	try {
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "produce help message")
			("filename,f", po::value<std::string>(), "path to the data file")
			("support,s", po::value<double>(&support), "minimum support of the set")
			("nRules,n", po::value<uint32>(&nRules), "number of rules to show")
			("order,o", po::value<std::string>(),
			 "order of rules by support (asc, desc)");

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
			std::cout << "Usage: options" << std::endl;
			std::cout << desc << std::endl;
			return false;
		}

		if (!vm.count("support")) {
			std::cout << "Please specify support value in percents." << std::endl;
			std::cout << "'--support value' or '-s value'." << std::endl;
			return false;
		}

		if (!vm.count("nRules")) {
			std::cout << "Please specify number of rules to print out." << std::endl;
			std::cout << "'--nRules value' or '-n value'." << std::endl;
			return false;
		}

		if (vm.count("filename")) {
			fileName = vm["filename"].as<std::string>();
		} else {
			fileName = "data/test.data";
		}

		if (vm.count("order")) {
			std::string orderStr = vm["order"].as<std::string>();
			if (strcmp(orderStr.data(), "asc") <= 0) {
				order = Order::ASC;
			} else if (strcmp(orderStr.data(), "desc") <= 0) {
				order = Order::DESC;
			} else {
				std::cout << "Unknown type of order." << std::endl;
				std::cout << "Please use 'asc' (ascending) or 'desc' (descending)." << std::endl;
				return false;
			}
		} else {
			order = Order::NONE;
		} 
	}
	catch (std::exception &e) {
		std::cerr << "error: " << e.what() << std::endl;
		return false;
	}

	return true;
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

	double support = 0;
	uint32 nRules = 0;
	std::string fileName = "";
	Order order;

	bool result = parseCommandLine(argc, argv, fileName, support, nRules, order);
	if (!result) {
		return 1;
	}

	auto transactions = getMatrix(fileName.data());
	uint32 maxItems = 100;

	// NOTE(myself): 5.152% - all items are passing k=1
	apriori::AprioriAlg apriori(transactions, support, maxItems);
	apriori.runAlg();
	apriori.printRules(nRules, order);

	return 0;
}
