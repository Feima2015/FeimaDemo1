
#include "USTPFtdcTraderApi.h"
#include"USTPFtdcMduserApi.h"
#include "USTPFtdcUserApiDataType.h"
#include"USTPFtdcUserApiStruct.h"
#include <iostream>
#include <string.h>
#include <Windows.h>
#pragma comment(lib,"USTPtraderapi.lib")

using namespace std;
//定义经纪商ID
TUstpFtdcBrokerIDType g_BrokerID;
//定义交易者ID
TUstpFtdcUserIDType g_UserID;

class CSimpleHandler :public CUstpFtdcTraderSpi
{
public:
	//构造函数
	CSimpleHandler(CUstpFtdcTraderApi *pUserID):m_pUserID(pUserID){}
	//析构函数
	~CSimpleHandler(){}
	//重写连接到服务器方法
	virtual void OnFrontConnected()
	{
		CUstpFtdcReqUserLoginField reqUserLogin;
		//获得经纪商ID
		memset(&reqUserLogin, 0, sizeof(reqUserLogin));
		cout << "请输入经纪商ID" << endl;
		cin >> g_BrokerID;
		strcpy(reqUserLogin.BrokerID, g_BrokerID);
		//获得客户ID
		cout << "请输入客户ID" << endl;
		cin >> g_UserID;
		strcpy(reqUserLogin.UserID, g_UserID);
		//获得密码
		cout << "请输入密码" << endl;
		cin >> reqUserLogin.Password;
		//发送登录要求
		int a = m_pUserID->ReqUserLogin(&reqUserLogin, 0);
		cout << ((a == 0) ? "成功" : "失败") << endl;
		a = m_pUserID->ReqUserLogin(&reqUserLogin, 0);
		getchar();
	}
	//断开连接
	virtual void OnFrontDisconnected(int nReason)
	{
		cout << "On Front Disconnected" << endl;
	}
	//通知是否登录成功
	virtual void OnRsqUserLogin(CUstpFtdcRspUserLoginField *gRspUserLogin, CUstpFtdcRspInfoField *gRspInfo, int nRsqID, bool bIsLast)
	{
		cout << "Error Code=" << gRspInfo->ErrorID << endl;
		cout << "Error Message=" << gRspInfo->ErrorMsg << endl;
		cout << "Requsest ID=" << nRsqID << endl;
		cout << "Chain=" << bIsLast << endl;
		//如果登录失败
		if (gRspInfo->ErrorID != 0)
		{
			cout << "出错！请参阅上面的错误代码和信息重新调试" << endl;
			getchar();
			exit(-1);
		}
	}
	//用户最大本地报单号


private:
	CUstpFtdcTraderApi *m_pUserID;

};

void main()
{
	CUstpFtdcTraderApi *pTrader = CUstpFtdcTraderApi::CreateFtdcTraderApi("");
	CSimpleHandler spi(pTrader);
	pTrader->RegisterSpi(&spi);
	pTrader->SubscribePrivateTopic(USTP_TERT_RESUME);
	pTrader->SubscribePublicTopic(USTP_TERT_RESUME);
	pTrader->SubscribeForQuote(USTP_TERT_RESUME);
	pTrader->RegisterFront("tcp://117.185.125.4:17198");
	pTrader->Init();
	pTrader->Join();
	//pTrader->Release();
	getchar();
}