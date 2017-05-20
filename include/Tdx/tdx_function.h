#pragma once

#include <stdio.h>

#include "tdx_struct.h"

#ifdef TDXAPI_EXPORTS
#define TDXAPI_API __declspec(dllexport)

// 由于接口实例中做了一次表头的缓存，直接删除后，下次再取将返回无效值，所以这功能只由接口来调用
TDXAPI_API void DeleteTableHeader(FieldInfo_STRUCT** ppHeader);
// 复制表头，注意要两个都需要清理
TDXAPI_API FieldInfo_STRUCT** CopyTableHeader(FieldInfo_STRUCT** ppHeader);

#else
#define TDXAPI_API __declspec(dllimport)

// 在编写代码时发现FILE不能跨DLL
void OutputCSVTableHeader(FILE* pFile, FieldInfo_STRUCT** ppHeader);

void OutputCSVTableBody(FILE* pFile, char** ppTable);
void OutputCSVTableBody(FILE* pFile, char** ppTable, int count);

void OutputCSVError(FILE* pFile, Error_STRUCT* pErr);
void OutputCSVErrors(FILE* pFile, Error_STRUCT** pErrs);
void OutputCSVErrors(FILE* pFile, Error_STRUCT** pErrs, int count);

#endif

//////////////////////////////////////////////////////////////////////////
// 表头处理

// 打印表头信息，实际是一个指针数组，指针指向字段信息，数组最后一个指针为null
TDXAPI_API void PrintTableHeader(FieldInfo_STRUCT** ppHeader);

// 根据字段名得到列索引
TDXAPI_API int GetIndexByFieldName(FieldInfo_STRUCT** ppHeader,char* FieldName);
// 根据字段ID得到列索引
TDXAPI_API int GetIndexByFieldID(FieldInfo_STRUCT** ppHeader, int FieldID);
// 得到表头列数
TDXAPI_API int GetCountTableHeader(FieldInfo_STRUCT** ppHeader);

//////////////////////////////////////////////////////////////////////////
// 表体处理

// 打印表体信息，实际是一个指针数组，指针指向字段信息，用一维数组表示二维数组，目前约定每行64列，每列中第1个元素不能为空
TDXAPI_API void PrintTableBody(char** ppTable);
TDXAPI_API void PrintTableBody(char** ppTable, int count);

// 取出的数据看情况是否要删除
TDXAPI_API void DeleteTableBody(char**  ppTable);
TDXAPI_API void DeleteTableBody(char** ppTable, int count);
// 得到某一行某一列，没有数据将返回null
TDXAPI_API char* GetAtTableBody(char** ppTable, int row, int col);
// 只有非空的行才需要数，而中间出现空的只表示你已经知道表行数了，没有必要数
TDXAPI_API int GetRowCountTableBody(char** ppTable);

//////////////////////////////////////////////////////////////////////////
// 错误处理
TDXAPI_API void PrintError(Error_STRUCT* pErr);
TDXAPI_API void PrintErrors(Error_STRUCT** pErrs);
TDXAPI_API void PrintErrors(Error_STRUCT** pErrs, int count);

TDXAPI_API void DeleteError(Error_STRUCT* pErr);
TDXAPI_API void DeleteErrors(Error_STRUCT** pErrs);
TDXAPI_API void DeleteErrors(Error_STRUCT** pErrs, int count);
// 得到错误数
TDXAPI_API int GetCountErrors(Error_STRUCT** pErrs);
//////////////////////////////////////////////////////////////////////////
TDXAPI_API int GetCountStructs(void** ppResults);
// 删除，注意，也存在Debug与Release的问题，所以在别的dll中生成的内存最好也在别的地方删
TDXAPI_API void DeleteStructs(void*** pppStructs);
//////////////////////////////////////////////////////////////////////////
// 股东列表处理，用于授权部分
TDXAPI_API void CharTable2GDLB(FieldInfo_STRUCT** ppFieldInfos_1122, char** ppTable, GDLB_STRUCT*** pppResults, void* Client);
// 由于部分券商股东列表中没有资金账号，只能从登录信息中另外获取
TDXAPI_API void CharTable2Login(FieldInfo_STRUCT** ppFieldInfos_1122, char** ppTable, GDLB_STRUCT*** pppResults, void* Client, FieldInfo_STRUCT** ppFieldInfos_148, char* CPZH);
// 删除请求和响应，进行内存释放
TDXAPI_API void DeleteRequestRespone(RequestResponse_STRUCT* pRespone);

//////////////////////////////////////////////////////////////////////////
// 打新结构体解析

// 新股申购额度查询
TDXAPI_API void CharTable2XGSGEDCX(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, XGSGEDCX_STRUCT*** pppResults, void* Client);
// 可申购新股查询
TDXAPI_API void CharTable2KSGXGCX(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, KSGXGCX_STRUCT*** pppResults, void* Client);
//// 新股配号查询，只有收盘后或第二天才能查询到
//TDXAPI_API void CharTable2XGPHCX(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, XGPHCX_STRUCT*** pppResults, void* Client);

//////////////////////////////////////////////////////////////////////////
// 行情解析
TDXAPI_API void CharTable2HQ(FieldInfo_STRUCT** ppFieldInfos, char** ppTable, HQ_STRUCT*** pppResults, void* Client);


