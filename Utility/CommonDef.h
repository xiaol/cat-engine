/***************************************************
Copyright (c) 2009, IvanJ.
All rights reserved.

File:    CommonDef.h
Purpose: 
Version: 0.1

****************************************************/


#ifndef _COMMONDEF_H_INCLUDED_
#define _COMMONDEF_H_INCLUDED_

#define WIN32_LEAN_AND_MEAN
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <Windows.h>
#include <io.h>
#include <direct.h>
#include <time.h>
#include <math.h>

#pragma warning(disable : 4996)
using namespace std;

typedef unsigned long   ULONG;
typedef unsigned char   BYTE;
typedef unsigned short  USHORT;
typedef __int64         LLONG;
typedef long            LONG;
typedef double          DOUBLE;

typedef string          STRING;
typedef vector<STRING>  VEC_STR;
typedef vector<ULONG>   VEC_ULONG;
typedef vector<USHORT>  VEC_USHORT;
typedef vector<BYTE>    VEC_BYTE;
typedef vector<LLONG>   IDVector;
typedef vector<ULONG>   IndexVector;

#define FLOAT_EQUAL( x1, x2 )       ( ( x1 > x2 || x1 < x2  ) ? false : true )
#define FLOAT_NOTEQUAL( x1, x2 )    ( ( x1 > x2 || x1 < x2  ) ? true : false )

#define FLOAT_TO_LLONG( dx )        ( LLONG( (dx) + 0.5 ) )
#define FLOAT_TO_LONG( dx )         ( long( (dx) + 0.5 ) )
#define FLOAT_TOULONG( dx )         ( ULONG( (dx) + 0.5 ) )
#define GET_MESHYLINE_COOR( d )     (double)( FLOAT_TO_LLONG( (d) * 10.0 ) / 10 )
#define TOLL_TYPE					33792		//四维POI中收费站的种类
#define TOLL_NODE					4609		//收费节点的属性值
#define MAX_COUNT					5000		//一次插入的最多条数
#define MAX_BUFSIZE					1024

//数据类型
enum DataType
{
	DATA_SW150,
	DATA_SW152,
	DATA_SW,
	DATA_YTT,
	DATA_RT
};

//行政区类型
enum AdminType
{
	COUNTRY_ADMIN,				//国行政区划编码
	PROV_ADMIN,					//省行政区划编码
	CITY_ADMIN,					//地级市、省直辖行政单位行政区划编码	
	COUNTY_ADMIN,				//县、区行政区划编码	
	CLIP_ADMIN
};

//图幅边界点类型
enum BoundType
{
	SIMPLE,						//普通点
	MESHBOUNDNODE,				//源数据图幅边界点
	DISTRICTBOUNDNODE,			//行政区划分割点
	CROSSLINKCUT,				//复合路口共用外link分割点,引导在复合路口的link的切分点
	PROVINCEBOUNDNODE			//省图幅边界点,用于全国数据合并
};

//日志类型
enum LogType
{
	LT_ERROR        = 0,		//错误信息
	LT_MSG          = 1,		//详细消息信息
	LT_TIMECOUNT    = 2,		//时间统计信息
	LT_TOTAL		= 3			//时间统计信息
};

//节点路口类型
enum CrossType
{
	NOT_CROSS_NODE,				//非路口节点
	SUB_NODE,					//复合路口子节点
	SINGLE_CROSS,				//单一路口节点
	MAIN_NODE					//复合路口主节点
};

//RECT与RECT的位置关系标记
enum RectToRectRelation			
{
	RRF_INSRC,					//目标RECT在当前RECT内部
	RRF_INDEST,					//当前RECT在目标RECT内部
	RRF_INTERSECT,				//相交
	RRF_EQUAL,					//相等
	RRF_DISCRETE				//相离	
};

//由于SW1.5数据把数据元素与名称分开存放,以下是各元素在名称表的类型
enum NameType
{
	IC_NAME = 3,				//IC名称
	DR_NAME = 4,				//DR名称
	BL_NAME = 5,				//背景线名称
	BP_NAME = 6,				//背景面名称
	T_NAME  = 8,				//背景点名称
	POI_NAME = 9,				//POI名称
	POI_ADDRESS = 10,			//POI地址	
	ADMIN_NAME	= 14			//行政区划名称
};

