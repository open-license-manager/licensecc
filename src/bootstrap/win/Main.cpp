#include <stdio.h>
#include "CryptoHelper.h"

void main() {
	CryptoHelper cryptoHlpr;
	BYTE *pbPublicKey = NULL, *pbPrivateKey = NULL;
	DWORD dwPublicKeySize = 0, dwPrivateKeySize = 0;
	HRESULT hr = S_OK;
	// Get the key container context.
	if (FAILED(hr = cryptoHlpr.AcquireContext((L"TestContainer")))) {
// Call FormatMessage to display the error returned in hr.
		return;
	}
	// Generate the public/private key pair.
	if (FAILED(hr = cryptoHlpr.GenerateKeyPair())) {
// Call FormatMessage to display the error returned in hr.
		return;
	}
	// Export out the public key blob.
	if (FAILED(
			hr = cryptoHlpr.ExportPublicKey(&pbPublicKey, dwPublicKeySize))) {
// Call FormatMessage to display the error returned in hr.
		return;
	}
	// Print out the public key to console as a
	// hexadecimal string.
	wprintf(L"\n\nPublicKey = \"");
	for (DWORD i = 0; i < dwPublicKeySize; i++) {
		wprintf(L"%02x", pbPublicKey[i]);
	}
	wprintf(L"\"\n");
	// Export out the private key blob.
	cryptoHlpr.ExportPrivateKey(&pbPrivateKey, dwPrivateKeySize);
	// Print out the private key to console as a
	// hexadecimal string.
	wprintf(L"\n\nPrivateKey = \"");
	for (i = 0; i < dwPrivateKeySize; i++) {
		wprintf(L"%02x", pbPrivateKey[i]);
	}
	wprintf(L"\"\n");
	// Delete the public key blob allocated by the
// ExportPublicKey method.
	if (pbPublicKey)
		delete[] pbPublicKey;
	// Delete the private key blob allocated by the
// ExportPrivateKey method.
	if (pbPrivateKey)
		delete[] pbPrivateKey;
	return;
}
