#include "stdafx.h"
#include "License.h"

#ifdef ENABLE_LICENSE


#include <time.h>

// BOOLEAN与asn.h中的冲突，不得不将asn.h中的改了，希望有人帮忙解决这个问题
#include <windows.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

#include "../include/inirw.h"

#define APP_NAME_USER		"User"
#define KEY_NAME_ACCOUNT	"Account"
#define KEY_NAME_USERNAME	"UserName"

#define KEY_NAME_PRODUCT	"Product"
#define APP_NAME_LICENSE	"License"
#define KEY_NAME_COUNT		"Count"
#define KEY_NAME_MAC		"MachineID"
#define KEY_NAME_TRIAL		"Trial"
#define KEY_NAME_EXPIRE_DATE	"ExpireDate"
#define KEY_NAME_SERIAL_NUMBER	"SerialNumber"

// Prints the MAC address stored in a 6 byte array to stdout
static void PrintMACaddress(unsigned char MACData[])
{

#ifdef PRINTING_TO_CONSOLE_ALLOWED

	printf("\nMAC Address: %02X-%02X-%02X-%02X-%02X-%02X\n",
		MACData[0], MACData[1], MACData[2], MACData[3], MACData[4], MACData[5]);

#endif

	char string[256];
	sprintf(string, "%02X-%02X-%02X-%02X-%02X-%02X\n", MACData[0], MACData[1],
		MACData[2], MACData[3], MACData[4], MACData[5]);
	//WriteConstantString("MACaddress", string);
	printf(string);
}

// Fetches the MAC address and prints it
DWORD GetMACaddress()
{
	DWORD MACaddress = 0;
	IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information
	// for up to 16 NICs
	DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

	DWORD dwStatus = GetAdaptersInfo(      // Call GetAdapterInfo
		AdapterInfo,                 // [out] buffer to receive data
		&dwBufLen);                  // [in] size of receive data buffer
	//assert(dwStatus == ERROR_SUCCESS);  // Verify return value is
	// valid, no buffer overflow

	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; // Contains pointer to
	// current adapter info
	do {
		if (MACaddress == 0)
			MACaddress = pAdapterInfo->Address[5] + pAdapterInfo->Address[4] * 256 +
			pAdapterInfo->Address[3] * 256 * 256 +
			pAdapterInfo->Address[2] * 256 * 256 * 256;
		//PrintMACaddress(pAdapterInfo->Address); // Print MAC address
		//memcpy(pAddress1, pAdapterInfo->Address, sizeof(pAdapterInfo->Address));
		//break;
		pAdapterInfo = pAdapterInfo->Next;    // Progress through linked list
	} while (pAdapterInfo);                    // Terminate if last adapter

	return MACaddress;
}


CLicense::CLicense()
{
	m_bLoaded = false;

	sprintf_s(m_RealMAC, "%ld", GetMACaddress());

	CreateDefault();
}


CLicense::~CLicense()
{
}

int CLicense::Today(int day)
{
	time_t now = time(0);
	now += day * 86400;
	struct tm *ptmNow = localtime(&now);

	return (ptmNow->tm_year + 1900) * 10000
		+ (ptmNow->tm_mon + 1) * 100
		+ ptmNow->tm_mday;
}

void CLicense::GetDllPathByFunctionName(const char* szFunctionName, char* szPath)
{
	HMODULE hModule = nullptr;
	GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, szFunctionName, &hModule);

	GetModuleFileNameA(hModule, szPath, MAX_PATH);
}

void CLicense::SetLicensePath(const char* licPath)
{
	// 授权文件的路径是生成签名和验证都需要，所以最核心
	char szPath[_MAX_PATH] = { 0 };
	char drive[_MAX_DRIVE] = {0};
	char dir[_MAX_DIR] = { 0 };
	char fname[_MAX_FNAME] = { 0 };
	char ext[_MAX_EXT] = { 0 };
	_splitpath(licPath, drive, dir, fname, ext);
	_makepath(m_LicensePath, drive, dir, fname, FILE_EXT_LIC);
	_makepath(m_PublicKeyPath, drive, dir, fname, FILE_EXT_PUB);
	_makepath(m_PrivateKeyPath, drive, dir, fname, FILE_EXT_PRIV);
	_makepath(m_SignaturePath, drive, dir, fname, FILE_EXT_SIGN);
}

void CLicense::SetPublicKeyString(const char* pubKey)
{
	strncpy(m_PublicKeyString, pubKey, sizeof(m_PublicKeyString));
}

