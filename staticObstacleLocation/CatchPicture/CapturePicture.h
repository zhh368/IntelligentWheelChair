#pragma once
#include "dhnetsdk.h"
#include "dhconfigsdk.h"
#include "DhDecode.h"
#include <string>
typedef struct DH_SNAP_ATTR_EN_EX
{
	int						m_bQueried[64];
	CFG_SNAPCAPINFO_INFO	m_stuSnapAttrEx[64];
}DH_SNAP_ATTR_EN_EX;


class CapturePicture
{
public:
	CapturePicture(void);
	~CapturePicture(void);

	void Login(const char* IPadress);
	void ShowLoginErrorReason(int nError);
	void InitSnapConfigUI(int nChn);
	void InitSnapConfigExUI(int nChn);
	bool GetSnapConfigEx(int nChn);
	void OnSelchangeComboChannel(void); //可以不要
	void OnRadioSnaptime(void);
	void OnBtnStartsnap(void);
	void InitNetSDK();
	void OnOnePicture(LONG ILoginID, BYTE *pBuf, UINT RevLen, UINT EncodeType, UINT CmdSerial);
	int                    m_nChannelCount;
	int                    m_ctlChannel;
	int                    m_ccbSnapMode;
	int                    m_snapmode;
	int                    m_serial;
	LONG                   m_LoginID;
	BOOL	               m_bJSON;
	DH_SNAP_ATTR_EN		   m_stuSnapAttr;
	DH_SNAP_ATTR_EN_EX	   m_stuSnapAttrEx;
	DHDEV_SNAP_CFG		   m_stuSnapCfg[32];
	CFG_ENCODE_INFO		   m_stuSnapCfgEx[64];
	const char *pFileName;
};

