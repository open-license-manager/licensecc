/*
 * boostrap.c
 *
 *  Created on: Apr 5, 2014
 *      Author: devel
 */

#include <string>


using namespace std;


void print_usage() {
	printf("usage: bootstrap private-fname public-fname\n");
}

int main(int argc, char** argv) {

	if (argc != 3) {
		print_usage();
		exit(2);
	} else {
		printf("********************************************\n");
		printf("*  Bootstrap!!!                            *\n");
		printf("********************************************\n");

	}
	string private_fname = string(argv[1]);
	string public_fname(argv[2]);

	return 0;
}