int CLicense::LoadIni()
{
	m_bLoaded = iniFileLoad(m_LicensePath);

	// 无所谓是否加载成功，反正有默认
	m_ExpireDate = iniGetInt(APP_NAME_LICENSE, KEY_NAME_EXPIRE_DATE, -1);
	m_Trial = iniGetInt(APP_NAME_LICENSE, KEY_NAME_TRIAL, 5);

	iniGetString(APP_NAME_USER, KEY_NAME_ACCOUNT, m_Account, sizeof(m_Account), "*|");
	iniGetString(APP_NAME_USER, KEY_NAME_USERNAME, m_UserName, sizeof(m_UserName), "*|");
	iniGetString(APP_NAME_LICENSE, KEY_NAME_MAC, m_MAC, sizeof(m_MAC), "*|");

	iniFileFree();

	vector<string> vct1;
	{
		char* token = strtok(m_Account, "|");
		while (token)
		{
			if (strlen(token) > 0)
			{
				vct1.push_back(string(token));
			}
			token = strtok(nullptr, "|");
		}
	}

	vector<string> vct2;
	{
		char* token = strtok(m_UserName, "|");
		while (token)
		{
			if (strlen(token) > 0)
			{
				vct2.push_back(string(token));
			}
			token = strtok(nullptr, "|");
		}
	}

	m_MacSet.clear();
	vector<string> vct3;
	{
		char* token = strtok(m_MAC, "|");
		while (token)
		{
			if (strlen(token) > 0)
			{
				m_MacSet.insert(string(token));
			}
			token = strtok(nullptr, "|");
		}
	}
	
	m_UserMap.clear();
	int count = min(vct1.size(), vct2.size());
	for (size_t i = 0; i < count; ++i)
	{
		AddUser(vct1[i].c_str(), vct2[i].c_str());
	}

	if (m_bLoaded != 0)
		return 0;

	sprintf(m_ErrorInfo, ERROR_CODE_1);
	m_ErrorCode = 1;
	return m_ErrorCode;
}

void CLicense::AddUser(const char* account, const char* name)
{
	m_UserMap[string(account)] = string(name);
}

int CLicense::SaveIni()
{
	iniFileLoad(m_LicensePath);

	iniSetInt(APP_NAME_LICENSE, KEY_NAME_EXPIRE_DATE, m_ExpireDate, 10);
	iniSetInt(APP_NAME_LICENSE, KEY_NAME_TRIAL, m_Trial, 10);

	m_Account[0] = 0;
	m_UserName[0] = 0;

	for (map<string, string>::iterator it = m_UserMap.begin(); it != m_UserMap.end(); ++it)
	{
		strcat(m_Account, it->first.c_str());
		strcat(m_Account, "|");
		strcat(m_UserName, it->second.c_str());
		strcat(m_UserName, "|");
	}
	iniSetString(APP_NAME_USER, KEY_NAME_ACCOUNT, m_Account);
	iniSetString(APP_NAME_USER, KEY_NAME_USERNAME, m_UserName);

	m_MAC[0] = 0;
	for (set<string>::iterator it = m_MacSet.begin(); it != m_MacSet.end(); ++it)
	{
		strcat(m_MAC, it->c_str());
		strcat(m_MAC, "|");
	}
	int bRet = iniSetString(APP_NAME_LICENSE, KEY_NAME_MAC, m_MAC);

	iniFileFree();

	if (bRet != 0)
		return 0;

	sprintf(m_ErrorInfo, ERROR_CODE_11);
	m_ErrorCode = -11;
	return m_ErrorCode;
}


void CLicense::CreateDefault()
{
	m_Trial = 5;	// 默认5次
	m_ExpireDate = Today(7);	// 7天
	m_UserMap.clear();
	m_MacSet.clear();
	m_nCurrentTrial = 0;

	AddUser("*", "*");
	m_MacSet.insert("*");
	m_MacSet.insert(m_RealMAC);
}

int CLicense::GetErrorCodeForSign()
{
	// 认证通过
	m_ErrorCode = 0;

	do
	{
		
		if (strlen(m_PublicKeyString) <= 0)
		{
			m_ErrorCode = 12;
			strcpy(m_ErrorInfo, ERROR_CODE_12);

			// 只有试用次数和日期可以检查了
			m_Trial = min(m_Trial, 5);
			m_ExpireDate = min(m_ExpireDate,Today(7));

			break;
		}

	} while (false);

	return m_ErrorCode;
}

