#include "CapturePicture.h"
<<<<<<< HEAD
#include "dhnetsdk.h"
#include "DhDecode.h"
=======


>>>>>>> ee624059fa4d4e51d6412d2f829bd2779430e51a
#include <iostream>
#include <string>

using namespace std;


CapturePicture::CapturePicture(void)
{
	m_LoginID = 0;
	m_nChannelCount = 0;
	m_bJSON = FALSE;
	memset(&m_stuSnapAttr, 0, sizeof(m_stuSnapAttr));
	memset(&m_stuSnapAttrEx, 0, sizeof(m_stuSnapAttrEx));
	memset(&m_stuSnapCfg, 0, sizeof(m_stuSnapCfg));
	m_ctlChannel=0;
	m_ccbSnapMode=0;
	m_snapmode=0;
	m_serial=1;
}


CapturePicture::~CapturePicture(void)
{
}

void CapturePicture::Login(const char* IPadress)
{
	int nError=0;
	int nPort=0;
	char szIp[32]={0};
	char szUser[32]={0};
	char szPwd[32]={0};
	NET_DEVICEINFO deviceInfo;

	nPort=37777;
	sprintf(szIp,IPadress);
	sprintf(szUser,"admin");
	sprintf(szPwd,"admin");
	
	LONG lRet=CLIENT_Login(szIp,nPort,szUser,szPwd,&deviceInfo,&nError);

	if (0==lRet)
	{
		ShowLoginErrorReason(nError);
	}
	else
	{
		m_LoginID=lRet;
		m_nChannelCount=deviceInfo.byChanNum;

		int nIndex = -1;
		m_ctlChannel=0;
		for(int i=0;i<m_nChannelCount;i++)
		{
			m_ctlChannel=i;
		}


		DH_DEV_ENABLE_INFO devEnable={0};
		DWORD dwRetLen=0;
		BOOL bRet=CLIENT_QuerySystemInfo(m_LoginID,ABILITY_DEVALL_INFO,(char*)&devEnable,sizeof(DH_DEV_ENABLE_INFO),(int*)&dwRetLen,1000);
		if (bRet==FALSE)
		{
			cout<<"Query device ability failed.";
			cout<<endl;
			return;
		}
	    

		m_bJSON=devEnable.IsFucEnable[EN_JSON_CONFIG]>0?TRUE:FALSE;
		if (m_bJSON==FALSE)
		{
			int nRetLen=0;
			int nRet=CLIENT_QueryDevState(lRet,DH_DEVSTATE_SNAP,(char*)&m_stuSnapAttr,sizeof(DH_SNAP_ATTR_EN),&nRetLen,1000);
			if (nRet==FALSE||nRetLen!=sizeof(DH_SNAP_ATTR_EN))
			{
				cout<<"Get snap ability failled."<<endl;
				return;
			}
		} 
		else //json
		{
			memset(&m_stuSnapAttrEx, 0, sizeof(m_stuSnapAttrEx));
			memset(&m_stuSnapCfg, 0, sizeof(m_stuSnapCfg));
			InitSnapConfigExUI(0);
		}

		if (!m_bJSON)
		{
			int nRet=CLIENT_GetDevConfig(lRet,DH_DEV_SNAP_CFG,-1,(LPVOID)m_stuSnapCfg,sizeof(DHDEV_SNAP_CFG)*32,&dwRetLen,1000);
			if (nRet<0)
			{
				cout<<"Get snap configure failed."<<endl;
				return;
			}
		}
		else //json»ñÈ¡×¥Í¼ÅäÖÃ
		{
			GetSnapConfigEx(0);
		}

		//OnSelchangeComboChannel();

	}


}
void CapturePicture::ShowLoginErrorReason(int nError)
{
	if(1 == nError)		    cout<<"incorrect password! prompt";
	else if(2 == nError)	cout<<"no this account! ";
	else if(3 == nError)	cout<<"time out!";
	else if(4 == nError)	cout<<"account have been logined!";
	else if(5 == nError)	cout<<"account have been locked!";
	else if(6 == nError)	cout<<"The user has listed into illegal!";
	else if(7 == nError)	cout<<"The system is busy!";
	else if(9 == nError)	cout<<"You Can't find the network server!";
	else	cout<<"Login falied!";

	cout<<endl;
}