//为保证ID的唯一性，在各种类型的ID前面加上特定的前缀
enum IDType
{
	ID_LINK,		//link id
	ID_NODE,		//node id
	ID_RULE,		//规制 id
	ID_COND,		//条件规制id
	ID_IC,			//ic id
	ID_DR,			//dr id
	ID_LANE,		//车道 id
	ID_POI,			//poi id
	ID_CAMERA,		//电子眼 id
	ID_BACK,		//背景 id
	ID_DM,			//DM ID
	ID_BR,			//BR ID
	ID_NONE
};

//转换区域类型：城市、省、全国
enum ConvertAreaType
{
	CITY,
	PROV,
	COUNTRY
};

//引导图片类型
enum ImageType
{
	HIGHWAY_CROSS = 1,		//1：高速分歧模式图
	HIGHWAY_ENTRANCE,		//2：高速出口放大图
	HIGHWAY_EXIT,			//3：高速入口放大图
	PATTERN_3D				//4：3d模式图
};

static string IDArray[ID_NONE] = {"110", "120", "130", "140", "150", "160", "170", "180", "190", "200"};

//数据库连接对象结构体
struct DBInfo
{
	string strServerIP;			//机器名
	string strPort;				//数据库访问端口号
	string strDBName;			//数据库名称
	string strUserName;			//用户名
	string strPassword;			//密码
};

//配置文件对象，包含配置文件里所有的信息
struct ConfigFile
{
	vector<string> CreateTableSql;				//创建表的语句
	DBInfo ServerInfo;							//全国库访问信息
	DBInfo LocalInfo;							//单省库访问信息
	string strTableSpaceName;					//表空间名称
	string strConvertIP;						//转换机

	multimap<string, string>	mpConvertArea;			//转换机对应转换区域信息
	map<ULONG, ULONG>			mpDistrictCombine;		//行政区划自定义分割方案
	map<ULONG, ULONG>			mpClipByMesh;			//图幅切割的行政区(目前只有东莞)
	map<ULONG, string>			mpAdminCodeToName;		//行政区划编码与行政区名称的对应

	string strFolderPath;						//源数据所在地址
	map<ConvertAreaType, string> mpFoldName;	//保存不同层文件夹名称，level0、level1、level2
	map<ConvertAreaType, VEC_STR> mpBkgLineType;//保存不同层背景线的显示种别
	map<ConvertAreaType, VEC_STR> mpBkgAreaType;//保存不同层背景面的显示种别
	vector<ULONG>	vProvZhixiaCity;			//省直辖行政单位
};

//为源数据重新设置id
static LLONG MakeID(const STRING& strID, IDType idType)
{
	if (strID == "0" || strID == "")
	{
		return 0;
	}
	string strNewID = IDArray[idType] + strID;
	return _atoi64(strNewID.c_str());
}

static double MyAtof(char* SrcStr)
{
	char buf[256];
	char* pSrc = SrcStr;
	char* pDest = buf;
	int iDec = 0;
	bool bDecBegin = false;
	while( *pSrc != '\0' )
	{
		if( *pSrc >= '0' && *pSrc <= '9' )
		{
			*pDest = *pSrc;
			pDest++;
			if( bDecBegin )
			{
				iDec++;
			}
		}
		else if( *pSrc == '.' )
		{
			bDecBegin = true;
		}
		pSrc++;
	}
	*pDest = '\0';
	__int64 ret = _atoi64( buf );
	double dRet = ret / pow( 10.0, iDec );
	return dRet;
}

//将源数据提供的id串分割为id数组
static void SetIDVector(IDVector& IDVec, const STRING& strSrcField, char chSplite, const IDType& type)
{
	if (strSrcField == "")
	{
		return;
	}
	string strTemp = strSrcField;
	size_t ipos = strTemp.find(chSplite);
	LLONG llID = 0;
	while(ipos != string::npos)
	{
		llID = MakeID(strTemp.substr(0, ipos), type);
		IDVec.push_back(llID);
		strTemp = strTemp.substr(++ipos);
		ipos = strTemp.find(chSplite);
	}
	if (strTemp != "0")
	{
		llID = MakeID(strTemp.substr(0, ipos), type);
		IDVec.push_back(llID);
	}
}

