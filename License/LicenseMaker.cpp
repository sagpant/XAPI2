// License.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <conio.h>
#include <iostream>

#include "License.h"


// 文件选择信息
CHAR strFile[MAX_PATH] = { 0 };
CHAR strFileTitle[MAX_PATH] = { 0 };

CHAR strFilter[MAX_PATH] = "*.exe\0*.exe\0TdxW.exe\0TdxW.exe\0Tc.exe\0Tc.exe\0all\0*.*\0";

BOOL OpenFileDlg()
{
	OPENFILENAMEA ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = strFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(strFile);
	ofn.lpstrFilter = strFilter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = strFileTitle;
	ofn.lpstrFileTitle[0] = ('\0');
	ofn.nMaxFileTitle = sizeof(strFileTitle);
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	return GetOpenFileNameA(&ofn);
}

void Select1()
{
	char priKey[128] = { 0 };
	char pubKey[128] = { 0 };
	char seed[1024] = { 0 };

	char fname[64] = { 0 };

	cout << "请输入文件名，不要带扩展名，系统将自动添加扩展名" << endl;
	cin >> fname;

	// 生成 RSA 密钥对
	sprintf_s(priKey, "%s.PrivateKey", fname);  // 生成的私钥文件
	sprintf_s(pubKey, "%s.PublicKey", fname);
	strcpy(seed, "seed");
	GenerateRSAKey(1024, priKey, pubKey, seed);

	cout << "已经生成在当前目录下，您需要自己修改文件名" << endl;
}

void Select2()
{
	cout << "选择授权文件：" << endl;

	CLicense license;

	memcpy(strFilter, "*.License\0*.License\0All\0*.*\0", sizeof(strFilter));
	if (OpenFileDlg())
	{
		// 设置授权文件
		license.SetLicensePath(strFile);

		string s = license.LoadStringFromFile(strFile);
		
		// 签名
		license.Sign(s.c_str());

		/*string k = license.LoadStringFromFile(license.m_PublicKeyPath);

		bool a = license.Verify(s.c_str(),k.c_str());
		int  b = 1;*/
	}

	cout << "签名已经生成，请查看" << endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "=====授权生成工具v1.0=====" << endl;
SELECT:
	cout << "请选择：" << endl;
	cout << "  1.生成公钥文件和私钥文件" << endl;	//
	cout << "  2.使用已有私钥签名文件" << endl;//
	cout << "  0.退出" << endl;		//
	cout << endl;

	int select = _getch();
	cout << (char)select << endl;
	switch (select - '0')
	{
	case 1:
		Select1();
		goto SELECT;
		break;
	case 2:
		Select2();
		goto SELECT;
		break;
	case 0:
		break;
	default:
		goto SELECT;
		break;
	}


	printf("按任意键关闭");
	getchar();

	return 0;
}

