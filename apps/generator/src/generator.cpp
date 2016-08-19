#include <stdlib.h>
#include "metalicensor/generator/license-generator.h"

int main(int argc, const char *argv[]) {
	 return license::LicenseGenerator::generateLicense(argc, argv);
}

