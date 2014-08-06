
#define _WIN32_WINNT 0x0400
#include <windows.h>
#include <wincrypt.h>
#include <tchar.h>

// Helper class definition to generate and export Public/Private keys
// for Asymmetric encryption. The semantics for usage are:
// Call AcquireContext with a container name, call
// GenerateKeyPair next and then  call ExportPublicKey or
// ExportPrivateKey.
class CryptoHelper
{
private:
	HCRYPTPROV    m_hCryptProv;
public:
	HCRYPTKEY     m_hCryptKey;
	CryptoHelper();
	~CryptoHelper();
	HRESULT AcquireContext(LPCTSTR wszContainerName);
	HRESULT GenerateKeyPair();

	HRESULT ExportPublicKey(BYTE **ppPublicKey, DWORD &cbKeySize);;
	HRESULT ExportPrivateKey(BYTE **ppPrivateKey, DWORD &cbKeySize);
};