static void MyTolower(char* str)
{
	size_t i = strlen(str);
	int pos = 'A' - 'a';
	for (size_t j = 0; j < i; j++)
	{
		if (str[j] >= 'A' && str[j] <= 'Z')
		{
			str[j] -= pos;
		}
	}
}

//将源数据mid文件的一行按照","进行分割
static void GetField(const string& strSource, VEC_STR& vField )
{
	size_t sSubstrStart = 0;
	size_t sSubstrEnd = 0;
	size_t sOldStart = 0;

	while (1)
	{
		sSubstrEnd = strSource.find( ',', sSubstrStart );
		if ( sSubstrEnd != string::npos )
		{
			string field = strSource.substr( sOldStart, sSubstrEnd - sOldStart );
			size_t find = field.find( '\"');
			if( find != string::npos && find == field.rfind( '\"' ) )
			{
				sSubstrStart = sSubstrEnd + 1;
			}
			else
			{
				sOldStart = sSubstrEnd + 1;
				sSubstrStart = sOldStart;

				size_t nPos = 0;
				while ( ( nPos = field.find('\"', nPos ) ) != string::npos )
				{
					field.erase(nPos, 1);
				}
				vField.push_back( field );
			}
		}
		else
		{
			break;
		}
	}
	size_t nPos = 0;
	string lastField = strSource.substr( sSubstrStart );
	while ( ( nPos = lastField.find( '\"', nPos ) ) != string::npos )
	{
		lastField.erase( nPos, 1 );
	}
	vField.push_back( lastField );
}

static void GetField(char* buf, char* AttrBuf, vector<char*>& strVec)
{
	char* pSrc = buf;
	char* pOne = AttrBuf;
	char* pos = pOne;
	while (*pSrc)
	{
		if (*pSrc == '\"')				//引号+逗号分割
		{
			++pSrc;
			while (*pSrc != '\"' ||
				(*(pSrc + 1) && *(pSrc + 1) != 0x0a && *(pSrc + 1) != ','))
			{
				if (*pSrc == '\"' && *(pSrc + 1) == '\"')
				{
					pSrc++;
				}
				else
				{
					*pos++ = *pSrc++;
				}
			}
			++pSrc;
		}
		else							//逗号分割
		{
			while (*pSrc != ',' && *pSrc)
			{
				*pos++ = *pSrc++;
			}
		}
		*pos = '\0';
		pos = pOne;
		strVec.push_back(pos);
		pOne += strlen(pos) + 1;
		pos = pOne;

		if (!*pSrc)
		{
			break;
		}
		++pSrc;
	}
}

//去掉字符串左右两侧的空格
static void TrimLeftAndRight(string& strSrc)
{
	size_t i = 0, j = 0;
	for (; i < strSrc.size(); i++)
	{
		if (strSrc[i] != ' ')
		{
			break;
		}
	}
	for (j = strSrc.size() - 1; j > 0; j--)
	{
		if (strSrc[j] != ' ')
		{
			break;
		}
	}
	strSrc = strSrc.substr(i, j - i + 1);
}

//根据点的坐标获取其所在map
static int GetMapIDByPoint(double dx, double dy)
{
	if ( dx < 0.000006 || dy < 0.000006 )
	{
		return 0;
	}
	//如595673fe前面四位
	double dxTail, dyTail;
	int ndx, ndy;
	int nX, nY;  
	int nX2, nY2;	//第五位和第六位
	dy = dy * 3.0 / 2;

	ndx	= (int)dx;
	ndy	= (int)dy;
	nX	= ndx - 60;	
	nY	= ndy;

	dxTail = dx - ndx;
	dyTail = dy - ndy;
	nX2 = (int)(dxTail / 0.125);
	nY2 = (int)(dyTail / 0.125);

	char buf[256];
	sprintf_s(buf, "%02d%02d%d%d", nY, nX, nY2, nX2);
	return atoi(buf);
}

