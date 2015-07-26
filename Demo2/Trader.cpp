
#include "USTPFtdcTraderApi.h"
#include"USTPFtdcMduserApi.h"
#include "USTPFtdcUserApiDataType.h"
#include"USTPFtdcUserApiStruct.h"
#include <iostream>
#include <string.h>
#include <Windows.h>
#pragma comment(lib,"USTPtraderapi.lib")

using namespace std;
//���徭����ID
TUstpFtdcBrokerIDType g_BrokerID;
//���彻����ID
TUstpFtdcUserIDType g_UserID;

class CSimpleHandler :public CUstpFtdcTraderSpi
{
public:
	//���캯��
	CSimpleHandler(CUstpFtdcTraderApi *pUserID):m_pUserID(pUserID){}
	//��������
	~CSimpleHandler(){}
	//��д���ӵ�����������
	virtual void OnFrontConnected()
	{
		CUstpFtdcReqUserLoginField reqUserLogin;
		//��þ�����ID
		memset(&reqUserLogin, 0, sizeof(reqUserLogin));
		cout << "�����뾭����ID" << endl;
		cin >> g_BrokerID;
		strcpy(reqUserLogin.BrokerID, g_BrokerID);
		//��ÿͻ�ID
		cout << "������ͻ�ID" << endl;
		cin >> g_UserID;
		strcpy(reqUserLogin.UserID, g_UserID);
		//�������
		cout << "����������" << endl;
		cin >> reqUserLogin.Password;
		//���͵�¼Ҫ��
		int a = m_pUserID->ReqUserLogin(&reqUserLogin, 0);
		cout << ((a == 0) ? "�ɹ�" : "ʧ��") << endl;
		a = m_pUserID->ReqUserLogin(&reqUserLogin, 0);
		getchar();
	}
	//�Ͽ�����
	virtual void OnFrontDisconnected(int nReason)
	{
		cout << "On Front Disconnected" << endl;
	}
	//֪ͨ�Ƿ��¼�ɹ�
	virtual void OnRsqUserLogin(CUstpFtdcRspUserLoginField *gRspUserLogin, CUstpFtdcRspInfoField *gRspInfo, int nRsqID, bool bIsLast)
	{
		cout << "Error Code=" << gRspInfo->ErrorID << endl;
		cout << "Error Message=" << gRspInfo->ErrorMsg << endl;
		cout << "Requsest ID=" << nRsqID << endl;
		cout << "Chain=" << bIsLast << endl;
		//�����¼ʧ��
		if (gRspInfo->ErrorID != 0)
		{
			cout << "���������������Ĵ���������Ϣ���µ���" << endl;
			getchar();
			exit(-1);
		}
	}
	//�û���󱾵ر�����


private:
	CUstpFtdcTraderApi *m_pUserID;

};

int main()
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
	return 0;
}