int CLicense::GetErrorCode()
{
	// 认证通过
	m_ErrorCode = 0;

	do
	{
		// 检查机器码
		for (set<string>::iterator it = m_MacSet.begin(); it != m_MacSet.end(); ++it)
		{
			const char *p = (it)->c_str();
			if (strncmp(m_RealMAC, p, strlen(m_RealMAC)) == 0)
			{
				// 找到了
				m_ErrorCode = 0;
				break;
			}
			else if (p[0] == '*')
			{
				// *号就通过
				m_ErrorCode = 0;
				break;
			}

			strcpy(m_ErrorInfo, ERROR_CODE_5);
			m_ErrorCode = -5;
		}

		//// 没有任何绑定的，不能超过90天
		//// 只绑定名字的，不能超过180天
		//int limitDay = 1000;
		//if (m_bHasStarAccount&&m_bHasStarMAC)
		//{
		//	if (m_bHasStarUserName)
		//	{
		//		limitDay = 90;
		//	}
		//	else
		//	{
		//		limitDay = 180;
		//	}
		//}

		//// 看授权时间是否合要求
		//if (m_ExpireDate > Today(limitDay))
		//{
		//	m_ErrorCode = -10;
		//	sprintf(m_ErrorInfo, ERROR_CODE_10);
		//	break;
		//}

		// 是否过期了
		if (m_ExpireDate < Today(0))
		{
			m_ErrorCode = -3;
			sprintf(m_ErrorInfo, ERROR_CODE_3, m_ExpireDate);
			break;
		}

		string s = LoadStringFromFile(m_LicensePath);

		if (strlen(m_PublicKeyString) <= 0)
		{
			m_ErrorCode = 12;
			strcpy(m_ErrorInfo, ERROR_CODE_12);

			// 只有试用次数可以检查了，
			m_Trial = min(m_Trial, 5);

			break;
		}

		if (Verify(s.c_str(), m_PublicKeyString))
		{

		}
		else
		{
			m_ErrorCode = -2;
			strcpy(m_ErrorInfo, ERROR_CODE_2);
			break;
		}

	} while (false);

	return m_ErrorCode;
}

int CLicense::GetErrorCodeByAccount(const char* account)
{
	// 认证通过
	m_ErrorCode = 0;

	do
	{
		m_ErrorCode = -4;
		sprintf(m_ErrorInfo, ERROR_CODE_4, account);

		for (map<string, string>::iterator it = m_UserMap.begin(); it != m_UserMap.end(); ++it)
		{
			const char *p = (it)->first.c_str();
			if (strncmp(account, p, strlen(account)) == 0)
			{
				// 找到了
				m_ErrorCode = 0;
				break;
			}
			else if (p[0] == '*')
			{
				// *号就通过
				m_ErrorCode = 0;
				break;
			}
		}
	} while (false);

	return m_ErrorCode;
}

int CLicense::GetErrorCodeByNameThenAccount(const char* name, const char* account)
{
	// 认证通过
	m_ErrorCode = 0;

	do
	{
		m_ErrorCode = -9;
		sprintf(m_ErrorInfo, ERROR_CODE_9, name);

		for (map<string, string>::iterator it = m_UserMap.begin(); it != m_UserMap.end(); ++it)
		{
			const char *p1 = (it)->first.c_str();
			const char *p2 = (it)->second.c_str();
			if (strncmp(name, p2, strlen(name)) == 0)
			{
				if (p1[0] == '*')
				{
					// *号就通过
					m_ErrorCode = 0;
					break;
				}
				else if (strncmp(account, p1, strlen(account)) == 0)
				{
					// 找到了
					m_ErrorCode = 0;
					break;
				}
			}
			else if (p2[0] == '*')
			{
				if (p1[0] == '*')
				{
					// *号就通过
					m_ErrorCode = 0;
					break;
				}
				else if (strncmp(account, p1, strlen(account)) == 0)
				{
					// 找到了
					m_ErrorCode = 0;
					break;
				}
			}
		}
	} while (false);

	return m_ErrorCode;
}

int CLicense::GetErrorCodeByTrial()
{
	do
	{
		// 检查次数
		if (m_Trial > 0)
		{
			if (m_nCurrentTrial >= m_Trial)
			{
				sprintf(m_ErrorInfo, ERROR_CODE_6, m_Trial);
				m_ErrorCode = -6;
				break;
			}
		}

		// 认证通过就加1
		++m_nCurrentTrial;

		// 认证通过
		m_ErrorCode = 0;
	} while (false);

	return m_ErrorCode;
}

const char* CLicense::GetErrorInfo()
{
	if (m_ErrorCode == 0)
		return nullptr;
	return m_ErrorInfo;
}

string CLicense::LoadStringFromFile(const char *filename)
{
	try
	{
		string result;
		FileSource(filename, true, new StringSink(result));
		return result;
	}
	catch (...)
	{
		return "";
	}
}

void CLicense::Sign(const char* message)
{
	if (strlen(message) <= 0)
	{
		return;
	}
	if (strlen(m_PrivateKeyPath) <= 0)
	{
		return;
	}
	RSASignString(m_PrivateKeyPath, message, m_SignaturePath);
}

bool CLicense::Verify(const char* message, const char* pubKey)
{
	if (strlen(message) <= 0)
	{
		return false;
	}
	if (strlen(pubKey) <= 0)
	{
		return false;
	}

	return RSAVerifyStringString(pubKey, message, m_SignaturePath);
}

