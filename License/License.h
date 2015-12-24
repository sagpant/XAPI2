#pragma once

#include <list>
#include <map>
#include <set>
#include <iostream>
#include <vector>

#include "randpool.h"
#include "rsa.h"
#include "hex.h"
#include "files.h"

// BOOLEAN与asn.h中的冲突，不得不将asn.h中的改了，希望有人帮忙解决这个部题解决可以不用改动
#include <windows.h>

#define ERROR_CODE_1	"没有读取到授权文件,将自动生成试用版授权到dll同目录下"
#define ERROR_CODE_2	"授权认证失败"
#define ERROR_CODE_3	"授权过期，过期时间：[%d]"
#define ERROR_CODE_4	"没有[用户号:%s]的授权"
#define ERROR_CODE_5	"机器码验证失败"
#define ERROR_CODE_6	"试用超次数,限制[%d]次"
#define ERROR_CODE_7	"授权产品名不对应"
#define ERROR_CODE_8	"[账号:%s]授权的[姓名:%s],现获得的为[%s],下单将被拒绝"
#define ERROR_CODE_9	"没有[姓名:%s]的授权"
#define ERROR_CODE_10	"授权范围太广时，到期时间不能过长"
#define ERROR_CODE_11	"生成授权文件失败，请检查是否有权限写入。或以管理员方式再次启动"

#define FILE_EXT_LIC "License"
#define FILE_EXT_PUB "PublicKey"
#define FILE_EXT_PRIV "PrivateKey"
#define FILE_EXT_SIGN "Signature"

using namespace std;
using namespace CryptoPP;

#pragma comment(lib, "../lib/cryptlib.lib")

void GenerateRSAKey(unsigned int keyLength, const char *privFilename, const char *pubFilename, const char *seed);
string RSAEncryptString(const char *pubFilename, const char *seed, const char *message);
string RSADecryptString(const char *privFilename, const char *ciphertext);
//void RSASignFile(const char *privFilename, const char *messageFilename, const char *signatureFilename);
//bool RSAVerifyFile(const char *pubFilename, const char *messageFilename, const char *signatureFilename);
void RSASignString(const char *privFilename, const char *messageFilename, const char *signatureFilename);
bool RSAVerifyString(const char *pubFilename, const char *messageFilename, const char *signatureFilename);

RandomPool & GlobalRNG();

/*
功能说明：
1.没有证书文件时要重新生成，生成的要能直接通过临时认证
2.有证书文件时，要判断是不是绑定了机器
3.还有试用检查
4.没有绑定的时间不能过长

应当检查一下，如果是试用版，就可以一直添加新用户和机器码，而正式用户就不可以了，这样就可以记录很多账户了

决定多加一些东西
1.记录一些权限信息.License
2.记录公钥.PublicKey
3.记录签名文件.Signature

4.记录私钥.PrivateKey

如果没有签名文件就使用临时授权，有签名文件就对照是否合有授权

如何防止用户自己生成公私钥后再签名
1.公钥不容易改，建议添加到资源中
2.授权文件再加一次密后再用来签名
*/
class CLicense
{
public:
	CLicense();
	~CLicense();

	void LoadIni();
	bool SaveIni();

	const char* GetErrorInfo();
	int GetErrorCode();
	int GetErrorCodeByTrial();
	int GetErrorCodeByAccount(const char* account);
	int GetErrorCodeByNameThenAccount(const char* name, const char* account);
	
	int Today(int day);

	void AddUser(const char* account, const char* name);

	//void CreateContent(char buf[]);

	//void CreateMD5(char input[],char output[]);

	void CreateDefault();

private:
	int m_ExpireDate;
	int m_Trial;
	int m_nCurrentTrial;
	
	char m_SerialNumber[128];

	char m_RealMAC[32];

	char m_LicPath[256];
	char m_ErrorInfo[256];
	int m_ErrorCode;

	map<string, string> m_UserMap;
	set<string> m_MacSet;
	char m_Account[1024];
	char m_UserName[1024];
	char m_MAC[1024];
	bool m_bLoaded;

	bool m_bHasStarAccount;
	bool m_bHasStarUserName;
	bool m_bHasStarMAC;
};

