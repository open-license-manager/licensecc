#include <build_properties.h>
#include <private-key.h>
#include "license-generator.h"
#include "../base_lib/CryptoHelper.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <iostream>
#include <string.h>
#include <boost/date_time.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/unordered_map.hpp>
#include <boost/assign.hpp>
#include <fstream>
#include <regex>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
namespace bt = boost::posix_time;
namespace po = boost::program_options;

using namespace std;

namespace license {

void LicenseGenerator::printHelp(const char* prog_name,
		const po::options_description& options) {
	cout << endl;
	cout << prog_name << " Version " << PROJECT_VERSION << endl << ". Usage:"
			<< endl;
	cout << prog_name << " [options] product_name1 product_name2 ... " << endl
			<< endl;
	cout
			<< " product_name1 ... = Product name. This string must match the one passed by the software."
			<< endl;
	cout << options << endl;
}

po::options_description LicenseGenerator::configureProgramOptions() {
	po::options_description common("General options");
	common.add_options()
		("help,h", "print help message and exit.")
		("verbose,v", "print more information.")
		("output,o", po::value<string>(), "Output file name. If not specified the "
			"license will be printed in standard output")
		;
	po::options_description licenseGeneration("License Generation");
	licenseGeneration.add_options()
		("private_key,p", po::value<string>(),
			"Specify an alternate file for the primary key to be used. "
					"If not specified the internal primary key will be used.")
		("begin_date,b", po::value<string>(),
			"Specify the start of the validity for this license. "
					" Format YYYYMMDD. If not specified defaults to today")
		("expire_date,e", po::value<string>(),
			"Specify the expire date for this license. "
					" Format YYYYMMDD. If not specified the license won't expire")
		("client_signature,s", po::value<string>(),
			"The signature of the pc that requires the license. "
					"It should be in the format XXXX-XXXX-XXXX-XXXX."
					" If not specified the license "
					"won't be linked to a specific pc.")
		("start_version,t", po::value<unsigned int>()->default_value(0
			/*FullLicenseInfo.UNUSED_SOFTWARE_VERSION*/, "All Versions"),
			"Specify the first version of the software this license apply to.")
		("end_version,n", po::value<unsigned int>()->default_value(0
			/*FullLicenseInfo.UNUSED_SOFTWARE_VERSION*/, "All Versions"),
			"Specify the last version of the software this license apply to.")
		("extra_data,x", po::value<string>(), "Specify extra data to be included into the license")
		;
	po::options_description visibleOptions;
	visibleOptions.add(common).add(licenseGeneration);
	return visibleOptions;
}

vector<FullLicenseInfo> LicenseGenerator::parseLicenseInfo(
		const po::variables_map& vm) {
	string begin_date = FullLicenseInfo::UNUSED_TIME;
	string end_date = FullLicenseInfo::UNUSED_TIME;
	if (vm.count("expire_date")) {
		const std::string dt_end = vm["expire_date"].as<string>();
		try {
			end_date = normalize_date(dt_end);
			char curdate[20];
			time_t curtime = time(NULL);
			strftime(curdate, 20, "%Y-%m-%d", localtime(&curtime));
			begin_date.assign(curdate);
		} catch (const invalid_argument &e) {
			cerr << endl << "End date not recognized: " << dt_end
					<< " Please enter a valid date in format YYYYMMDD" << endl;
			exit(2);
		}
	}
	if (vm.count("begin_date")) {
		const std::string begin_date_str = vm["begin_date"].as<string>();
		try {
			begin_date = normalize_date(begin_date_str);
		} catch (invalid_argument &e) {
			cerr << endl << "Begin date not recognized: " << begin_date_str
					<< " Please enter a valid date in format YYYYMMDD" << endl;
			//print_usage(vm);
			exit(2);
		}
	}
	string client_signature = "";
	if (vm.count("client_signature")) {
		client_signature = vm["client_signature"].as<string>();
		regex e("[A-Za-z0-9\\+/]{4}-[A-Za-z0-9\\+/]{4}-[A-Za-z0-9\\+/]{4}-[A-Za-z0-9\\+/]{4}");
		if (!regex_match(client_signature, e)) {
			cerr << endl << "Client signature not recognized: "
				<< client_signature
				<< " Please enter a valid signature in format XXXX-XXXX-XXXX-XXXX"
				<< endl;
			exit(2);
		}
	}
	string extra_data = "";
	if (vm.count("extra_data")) {
		extra_data = vm["extra_data"].as<string>();
	}
	unsigned int from_sw_version = vm["start_version"].as<unsigned int>();
	unsigned int to_sw_version = vm["end_version"].as<unsigned int>();
	if (vm.count("product") == 0) {
		cerr << endl << "Parameter [product] not found. " << endl;
		exit(2);
	}
	vector<string> products = vm["product"].as<vector<string>>();
	vector<FullLicenseInfo> licInfo;
	licInfo.reserve(products.size());
	for (auto it = products.begin(); it != products.end(); it++) {
		if (boost::algorithm::trim_copy(*it).length() > 0) {
			licInfo.push_back(
					FullLicenseInfo("", *it, "", PROJECT_INT_VERSION,
							begin_date, end_date, client_signature,
							from_sw_version, to_sw_version, extra_data));
		}
	}
	return licInfo;
}

void LicenseGenerator::generateAndOutputLicenses(const po::variables_map& vm,
		ostream& outputFile) {
	vector<FullLicenseInfo> licenseInfo = parseLicenseInfo(vm);
	unique_ptr<CryptoHelper> helper = CryptoHelper::getInstance();
	const char pkey[] = PRIVATE_KEY;
	size_t len = sizeof(pkey);
	for (auto it = licenseInfo.begin(); it != licenseInfo.end(); ++it) {
		const string license = it->printForSign();
		string signature = helper->signString((const void *)pkey,len,license);
		it->license_signature = signature;
		it->printAsIni(outputFile);
	}
}

int LicenseGenerator::generateLicense(int argc, const char **argv) {

	po::options_description visibleOptions = configureProgramOptions();
	//positional options must be added to standard options
	po::options_description allOptions;
	allOptions.add(visibleOptions).add_options()("product",
			po::value<vector<string>>(), "product names");

	po::positional_options_description p;
	p.add("product", -1);

	po::variables_map vm;
	po::store(
			po::command_line_parser(argc, argv).options(allOptions).positional(
					p).run(), vm);
	po::notify(vm);
	if (vm.count("help") || argc == 1) {
		printHelp(argv[0], visibleOptions);
		return 0;
	}

	if (vm.count("output")) {
		const std::string fname = vm["output"].as<string>();

		fstream ofstream(fname, std::ios::out | std::ios::app);
		if (!ofstream.is_open()) {
			cerr << "can't open file [" << fname << "] for output." << endl
					<< " error: " << strerror( errno);
			exit(3);
		}
		generateAndOutputLicenses(vm, ofstream);
		ofstream.close();
	} else {
		generateAndOutputLicenses(vm, cout);
	}
	return 0;
}

const std::string formats[] = { "%4u-%2u-%2u", "%4u/%2u/%2u", "%4u%2u%2u" };
const size_t formats_n = 3;

string LicenseGenerator::normalize_date(const std::string& sDate) {
	if(sDate.size()<8)
		throw invalid_argument("Date string too small for known formats");
	unsigned int year, month, day;
	bool found = false;
	for (size_t i = 0; i < formats_n && !found; ++i) {
		const int chread = sscanf(sDate.c_str(),formats[i].c_str(),&year,&month,&day);
		if(chread==3) {
			found = true;
			break;
		}
	}
	if(!found)
		throw invalid_argument("Date string did not match a known format");
	ostringstream oss;
	oss << year << "-" << setfill('0') << std::setw(2) << month << "-" << setfill('0') << std::setw(2) << day;
	return oss.str();
}
}