//------------------------
// 定义全局的随机数池
//------------------------
RandomPool & GlobalRNG()
{
	static RandomPool randomPool;
	return randomPool;
}

void GenerateRSAKey(unsigned int keyLength, const char *privFilename, const char *pubFilename, const char *seed)
{
	RandomPool randPool;
	randPool.IncorporateEntropy((byte *)seed, strlen(seed));

	RSAES_OAEP_SHA_Decryptor priv(randPool, keyLength);
	HexEncoder privFile(new FileSink(privFilename));
	priv.DEREncode(privFile);
	privFile.MessageEnd();

	RSAES_OAEP_SHA_Encryptor pub(priv);
	HexEncoder pubFile(new FileSink(pubFilename));
	pub.DEREncode(pubFile);
	pubFile.MessageEnd();
}

string RSAEncryptString(const char *pubFilename, const char *seed, const char *message)
{
	FileSource pubFile(pubFilename, true, new HexDecoder);
	RSAES_OAEP_SHA_Encryptor pub(pubFile);

	RandomPool randPool;
	randPool.IncorporateEntropy((byte *)seed, strlen(seed));

	string result;
	StringSource(message, true, new PK_EncryptorFilter(randPool, pub, new HexEncoder(new StringSink(result))));
	return result;
}

string RSADecryptString(const char *privFilename, const char *ciphertext)
{
	FileSource privFile(privFilename, true, new HexDecoder);
	RSAES_OAEP_SHA_Decryptor priv(privFile);

	string result;
	StringSource(ciphertext, true, new HexDecoder(new PK_DecryptorFilter(GlobalRNG(), priv, new StringSink(result))));
	return result;
}
//
//void RSASignFile(const char *privFilename, const char *messageFilename, const char *signatureFilename)
//{
//	FileSource privFile(privFilename, true, new HexDecoder);
//	RSASS<PKCS1v15, SHA>::Signer priv(privFile);
//	FileSource f(messageFilename, true, new SignerFilter(GlobalRNG(), priv, new HexEncoder(new FileSink(signatureFilename))));
//}
//
//bool RSAVerifyFile(const char *pubFilename, const char *messageFilename, const char *signatureFilename)
//{
//	FileSource pubFile(pubFilename, true, new HexDecoder);
//	RSASS<PKCS1v15, SHA>::Verifier pub(pubFile);
//
//	FileSource signatureFile(signatureFilename, true, new HexDecoder);
//	if (signatureFile.MaxRetrievable() != pub.SignatureLength())
//		return false;
//	SecByteBlock signature(pub.SignatureLength());
//	signatureFile.Get(signature, signature.size());
//
//	VerifierFilter *verifierFilter = new VerifierFilter(pub);
//	verifierFilter->Put(signature, pub.SignatureLength());
//	FileSource f(messageFilename, true, verifierFilter);
//
//	return verifierFilter->GetLastResult();
//}

void RSASignString(const char *privFilename, const char *messageFilename, const char *signatureFilename)
{
	FileSource privFile(privFilename, true, new HexDecoder);
	RSASS<PKCS1v15, SHA>::Signer priv(privFile);
	StringSource f(messageFilename, true, new SignerFilter(GlobalRNG(), priv, new HexEncoder(new FileSink(signatureFilename))));
}

bool RSAVerifyString(const char *pubFilename, const char *messageFilename, const char *signatureFilename)
{
	FileSource pubFile(pubFilename, true, new HexDecoder);
	RSASS<PKCS1v15, SHA>::Verifier pub(pubFile);

	FileSource signatureFile(signatureFilename, true, new HexDecoder);
	if (signatureFile.MaxRetrievable() != pub.SignatureLength())
		return false;
	SecByteBlock signature(pub.SignatureLength());
	signatureFile.Get(signature, signature.size());

	VerifierFilter *verifierFilter = new VerifierFilter(pub);
	verifierFilter->Put(signature, pub.SignatureLength());
	StringSource f(messageFilename, true, verifierFilter);

	return verifierFilter->GetLastResult();
}

bool RSAVerifyStringString(const char *pubFilename, const char *messageFilename, const char *signatureFilename)
{
	StringSource pubFile(pubFilename, true, new HexDecoder);
	RSASS<PKCS1v15, SHA>::Verifier pub(pubFile);

	FileSource signatureFile(signatureFilename, true, new HexDecoder);
	if (signatureFile.MaxRetrievable() != pub.SignatureLength())
		return false;
	SecByteBlock signature(pub.SignatureLength());
	signatureFile.Get(signature, signature.size());

	VerifierFilter *verifierFilter = new VerifierFilter(pub);
	verifierFilter->Put(signature, pub.SignatureLength());
	StringSource f(messageFilename, true, verifierFilter);

	return verifierFilter->GetLastResult();
}

#endif
