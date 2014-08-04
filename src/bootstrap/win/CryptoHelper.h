
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
3333333//--------------------------------------------------------------------
// The destructor releases the handles acquired
// when an object goes out of scope.
//--------------------------------------------------------------------
CryptoHelper::~CryptoHelper()
{
       if (m_hCryptProv)
       {
              CryptReleaseContext(m_hCryptProv,0);
              m_hCryptProv = NULL;
       }
       if (m_hCryptKey)
              m_hCryptKey = NULL;
}

//--------------------------------------------------------------------
// This method calls the CryptAcquireContext function
// to get a handle to a key container owned by the the
// Microsoft Enhanced Cryptographic Provider.
//--------------------------------------------------------------------
HRESULT CryptoHelper::AcquireContext(LPCTSTR wszContainerName)
{
       HRESULT       hr = S_OK;
       DWORD         dwErrCode;
// Release a previously acquired handle to the key container.
if (m_hCryptProv != NULL)
       {
              CryptReleaseContext(m_hCryptProv,0);
              m_hCryptProv = NULL;
       }
// Release a previously acquired handle to key-pair.
       if (m_hCryptKey != NULL)
              m_hCryptKey = NULL;
       // Attempt to acquire a context and a key container.
// The context will use Microsoft Enhanced Cryptographic
       // Provider for the RSA_FULL provider type.
       if(!CryptAcquireContext(&m_hCryptProv,
                               wszContainerName,
                               MS_ENHANCED_PROV,
                               PROV_RSA_FULL,
    CRYPT_MACHINE_KEYSET))
       {
         // An error occurred in acquiring the context. This could mean
         // that the key container requested does not exist. In this case,
        // the function can be called again to attempt to create a new key
              // container.
              if (GetLastError() == NTE_BAD_KEYSET)
              {
                     if(!CryptAcquireContext(&m_hCryptProv,
                                            wszContainerName,
                                            MS_ENHANCED_PROV,   PROV_RSA_FULL,
                                            CRYPT_MACHINE_KEYSET|CRYPT_NEWKEYSET))
                     {
                           dwErrCode = GetLastError();
                            return HRESULT_FROM_WIN32(dwErrCode);
                     }
              }
              else
              {
                     dwErrCode = GetLastError();
                     return HRESULT_FROM_WIN32(dwErrCode);
              }
       }
       return hr;
}
//--------------------------------------------------------------------

// This method calls the CryptGenKey function to get a handle to an

