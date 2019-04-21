/*
 * LicenseSigner.h
 *
 *  Created on: Apr 6, 2014
 *      
 */

#ifndef LICENSE_GENERATOR_H_
#define LICENSE_GENERATOR_H_

#include <boost/program_options.hpp>
#include "../../library/LicenseReader.h"

namespace license {

namespace po = boost::program_options;
/**
 * This class contains all the logic used to generate a new license.
 * Since it has no method "main", can be easily used in unit tests.
 */
class LicenseGenerator {
private:
	LicenseGenerator();
	static void printHelp(const char* prog_name, const po::options_description& options);
	static po::options_description configureProgramOptions();
	static vector<FullLicenseInfo> parseLicenseInfo(const po::variables_map& vm);
	static void generateAndOutputLicenses(const po::variables_map& vm,
			ostream& outputFile);
	static string normalize_date(const std::string& s);
public:
	/**
	 * Available options:
	 * <ul>
	 * 	<li>-s : use Client Signature.</li>
	 * </ul>
	 * @param argc
	 * 			count of arguments.
	 * @param argv
	 * 			char** of parameters.
	 * @return
	 */
	static int generateLicense(int argc, const char** argv);
};

} /* namespace license */

#endif /* LICENSE_GENERATOR_H_ */
