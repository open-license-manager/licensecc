
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include "metalicensor/crypto/CryptoHelper.h"

using namespace std;
namespace license {

void write_pubkey_file(const string& public_fname, const string& pbPublicKey) {
    FILE* fp = fopen(public_fname.c_str(), "w");
    if (fp == NULL) {
        throw ios_base::failure(string("can't create :") + public_fname);
    }
    fprintf(fp, "//file generated by bootstrap.cpp, do not edit.\n\n");
    fprintf(fp, "#ifndef PUBLIC_KEY_H_\n#define PUBLIC_KEY_H_\n");
    fprintf(fp, "#define PUBLIC_KEY { \\\n");
    fprintf(fp, "%s", pbPublicKey.c_str());
    fprintf(fp, "}\n\n");
    int random = rand() % 1000;
    fprintf(fp, "#define SHARED_RANDOM %d;\n", random);
    fprintf(fp, "#endif\n");
    fclose(fp);
}

void write_privkey_file(const string& private_fname, const string& privateKey) {
    FILE* fp = fopen(private_fname.c_str(), "w");
    if (fp == NULL) {
        throw ios_base::failure(string("can't create :") + private_fname);
    }
    fprintf(fp, "//file generated by bootstrap.cpp, do not edit.\n\n");
    fprintf(fp, "#ifndef PRIVATE_KEY_H_\n#define PRIVATE_KEY_H_\n");
    fprintf(fp, "#define PRIVATE_KEY { \\\n");
    fprintf(fp, "%s", privateKey.c_str());
    fprintf(fp, "}\n\n");
    fprintf(fp, "#endif\n");
    fclose(fp);
}

void generatePk(string private_include, string public_include) {
    unique_ptr<CryptoHelper> cryptoHlpr = CryptoHelper::getInstance();

    try {
        cryptoHlpr->generateKeyPair();
    } catch (exception &e) {
        cerr << endl << "Error generating key pair: " << e.what() << endl
                << "aborting" << endl;
        exit(2);
    }

    try {
        const string pubKey = cryptoHlpr->exportPublicKey();
        write_pubkey_file(public_include, pubKey);
        // Print out the public key to console as a
        // hexadecimal string.
        cout << endl << "PublicKey" << pubKey.c_str() << endl;
    } catch (exception &e) {
        cerr << endl << "Error exporting public key: " << e.what() << endl
                << "aborting." << endl;
        exit(4);
    }

    try {
        const string privKey = cryptoHlpr->exportPrivateKey();
        write_privkey_file(private_include, privKey);
    } catch (exception &e) {
        cerr << endl << "Error exporting private key: " << e.what() << endl
                << "aborting" << endl;
        exit(5);
    }

    return;
}
}

int main(int argc, char** argv) {

    if (argc != 3) {
        //print_usage();
        exit(2);
    } else {
        printf("********************************************\n");
        printf("*  Bootstrap!!!                            *\n");
        printf("********************************************\n");

    }
    string private_fname = string(argv[1]);
    string public_fname(argv[2]);

    license::generatePk(private_fname, public_fname);
    return 0;
}
