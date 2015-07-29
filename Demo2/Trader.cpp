
#include "USTPFtdcTraderApi.h"
#include"USTPFtdcMduserApi.h"
#include "USTPFtdcUserApiDataType.h"
#include"USTPFtdcUserApiStruct.h"
#include <iostream>
#include <string.h>
#include <Windows.h>
#pragma comment(lib,"USTPtraderapi.lib")

using namespace std;
//报单录入操作是否完成的标志
HANDLE g_hEvent = CreateEvent(NULL, true, false, NULL);
//定义经纪商ID
TUstpFtdcBrokerIDType g_BrokerID;
//定义交易者ID
TUstpFtdcUserIDType g_UserID;
//定义合约代码
TUstpFtdcInstrumentIDType g_InstrumentID;
//定义用户本地订单号
int g_UserOrderLocalID;
//定义投资者Trader代码
TUstpFtdcInvestorIDType g_InvesterID;

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
	}
	//断开连接
	virtual void OnFrontDisconnected(int nReason)
	{
		cout << "On Front Disconnected" << endl;
	}
	//通知是否登录成功
	virtual void OnRspUserLogin(CUstpFtdcRspUserLoginField *gRspUserLogin, CUstpFtdcRspInfoField *gRspInfo, int nRsqID, bool bIsLast)
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
		}
		if (gRspInfo->ErrorID == 0)
		{
			cout << "登录成功！" << endl;
		}
		//用户最大本地报单号
		g_UserOrderLocalID = atoi(gRspUserLogin->MaxOrderLocalID) + 1;
		//登陆成功，发送报单录入请求
		CUstpFtdcInputOrderField ord;
		memset(&ord, 0, sizeof(ord));
		//经纪公司代码
		strcpy(ord.BrokerID, g_BrokerID);
		//用户代码
		strcpy(ord.UserID, g_UserID);
		//输入合约代码
		cout << "请输入合约代码" << endl;
		cin >> g_InstrumentID;
		strcpy(ord.InstrumentID, g_InstrumentID);
		//输入投资者代码
		cout << "请输入投资者代码" << endl;
		cin >> g_InvesterID;
		strcpy(ord.InvestorID, g_InvesterID);
		//输入买卖方向
		while (true)
		{
			cout << "请输入买卖方向：1表示买，2表示卖" << endl;
			cin >> BuyDirection;
			if (BuyDirection == 1)
			{
				ord.Direction = USTP_FTDC_D_Buy;
				break;
			}
			else if (BuyDirection == 2)
			{
				ord.Direction = USTP_FTDC_D_Sell;
				break;
			}
			else
			{
				cout << "输入买卖方向非法！请重新输入！" << endl;
			}
			if (BuyDirection == 1 || BuyDirection == 2) break;
		}
		//输入开平标志
		while (true)
		{
			cout << "请输入开平标志：0表示开仓，1表示平仓，2表示强平，3表示平今，4表示平昨" << endl;
			cin >> BuyFlag;
			switch (BuyFlag)
			{
		    //0表示开仓
			case 0:ord.OffsetFlag = USTP_FTDC_OF_Open; break;
			//1表示平仓
			case 1:ord.OffsetFlag = USTP_FTDC_OF_Close; break;
			//2表示强平
			case 2:ord.OffsetFlag = USTP_FTDC_OF_ForceClose; break;
			//3表示平今
			case 3:ord.OffsetFlag = USTP_FTDC_OF_CloseToday; break;
			//4表示平昨
			case 4:ord.OffsetFlag = USTP_FTDC_OF_CloseYesterday; break;
			//其余报错
			default: cout << "非法的开平标志！" << endl; break;
			}
			if (BuyFlag == 1||BuyFlag == 2||BuyFlag == 3||BuyFlag == 4||BuyFlag == 0)
				break;
		}
		//设置投机套保标志为投机
		ord.HedgeFlag = USTP_FTDC_CHF_Speculation;
		//设置限定价格
		cout << "请输入价格" << endl;
		cin >> ord.LimitPrice;
		//设定数量
		cout << "请输入数量" << endl;
		cin >> ord.VolumeCondition;
		//
		while (true)
		{
			cout << "请输入有效期类型：1表示立即完成否则撤销；2表示本节有效；3表示当日有效；4表示指定日期前有效；5表示撤销前有效；6表示集合竞价有效" << endl;
			cin >> BuyTimeCondition;
			switch (BuyTimeCondition)
			{
				//1表示立即完成否则撤销
			case 1:ord.TimeCondition = USTP_FTDC_TC_IOC; break;
				//2表示本节有效
			case 2:ord.TimeCondition = USTP_FTDC_TC_GFS; break;
				//3表示当日有效
			case 3:ord.TimeCondition = USTP_FTDC_TC_GFD; break;
				//4表示指定日期前有效
			case 4:ord.TimeCondition = USTP_FTDC_TC_GTD; break;
				//5表示撤销前有效
			case 5:ord.TimeCondition = USTP_FTDC_TC_GTC; break;
				//6表示集合竞价有效
			case 6:ord.TimeCondition = USTP_FTDC_TC_GFA; break;
				//其余报错
			default: cout << "非法的有效期类型！" << endl; break;
			}
			if (BuyTimeCondition == 1 || BuyTimeCondition == 2 || BuyTimeCondition == 3 || BuyTimeCondition == 4 || BuyTimeCondition == 5 || BuyTimeCondition==6)
				break;
		}
		//自动挂起标志
		ord.IsAutoSuspend = 0;
		//交易所
		strcpy(ord.ExchangeID, "CFFEX");
		//本地报单号
		sprintf(ord.UserOrderLocalID, "%012d", g_UserOrderLocalID++);
		m_pUserID->ReqOrderInsert(&ord, 1);
	}
	
	//报单录入应答
	virtual void OnRspOrderInsert(CUstpFtdcRspUserLoginField *pInputOrder, CUstpFtdcRspInfoField *pRspInfo, int nRequestId, bool bIsLast)
	{
		//输出录入结果
		cout << "ErrorCode=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
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
	int BuyDirection;
	int BuyFlag;
	int BuyHedge;
	int BuyTimeCondition;
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
	cout << "请输入要做的工作" << endl;
	//pTrader->Release();
	getchar();
}