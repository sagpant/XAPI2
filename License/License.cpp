#include "stdafx.h"
#include "License.h"

#include <time.h>

#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")


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

void CLicense::LoadIni()
{
	// 过期时间，要想长期有效就改大，比如说改成20991231
	m_ExpireDate = ::GetPrivateProfileIntA(APP_NAME_LICENSE, KEY_NAME_EXPIRE_DATE, -1, m_LicPath);
	m_bLoaded = m_ExpireDate > 0;
	if (!m_bLoaded)
	{
		m_ExpireDate = 19860901;
	}

	// 试用时每次下单次数，默认给5次，如果为0表示没有限制
	m_Trial = ::GetPrivateProfileIntA(APP_NAME_LICENSE, KEY_NAME_TRIAL, 5, m_LicPath);
	//::GetPrivateProfileStringA(APP_NAME_LICENSE, KEY_NAME_SERIAL_NUMBER, "#", m_SerialNumber, sizeof(m_SerialNumber), m_LicPath);

	::GetPrivateProfileStringA(APP_NAME_USER, KEY_NAME_ACCOUNT, "*|", m_Account, sizeof(m_Account), m_LicPath);
	::GetPrivateProfileStringA(APP_NAME_USER, KEY_NAME_USERNAME, "*|", m_UserName, sizeof(m_UserName), m_LicPath);
	::GetPrivateProfileStringA(APP_NAME_LICENSE, KEY_NAME_MAC, "*|", m_MAC, sizeof(m_MAC), m_LicPath);

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
}

void CLicense::AddUser(const char* account, const char* name)
{
	m_UserMap[string(account)] = string(name);
}

bool CLicense::SaveIni()
{
	char tmp[32] = { 0 };
	_itoa(m_ExpireDate, tmp, 10);
	BOOL bRet = ::WritePrivateProfileStringA(APP_NAME_LICENSE, KEY_NAME_EXPIRE_DATE, tmp, m_LicPath);
	_itoa(m_Trial, tmp, 10);
	::WritePrivateProfileStringA(APP_NAME_LICENSE, KEY_NAME_TRIAL, tmp, m_LicPath);

	//::WritePrivateProfileStringA(APP_NAME_LICENSE, KEY_NAME_SERIAL_NUMBER, m_SerialNumber, m_LicPath);

	m_Account[0] = 0;
	m_UserName[0] = 0;

	for (map<string, string>::iterator it = m_UserMap.begin(); it != m_UserMap.end(); ++it)
	{
		strcat(m_Account, it->first.c_str());
		strcat(m_Account, "|");
		strcat(m_UserName, it->second.c_str());
		strcat(m_UserName, "|");
	}

	::WritePrivateProfileStringA(APP_NAME_USER, KEY_NAME_ACCOUNT, m_Account, m_LicPath);
	::WritePrivateProfileStringA(APP_NAME_USER, KEY_NAME_USERNAME, m_UserName, m_LicPath);

	m_MAC[0] = 0;
	for (set<string>::iterator it = m_MacSet.begin(); it != m_MacSet.end(); ++it)
	{
		strcat(m_MAC, it->c_str());
		strcat(m_MAC, "|");
	}

	::WritePrivateProfileStringA(APP_NAME_LICENSE, KEY_NAME_MAC, m_MAC, m_LicPath);

	if (!bRet)
	{
		sprintf(m_ErrorInfo, ERROR_CODE_11);
		m_ErrorCode = -11;
	}

	return bRet;
}

//void CLicense::SaveSerialNumber()
//{
//	char buf[512] = { 0 };
//	CreateContent(buf);
//	char md5[64] = { 0 };
//	CreateMD5(buf, md5);
//	strcpy(m_SerialNumber, md5);
//}

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

//void CLicense::CreateContent(char buf[])
//{
//	strcat(buf, "#");
//	m_Account[0] = 0;
//	m_UserName[0] = 0;
//	for (map<string, string>::iterator it = m_UserMap.begin(); it != m_UserMap.end(); ++it)
//	{
//		strcat(m_Account, it->first.c_str());
//		strcat(m_Account, "|");
//		strcat(m_UserName, it->second.c_str());
//		strcat(m_UserName, "|");
//	}
//	strcat(buf, m_Account);
//	strcat(buf, m_UserName);
//	strcat(buf, "#");
//	m_MAC[0] = 0;
//	for (set<string>::iterator it = m_MacSet.begin(); it != m_MacSet.end(); ++it)
//	{
//		strcat(m_MAC, it->c_str());
//		strcat(m_MAC, "|");
//	}
//	strcat(buf, m_MAC);
//	strcat(buf, "#");
//	char tmp[32] = { 0 };
//	_itoa(m_Trial, tmp, 10);
//	strcat(buf, tmp);
//	strcat(buf, "#");
//	_itoa(m_ExpireDate, tmp, 16);// 注意，这里乱搞了一个16
//	strcat(buf, tmp);
//	strcat(buf, "#");
//
//	// 发现没有什么功能限制，时间不能太长
//	m_bHasStarAccount = strstr(m_Account, "*");
//	m_bHasStarUserName = strstr(m_UserName, "*");
//	m_bHasStarMAC = strstr(m_MAC, "*");
//}
//
//void CLicense::CreateMD5(char input[], char output[])
//{
//	char priKey[128] = { 0 };
//	char pubKey[128] = { 0 };
//	char seed[1024] = { 0 };
//
//	// 生成 RSA 密钥对
//	strcpy(priKey, "pri");  // 生成的私钥文件
//	strcpy(pubKey, "pub");  // 生成的公钥文件
//	strcpy(seed, "seed");
//	GenerateRSAKey(1024, priKey, pubKey, seed);
//
//	// RSA 加解密
//	char message[1024] = { 0 };
//	cout << "Origin Text:\t" << "Hello World!" << endl << endl;
//	strcpy(message, "Hello World!");
//	string encryptedText = RSAEncryptString(pubKey, seed, message);  // RSA 加密
//	cout << "Encrypted Text:\t" << encryptedText << endl << endl;
//	string decryptedText = RSADecryptString(priKey, encryptedText.c_str());  // RSA 解密
//	//string decryptedText = RSADecryptString(priKey, "6014655DB01F43C33B292ADB2AD6BA567EF156BD8380CC1C3C79987523BB9416EE87970CA99BE79E958D0AF3FB5B17F43A52E12EEDDD3B23C07FD2EE4C3D9ACFBE10BD9B4B752B1ADD738ED74E9ED76650AB8ED76E76767F77DD311F88BCD2B15E4C106A552DED2D241910F1EDF881095DF34F8CDA16B502BF51FF3DE56FC37E");  // RSA 解密
//	cout << "Decrypted Text:\t" << decryptedText << endl << endl;
//}

int CLicense::GetErrorCode()
{
	// 认证通过
	m_ErrorCode = 0;

	do
	{
		if (!m_bLoaded)
		{
			strcpy(m_ErrorInfo, ERROR_CODE_1);
			m_ErrorCode = -1;
			break;
		}

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
		
		//char buf[1024] = { 0 };
		//CreateContent(buf);
		//char md5[1024] = { 0 };
		//CreateMD5(buf, md5);

		/*if (_stricmp(md5, m_SerialNumber) == 0)
		{
			
		}
		else
		{
			m_ErrorCode = -2;
			strcpy(m_ErrorInfo, ERROR_CODE_2);
			break;
		}*/

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
