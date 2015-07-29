
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

//报单录入操作是否完成的标志
//Create a manual reset event with no signal
HANDLE g_hEvent = CreateEvent(NULL, true, false, NULL);

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


	//报单录入应答
	virtual void OnRspOrderInsert(CUstpFtdcRspUserLoginField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestId, bool bIsLast)
	{
		//输出录入结果
		cout << "ErrorCode=" << pRspInfo->ErrorID << ", ErrorMsg="<<pRspInfo->ErrorMsg<<endl;
		//通知报单录入完成
		SetEvent(g_hEvent);
	}

	//报单回报
	virtual void OnRtnOrder(CUstpFtdcOrderField *pOrder)
	{
		cout << "OnRtnOrder:" << endl;
		cout << "OrderSysID=" << pOrder->OrderSysID << endl;
	}

	//针对用户请求的出错通知
	virtual void OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		cout << "OnRspError:" << endl;
		cout << "ErrorCode=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
		cout << "RequestID=" << nRequestID << ", Chain=" << bIsLast << endl;
		//客户端需要进行错误处理
		{
			//客户端的错误处理
		}
	}

private:
	CUstpFtdcTraderApi *m_pUserID;

	//指向CUstpFtdcMduserApi实例的指针
	CUstpFtdcTraderApi *m_pUserApi;
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