void CapturePicture::InitSnapConfigUI(int nChn)
{
	const DH_QUERY_SNAP_INFO& stuSnapInfo = m_stuSnapAttr.stuSnap[nChn];
	
	m_ccbSnapMode=0;
	for(int i = 0 ; i < stuSnapInfo.nSnapModeCount; i++)
	{
		if(stuSnapInfo.SnapMode[i] == 0)
		{	
			m_ccbSnapMode=0;
			cout<<"Timing snap"<<endl;
		}
		else if(stuSnapInfo.SnapMode[i] == 1)
		{
			m_ccbSnapMode=1;
			cout<<"Trigger snap"<<endl;
		}
	}
	char csFrame[50]={0};
	for(int i = 0; i < stuSnapInfo.nFramesCount; i++)
	{
		if(stuSnapInfo.Frames[i] > 0)
		{
			//csFrame.Format(ConvertString("%d frame pre second"), stuSnapInfo.Frames[i]);
			sprintf(csFrame,"%d frame pre second",stuSnapInfo.Frames[i]);
		}
		else if(stuSnapInfo.Frames[i] < 0)
		{
			//csFrame.Format(ConvertString("%d second pre frame"), abs(stuSnapInfo.Frames[i]));
			sprintf(csFrame,"%d frame pre second",abs(stuSnapInfo.Frames[i]));
		}

		cout<<csFrame<<endl;
	}
	char szSize[32][8] = {"D1",	"HD1","BCIF","CIF",
		"QCIF", "VGA", "QVGA", "SVCD",
		"QQVGA","SVGA",	"XVGA", "WXGA", 
		"SXGA", "WSXGA","UXGA", "WUXGA",
		"LTF",	"720p","1080p","1.3M",
		"2M","5M","3M","5M",
		"1.2M","1.5M","8M","5M"	};

	DWORD dwMask = 0x0001;
	
	for(int i = 0; i < 32; i++)
	{
		if( (stuSnapInfo.dwVideoStandardMask&dwMask) > 0)
		{
		cout<<szSize[i]<<endl;
		}
		dwMask <<= 1;
	}

	for(int i = 0; i < stuSnapInfo.nPicQualityCount; i++)
	{
		//csQuality.Format("%d%s", stuSnapInfo.PictureQuality[i], "%");
		//m_ccbQuality.SetItemData(m_ccbQuality.AddString(csQuality), stuSnapInfo.PictureQuality[i]);
		cout<<stuSnapInfo.PictureQuality[i]<<endl;
	}
}