// exportable key-pair. The key-pair is  generated with the RSA public-key
// key exchange algorithm using Microsoft Enhanced Cryptographic Provider.
//--------------------------------------------------------------------
HRESULT CryptoHelper::GenerateKeyPair()
{
       HRESULT       hr = S_OK;
       DWORD         dwErrCode;
       // If the handle to key container is NULL, fail.
if (m_hCryptProv == NULL)
              return E_FAIL;
// Release a previously acquired handle to key-pair.
if (m_hCryptKey)
              m_hCryptKey = NULL;
       // Call the CryptGenKey method to get a handle
       // to a new exportable key-pair.
if(!CryptGenKey(m_hCryptProv,
                ENCRYPT_ALGORITHM,
   KEYLENGTH | CRYPT_EXPORTABLE,
   &m_hCryptKey))
       {
              dwErrCode = GetLastError();
              return HRESULT_FROM_WIN32(dwErrCode);
       }
       return hr;
}
//--------------------------------------------------------------------
// This method calls the CryptExportKey function to get the Public key
// in a byte array. The byte array is allocated on the heap and the size
// of this is returned to the caller. The caller is responsible for releasing // this memory using a delete call.
//--------------------------------------------------------------------
HRESULT CryptoHelper::ExportPublicKey(BYTE **ppPublicKey, DWORD &cbKeySize)
{
       HRESULT hr = S_OK;
       DWORD    dwErrCode;
       DWORD dwBlobLen;
       BYTE *pbKeyBlob = NULL;
       // If the handle to key container is NULL, fail.
       if (m_hCryptKey == NULL)
              return E_FAIL;
       // This call here determines the length of the key
       // blob.
       if(!CryptExportKey(
                 m_hCryptKey,
                 NULL,
                 PUBLICKEYBLOB,
                 0,
                 NULL,
                 &dwBlobLen))
       {
              dwErrCode = GetLastError();
              return HRESULT_FROM_WIN32(dwErrCode);
       }
       // Allocate memory for the pbKeyBlob.
       if((pbKeyBlob = new BYTE[dwBlobLen]) == NULL)
       {
              return E_OUTOFMEMORY;
       }
       // Do the actual exporting into the key BLOB.
       if(!CryptExportKey(
                 m_hCryptKey,
                 NULL,
                 PUBLICKEYBLOB,
                 0,
                 pbKeyBlob,
                 &dwBlobLen))
       {
              delete pbKeyBlob;
              dwErrCode = GetLastError();
              return HRESULT_FROM_WIN32(dwErrCode);
       }
       else
       {
               *ppPublicKey = pbKeyBlob;
               cbKeySize = dwBlobLen;
       }
       return hr;
}
//--------------------------------------------------------------------
// This method calls the CryptExportKey function to get the Private key
// in a byte array. The byte array is allocated on the heap and the size
// of this is returned to the caller. The caller is responsible for releasing // this memory using a delete call.
//--------------------------------------------------------------------
HRESULT CryptoHelper::ExportPrivateKey(BYTE **ppPrivateKey, DWORD &cbKeySize)
{
       HRESULT       hr = S_OK;
       DWORD         dwErrCode;
       DWORD dwBlobLen;
       BYTE *pbKeyBlob;
       // If the handle to key container is NULL, fail.
       if (m_hCryptKey == NULL)
              return E_FAIL;
       // This call here determines the length of the key
       // blob.
       if(!CryptExportKey(
                 m_hCryptKey,
                 NULL,
                 PRIVATEKEYBLOB,
                 0,
                 NULL,
                 &dwBlobLen))
       {
              dwErrCode = GetLastError();
              return HRESULT_FROM_WIN32(dwErrCode);
       }
       // Allocate memory for the pbKeyBlob.
       if((pbKeyBlob = new BYTE[dwBlobLen]) == NULL)
       {
              return E_OUTOFMEMORY;
       }

       // Do the actual exporting into the key BLOB.
       if(!CryptExportKey(
                 m_hCryptKey,
                 NULL,
                 PRIVATEKEYBLOB,
                 0,
                 pbKeyBlob,
                 &dwBlobLen))
       {
              delete pbKeyBlob;
              dwErrCode = GetLastError();
              return HRESULT_FROM_WIN32(dwErrCode);
       }
       else
       {
               *ppPrivateKey = pbKeyBlob;
               cbKeySize = dwBlobLen;
       }
       return hr;
}

Main.cpp
 #include <stdio.h>
#include "CryptoHelper.h"
void main()
{
       CryptoHelper  cryptoHlpr;
       BYTE          *pbPublicKey = NULL, *pbPrivateKey = NULL;
       DWORD         dwPublicKeySize = 0,dwPrivateKeySize = 0;
       HRESULT       hr = S_OK;
       // Get the key container context.
       if (FAILED(hr = cryptoHlpr.AcquireContext((L"TestContainer"))))
       {
// Call FormatMessage to display the error returned in hr.
              return;
       }
        // Generate the public/private key pair.
if (FAILED(hr = cryptoHlpr.GenerateKeyPair()))
       {
// Call FormatMessage to display the error returned in hr.
              return;
       }
       // Export out the public key blob.
if (FAILED(hr = cryptoHlpr.ExportPublicKey(&pbPublicKey, dwPublicKeySize)))
       {
// Call FormatMessage to display the error returned in hr.
              return;
       }
       // Print out the public key to console as a
       // hexadecimal string.
wprintf(L"\n\nPublicKey = \"");
       for (DWORD i=0; i < dwPublicKeySize; i++)
       {
              wprintf(L"%02x",pbPublicKey[i]);
       }
       wprintf(L"\"\n");
       // Export out the private key blob.
       cryptoHlpr.ExportPrivateKey(&pbPrivateKey, dwPrivateKeySize);
       // Print out the private key to console as a
       // hexadecimal string.
       wprintf(L"\n\nPrivateKey = \"");
       for (i=0; i < dwPrivateKeySize; i++)
       {
              wprintf(L"%02x",pbPrivateKey[i]);
       }
wprintf(L"\"\n");
       // Delete the public key blob allocated by the
// ExportPublicKey method.
if (pbPublicKey)
          delete [] pbPublicKey;
       // Delete the private key blob allocated by the
// ExportPrivateKey method.
       if (pbPrivateKey)
              delete [] pbPrivateKey;
       return;
}
