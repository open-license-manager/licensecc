/*
 * LicenseSigner.h
 *
 *  Created on: Apr 6, 2014
 *      Author: devel
 */

#ifndef LICENSE_GENERATOR_H_
#define LICENSE_GENERATOR_H_

#include <boost/program_options.hpp>
#include "../library/LicenseReader.h"

namespace license {

namespace po = boost::program_options;
/**
 * This class is used to generate a new license.
 * It has no metnod main, for testing reasons.
 */
class LicenseGenerator {
private:
	LicenseGenerator();
	static void printHelp(const char* prog_name, const po::options_description& options);
	static po::options_description configureProgramOptions();
	static vector<FullLicenseInfo> parseLicenseInfo(po::variables_map vm);
	static void generateAndOutptuLicenses(const po::variables_map& vm,
			ostream& outputFile);

public:
	static int generateLicense(int argc, const char** argv);
};

} /* namespace license */

#endif /* LICENSE_GENERATOR_H_ */