void CapturePicture::InitSnapConfigExUI(int nChn)
{
	if (m_stuSnapAttrEx.m_bQueried[nChn]==FALSE)
	{
		char * szSnapAttr = new char[1024*100];
		memset(szSnapAttr, 0, 1024*100);
		DWORD dwRetLen = 0;
		BOOL bRet=CLIENT_GetNewDevConfig(m_LoginID,CFG_CMD_SNAPCAPINFO,
			                             nChn,(char*)szSnapAttr,1024*100,
										 NULL,1000);
		if (bRet==FALSE)
		{
			cout<<"Query device ability failed."<<endl;
			delete []szSnapAttr;
			return;
		}
		bRet=CLIENT_ParseData(CFG_CMD_SNAPCAPINFO,szSnapAttr,
			                  (LPVOID)&m_stuSnapAttrEx.m_stuSnapAttrEx[nChn],
							  sizeof(CFG_SNAPCAPINFO_INFO),
							  NULL);
		if(bRet == FALSE)
		{
			//AfxMessageBox(ConvertString("Parse device ability failed.")); 
			cout<<"Parse device ability failed."<<endl;
			delete []szSnapAttr;
			return;	
		}
		m_stuSnapAttrEx.m_bQueried[nChn] = TRUE;
		delete []szSnapAttr;
	}

	if(m_stuSnapAttrEx.m_bQueried[nChn] == FALSE)
	{
		return;
	}

	const CFG_SNAPCAPINFO_INFO & stuSnapInfo = m_stuSnapAttrEx.m_stuSnapAttrEx[nChn];
	int i = 0;

	m_ccbSnapMode=0;
	if((stuSnapInfo.dwMode&0x00000001) > 0)
	{	
		m_ccbSnapMode=0;
		cout<<"Timing snap"<<endl;
	}
	if((stuSnapInfo.dwMode&0x00000002) > 0)
	{
		m_ccbSnapMode=1;
		cout<<"Trigger snap"<<endl;
	}

	char csFrame[50]={0};
	
	for(i = 0; i < stuSnapInfo.dwFramesPerSecNum; i++)
	{
		if(stuSnapInfo.nFramesPerSecList[i] > 0)
		{
			//csFrame.Format(ConvertString("%d frame pre second"), stuSnapInfo.nFramesPerSecList[i]);
			sprintf(csFrame,"%d frame pre second",stuSnapInfo.nFramesPerSecList[i]);
		}
		else if(stuSnapInfo.nFramesPerSecList[i] < 0)
		{
			//csFrame.Format(ConvertString("%d second pre frame"), abs(stuSnapInfo.nFramesPerSecList[i]));
			sprintf(csFrame,"%d frame pre second",abs(stuSnapInfo.nFramesPerSecList[i]));
		}

		cout<<csFrame<<endl;
	}


	char szSize[32][8] = {	"D1",	"HD1",	"BCIF",	"CIF",	"QCIF", "VGA", 
		"QVGA", "SVCD",	"QQVGA","SVGA",	"XVGA", "WXGA", 
		"SXGA", "WSXGA","UXGA", "WUXGA","LTF",	"720",
		"1080",	"1_3M"};
	int nSizeIndex = 0;

	for(i = 0; i < stuSnapInfo.dwIMageSizeNum; i++)
	{
		nSizeIndex = stuSnapInfo.emIMageSizeList[i];
		//m_ccbSize.SetItemData(m_ccbSize.AddString(szSize[nSizeIndex]), nSizeIndex);
		cout<<szSize[nSizeIndex]<<endl;
	}



	int nMapQuality[7] = {0, 10, 30, 50, 60, 80, 100};
	char csQuality[50]={0};
	
	for(i = 0; i < stuSnapInfo.dwQualityMun; i++)
	{
		if(stuSnapInfo.emQualityList[i] > 0 && stuSnapInfo.emQualityList[i] < 7)
		//	csQuality.Format("%d%s", nMapQuality[stuSnapInfo.emQualityList[i]], "%");
		sprintf(csQuality,"%d%s",nMapQuality[stuSnapInfo.emIMageSizeList[i]],"%");

		//m_ccbQuality.SetItemData(m_ccbQuality.AddString(csQuality), stuSnapInfo.emQualityList[i]);
		cout<<csQuality<<endl;
	}
}


bool CapturePicture::GetSnapConfigEx(int nChn)
{
	char * szSnapAttr = new char[1024*100];
	memset(szSnapAttr, 0, 1024*100);
	DWORD dwRetLen = 0;
	BOOL bRet=CLIENT_GetNewDevConfig(m_LoginID,CFG_CMD_ENCODE,nChn,
		                             (char*)szSnapAttr,1024*100,NULL,1000);
	if (bRet==FALSE)
	{
		cout<<"Query device ability failed."<<endl;
		delete []szSnapAttr;
		return false;
	}

	memset(&m_stuSnapCfgEx[nChn],0,sizeof(CFG_ENCODE_INFO));
	bRet=CLIENT_ParseData(CFG_CMD_ENCODE,szSnapAttr,
		                   (LPVOID)&m_stuSnapCfgEx[nChn],
						   sizeof(CFG_ENCODE_INFO),
						   NULL);
	if (bRet==FALSE)
	{
		cout<<"Get snap configure failed."<<endl;
		delete []szSnapAttr;
		return true;
	}

	delete []szSnapAttr;
	return true;
}