//格式化时间规制中的时间信息
static void FormatHour(const string& strInHour, string& strOutHour)
{
	string strTemp = strInHour;
	transform(strTemp.begin(), strTemp.end(), strTemp.begin(), ::tolower);
	size_t iHpos = strTemp.find("h");
	strTemp = strTemp.substr(++iHpos);
	size_t iMpos = strTemp.find("m");
	if (iMpos == string::npos)
	{
		strTemp += ":00";
	}
	else
	{
		strTemp[iMpos] = ':';
	}
	strOutHour += strTemp;
}

static string DBCToSBC(const string& strSrc)
{
	wchar_t wcs[1024];
	int ret = MultiByteToWideChar( CP_ACP, 0, strSrc.c_str(), (int)strSrc.size(), wcs, 1024 );
	wcs[ret] = 0;
	for (int i = 0; i < ret; i++)
	{
		if (wcs[i] == 32)
		{
			wcs[i] = 12288;
			continue;
		}
		if (wcs[i] < 127)
			wcs[i] = (wchar_t)(wcs[i] + 65248);
	}
	char buf[2048];
	ret = WideCharToMultiByte( CP_ACP, 0,wcs, ret, buf, sizeof(buf), NULL, NULL );
	buf[ret] = '\0';
	return buf;
}

//根据特殊字符分割字符串
static void SeparateString(const char* pSrc, vector<char*>* pStrs)
{
	if( pSrc == NULL )
		return;
	size_t nLen = strlen( pSrc );
	if( nLen == 0 )
		return;

	size_t sum = strlen(pSrc);
	for(size_t i = 1, j = 1; i < sum; i++)
	{
		if((*(pSrc+i) == ';') || (*(pSrc+i) == ','))
		{
			char* str = new char[i-j+1];
			strncpy(str,pSrc+j,i-j);
			*(str+i-j) = '\0';

			pStrs->push_back(str);
			j = i+1;
		}
		else if(*(pSrc+i) == '}')
		{
			char* str = new char[i-j+1];
			strncpy(str,pSrc+j,i-j);
			*(str+i-j) = '\0';

			pStrs->push_back(str);
			break;
		}
		else if(*(pSrc+i) == '\0')
		{
			char* str = new char[i-j+1];
			strncpy(str,pSrc+j,i-j);
			*(str+i-j) = '\0';

			pStrs->push_back(str);
			j = i+1;
		}
	}
}

//将utf8编码的字符串转为asc编码的字符串
static bool UTF8ToASC(const wchar_t* pUnicode,string& asc)
{
	unsigned __int32 i64Num = WideCharToMultiByte(CP_ACP,NULL,pUnicode,-1,NULL,0,NULL,FALSE);
	char *psText;
	psText = new char[i64Num];
	if(!psText)
	{   
		delete []psText;
		return false;
	}
	WideCharToMultiByte (CP_ACP,NULL,pUnicode,-1,psText,i64Num,NULL,FALSE);
	asc = psText;
	delete[] psText;
	return true;
}

//将utf8编码的字符串转为asc编码的字符串
static bool UTF8ToASC(const char* pUnicode,string& asc)
{
	unsigned __int32 i64Num = MultiByteToWideChar(CP_UTF8,NULL,pUnicode,-1,NULL,0);
	wchar_t* psText;
	psText = new wchar_t[i64Num];
	if(!psText)
	{   
		delete []psText;
		return false;
	}
	MultiByteToWideChar(CP_UTF8,NULL,pUnicode,-1,psText,i64Num);
	UTF8ToASC(psText,asc);
	delete[] psText;
	return true;
}

//将Unicode编码的字符串转为utf8编码的字符串
static bool UnicodeToUTF8(const wchar_t* pUnicode,string& utf8 )
{
	unsigned __int32 i64Num = WideCharToMultiByte(CP_UTF8, NULL, pUnicode, -1, NULL, 0, NULL, FALSE);
	char *psText;
	psText = new char[i64Num];
	if(!psText)
	{   
		delete []psText;
		return false;
	}
	WideCharToMultiByte (CP_UTF8, NULL, pUnicode, -1, psText, i64Num, NULL, FALSE);
	utf8 = psText;
	delete[] psText;
	return true;
}