void CapturePicture::OnSelchangeComboChannel()
{
	int nIndex = m_ctlChannel;
	if (nIndex==CB_ERR)
	{
		return;
	}
	int nChn = m_ctlChannel;
	
	int nSnapMode = 0;
	nIndex = m_ccbSnapMode;
	if(nIndex != CB_ERR)
	{
		nSnapMode = m_ccbSnapMode;
	}

	if (m_bJSON==FALSE)
	{
		InitSnapConfigUI(nChn);
		
	} 
	else
	{
	}

}


void  CapturePicture::OnRadioSnaptime()
{
	m_snapmode=1;
}

void CapturePicture::OnBtnStartsnap()
{
	if (0!=m_LoginID)
	{
		if (m_snapmode==-1)
		{
			cout<<"please select snap mode!"<<endl;
			return;
		}

		SNAP_PARAMS snapparams = {0};
		snapparams.Channel=m_ctlChannel;
		snapparams.mode=m_snapmode;
		snapparams.CmdSerial=m_serial;

		if (snapparams.mode==1)
		{
			//Time interval for scheduled snapshot. Use snapshot setup to configure.
			//snapparams.InterSnap = GetDlgItemInt(IDC_EDIT_SNAPINT);
		}

		BOOL b=CLIENT_SnapPicture(m_LoginID,snapparams);
		if (!b)
		{
			cout<<"begin to snap failed!"<<endl;
		}

	}

}

void CALLBACK DisConnectFunc(LONG lLoginID, char *pchDVRIP, LONG nDVRPort, DWORD dwUser)
{
	if(dwUser == 0)
	{
		return;
	}

	cout<<"Netword disconnected!"<<endl;
}

void CALLBACK SnapPicRet(LONG ILoginID, BYTE *pBuf, UINT RevLen, UINT EncodeType, DWORD CmdSerial, DWORD dwUser)
{
	CapturePicture *pThis = (CapturePicture*)dwUser;
	pThis->OnOnePicture(ILoginID,pBuf,RevLen,EncodeType,CmdSerial);
}

void CapturePicture::InitNetSDK()
{
	BOOL ret = CLIENT_Init(DisConnectFunc, (DWORD)this);
	if (ret)
	{
		CLIENT_SetSnapRevCallBack(SnapPicRet,(DWORD)this);
	}
	else
	{
		cout<<"initialize SDK failed!"<<endl;
	}
}

void CapturePicture::OnOnePicture(LONG ILoginID, BYTE *pBuf, UINT RevLen, UINT EncodeType, UINT CmdSerial)
{
	
   /* Save image original file*/
	FILE *stream;
	if( (stream = fopen((const char*) pFileName, "wb")) != NULL )
	{
		int numwritten = fwrite( pBuf, sizeof( char ), RevLen, stream );
		fclose( stream );
	}
	/*Veirfy image encode type. If it is an I frame of mpeg4,then call I frame to decode to BMP to display.*/
<<<<<<< HEAD
}

void CapturePicture::Logout() 
{
	// TODO: Add your control notification handler code here
	if(0 != m_LoginID)
	{
		BOOL bSuccess = CLIENT_Logout(m_LoginID);
		if(bSuccess)
		{
			m_LoginID = 0;
			m_bJSON = FALSE;
		}
		else
		{
			cout<<"Logout failed!";
		}
	}
}
=======
}
>>>>>>> ee624059fa4d4e51d6412d2f829bd2779430e51a