//将Unicode编码的字符串转为utf8编码的字符串
static bool UnicodeToUTF8(const char* pUnicode,string& utf8 )
{
	unsigned __int32 i64Num = MultiByteToWideChar(CP_NONE, NULL, pUnicode, -1, NULL, 0);
	wchar_t* psText;
	psText = new wchar_t[i64Num];
	if(!psText)
	{   
		delete []psText;
		return false;
	}
	MultiByteToWideChar(CP_NONE, NULL, pUnicode, -1, psText, i64Num);
	UnicodeToUTF8(psText, utf8 );
	delete[] psText;
	return true;
}

//将asc编码的字符串转为utf8编码的字符串
static bool ASC2UTF8(char *cASCII, char *cUTF8, int char_len)   
{   
	wchar_t wbuf[1024];   
	char utf8buf[1024];   

	ZeroMemory(wbuf,sizeof(wbuf));   
	int nret= MultiByteToWideChar(CP_ACP,0,cASCII,char_len,wbuf,sizeof(wbuf)/sizeof(wbuf[0]));   
	if(nret > 0 && nret < sizeof(wbuf))   
	{ 
		nret = WideCharToMultiByte(CP_UTF8,0,wbuf,nret,utf8buf,sizeof(utf8buf),NULL,NULL);   
		if(nret <= 0 && nret >= sizeof(utf8buf))   
			return false;   
	}
	else
		return false;   
	utf8buf[nret] = '\0';   
	strcpy(cUTF8, utf8buf);   
	return true;
}

static bool IsZhiXia(ULONG ulAdminCode)
{
	USHORT usPos = USHORT(ulAdminCode / 10000);
	if (usPos == 11 || usPos == 12 || usPos == 31 || usPos == 50)
	{
		return true;
	}
	return false;
}

static string GetCurrentData()
{
	time_t t = time(NULL);
	char szCurrentDate[64];
	strftime( szCurrentDate, sizeof( szCurrentDate ), "%Y%m%d", localtime(&t) ); 
	string strCurrentDate = szCurrentDate;
	return strCurrentDate;
}

static void CutString(const string& strSrc, char chBuf, VEC_STR& OutVec)
{
	string strTemp = strSrc;
	size_t ipos = strTemp.find_first_of(chBuf);
	while (ipos != string::npos)
	{
		OutVec.push_back(strTemp.substr(0, ipos));
		strTemp = strTemp.substr(++ipos);
		ipos = strTemp.find_first_of(chBuf);
	}
	OutVec.push_back(strTemp);
}

//指针向量对象释放
template <class T> 
void ReleaseVect( vector<T*>& vItems )
{
	size_t iSize = vItems.size();
	for (size_t i = 0; i < iSize; i++)
	{	
		if (vItems.at(i) != NULL)
		{
			delete vItems[i];
		}
	}
	vItems.clear();
}

//指针map对象释放
template< class Key, class Data >
void ReleaseMap( map<Key, Data>& mItems )
{
	for ( map<Key, Data>::iterator it = mItems.begin(); it != mItems.end(); ++it )
	{
		if (it->second != NULL)
		{ 
			delete it->second;
			it->second = NULL;
		}
	}
	mItems.clear();
}

//指针multimap对象释放
template< class Key, class Data >
void ReleaseMultimap( multimap<Key, Data>& mItems )
{
	for ( multimap<Key, Data>::iterator it = mItems.begin(); it != mItems.end(); ++it )
	{
		if (it->second != NULL)
		{ 
			delete it->second;
			it->second = NULL;
		}
	}
	mItems.clear();
}

template<class key, class data>
void InsertVecMap(key keyval, data* pData, map<key, vector<data*>>& mpValue)
{
	map<key, vector<data*>>::iterator itF = mpValue.find(keyval);
	if(itF == mpValue.end())
	{
		vector<data*> TempVec;
		TempVec.push_back(pData);
		mpValue.insert(make_pair(keyval, TempVec));
	}
	else
	{
		itF->second.push_back(pData);
	}
}
#endif