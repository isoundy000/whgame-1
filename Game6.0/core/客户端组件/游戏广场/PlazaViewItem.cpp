#include "Stdafx.h"
#include "GamePlaza.h"
#include "GlobalUnits.h"
#include "RoomViewItem.h"
#include "PlazaViewItem.h"
#include "DlgControlBar.h"

#define		LIST_TOP			23				//列表头高
#define		LIST_LEFT			4				//列表左宽
#define		LIST_RIGHT			5				//列表右宽
#define		LIST_BOTTOM			7				//列表底高

#define		WEB_TOP				28				//网页头高
#define		WEB_LEFT			5				//网页左宽
#define		WEB_RIGHT			1				//网页右宽
#define		WEB_BOTTOM			7				//网页底高

#define		IDC_WEB_BBS			100
#define		IDC_WEB_BEFORE		101
#define		IDC_WEB_BRUSH		102
#define		IDC_WEB_HOME		103
#define		IDC_WEB_NEXT		104
#define		IDC_WEB_STOP		105

BEGIN_MESSAGE_MAP(CPlazaViewItem, CDialog)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_WEB_BBS, OnBnClickedBBS)
	ON_BN_CLICKED(IDC_WEB_HOME, OnBnClickedHOME)
	ON_BN_CLICKED(IDC_WEB_NEXT, OnBnClickedNEXT)
	ON_BN_CLICKED(IDC_WEB_STOP, OnBnClickedSTOP)
	ON_BN_CLICKED(IDC_WEB_BRUSH, OnBnClickedMBRUSH)
	ON_BN_CLICKED(IDC_WEB_BEFORE, OnBnClickedBEFORE)
	ON_COMMAND(IDM_LOGON_PLAZA,		OnCommandLogon)
	ON_COMMAND(IDM_CONNECT_SERVER,	OnCommandConnect)
	ON_COMMAND(IDM_CANCEL_CONNECT,	OnCommandCancelConnect)
END_MESSAGE_MAP()


CPlazaViewItem::CPlazaViewItem() : CDialog(IDD_GAME_PLAZA), m_Splitter(VorSpliter)
{
	//设置变量
	m_bRectify		= false;
	m_bInitDialog	= false;
	m_bLogonPlaza	= false;
	m_pHtmlBrower	= NULL;

	//服务器列表
	HINSTANCE hInstance=AfxGetInstanceHandle();
	m_ImageServerBL.SetLoadInfo(IDB_SERVER_BL,hInstance);
	m_ImageServerBM.SetLoadInfo(IDB_SERVER_BM,hInstance);
	m_ImageServerBR.SetLoadInfo(IDB_SERVER_BR,hInstance);
	m_ImageServerLM.SetLoadInfo(IDB_SERVER_LM,hInstance);
	m_ImageServerRM.SetLoadInfo(IDB_SERVER_RM,hInstance);
	m_ImageServerTL.SetLoadInfo(IDB_SERVER_TL,hInstance);
	m_ImageServerTM.SetLoadInfo(IDB_SERVER_TM,hInstance);
	m_ImageServerTR.SetLoadInfo(IDB_SERVER_TR,hInstance);

	//网页浏览
	m_ImageWebBL.SetLoadInfo(IDB_WEB_BL,hInstance);
	m_ImageWebBM.SetLoadInfo(IDB_WEB_BM,hInstance);
	m_ImageWebBR.SetLoadInfo(IDB_WEB_BR,hInstance);
	m_ImageWebLM.SetLoadInfo(IDB_WEB_LM,hInstance);
	m_ImageWebRM.SetLoadInfo(IDB_WEB_RM,hInstance);
	m_ImageWebTL.SetLoadInfo(IDB_WEB_TL,hInstance);
	m_ImageWebTM.SetLoadInfo(IDB_WEB_TM,hInstance);
	m_ImageWebTR.SetLoadInfo(IDB_WEB_TR,hInstance);

	return ;
}

CPlazaViewItem::~CPlazaViewItem()
{
}

//控件绑定
void CPlazaViewItem::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VOR_SPLITER, m_Splitter);
	DDX_Control(pDX, IDC_SERVER_LIST, m_ServerItemView);
}

BOOL CPlazaViewItem::PreTranslateMessage(MSG* pMsg)
{
	//提示消息
	if (m_ToolTipCtrl.m_hWnd!=NULL) 
	{
		m_ToolTipCtrl.RelayEvent(pMsg);
	}

	return __super::PreTranslateMessage(pMsg);
}

//初始化函数
BOOL CPlazaViewItem::OnInitDialog()
{
	__super::OnInitDialog();

	//设置接口
	m_Splitter.SetSplitterSink(this);
	g_pControlBar->SetPlazaViewItem(this);

	//创建控件
	m_pHtmlBrower=new CHtmlBrower;
	m_pHtmlBrower->Create(NULL,NULL,WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,20,NULL);

	//浏览野蛮
	m_pHtmlBrower->Navigate(szStationPage);

	//用户视图
	m_UserInfoView.Create(IDD_USER_INFO,this);
	m_UserInfoView.ShowWindow(SW_SHOW);

	//设置控件
	m_ServerItemView.InitServerItemView(this);
	g_GlobalUnits.m_ServerListManager.InitServerListManager(&m_ServerItemView);

	//创建按钮
	m_WEB_BEFORE.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_WEB_BEFORE);
	m_WEB_BRUSH.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_WEB_BRUSH);
	m_WEB_HOME.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_WEB_HOME);
	m_WEB_NEXT.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_WEB_NEXT);
	m_WEB_STOP.Create(NULL,WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),this,IDC_WEB_STOP);

	//设置按钮
	m_WEB_BEFORE.SetButtonImage(IDB_BT_WEB_BEFORE,AfxGetInstanceHandle(),false);
	m_WEB_BRUSH.SetButtonImage(IDB_BT_WEB_BRUSH,AfxGetInstanceHandle(),false);
	m_WEB_HOME.SetButtonImage(IDB_BT_WEB_HOME,AfxGetInstanceHandle(),false);
	m_WEB_NEXT.SetButtonImage(IDB_BT_WEB_NEXT,AfxGetInstanceHandle(),false);
	m_WEB_STOP.SetButtonImage(IDB_BT_WEB_STOP,AfxGetInstanceHandle(),false);

	//提示控件
	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);
	m_ToolTipCtrl.AddTool(&m_WEB_BEFORE,TEXT("后退"));
	m_ToolTipCtrl.AddTool(&m_WEB_NEXT,TEXT("前进"));
	m_ToolTipCtrl.AddTool(&m_WEB_STOP,TEXT("停止"));
	m_ToolTipCtrl.AddTool(&m_WEB_HOME,TEXT("主页"));
	m_ToolTipCtrl.AddTool(&m_WEB_BRUSH,TEXT("刷新"));

	//登录广场
	SendLogonMessage();

	//设置变量
	m_bInitDialog=true;

	return TRUE;
}

//接口查询
void * __cdecl CPlazaViewItem::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IViewItem,Guid,dwQueryVer);
	QUERYINTERFACE(IPlazaViewItem,Guid,dwQueryVer);
	QUERYINTERFACE(IClientSocketSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IViewItem,Guid,dwQueryVer);
	return NULL;
}

//按钮消息
bool CPlazaViewItem::OnSplitterButton(CSkinSplitter * pSkinSplitter, CSkinButton * pSplitterButton)
{
	return true;
}

//拆分条消息
bool CPlazaViewItem::OnSplitterEvent(CSkinSplitter * pSkinSplitter, UINT uSplitID, int nXPos, int nYPos)
{
	CRect rcClient;
	GetClientRect(&rcClient);
	RectifyControl(rcClient.Width(),rcClient.Height());

	return true;
}

//左键单击
void __cdecl CPlazaViewItem::OnTreeLeftClick(CListItem *pListItem, HTREEITEM hTreeItem, CTreeCtrl * pTreeCtrl)
{
	//效验参数
	if (pListItem==NULL) 
	{
		return;
	}

	//消息处理
	switch (pListItem->GetItemGenre())
	{
	case ItemGenre_Inside:
		{
			//变量定义
			CListInside * pListInside=(CListInside *)pListItem;
			tagGameInside * pGameInside=pListInside->GetItemInfo();

			WebBrowse(TEXT("http://www.dreamlib.com/game/"),false);

			return;
		}
	case ItemGenre_Kind:		//游戏类型
		{
			//变量定义
			CListKind * pListKind=(CListKind *)pListItem;
			tagGameKind * pGameKind=pListKind->GetItemInfo();

			//连接规则
			TCHAR szRuleUrl[256]=TEXT("");
			_snprintf(szRuleUrl,sizeof(szRuleUrl),TEXT("http://www.dreamlib.com/game/rules/rule%ld.html"),pGameKind->wKindID);
			WebBrowse(szRuleUrl,true);

			return;
		}
	}

	return;
}

//右键单击
void __cdecl CPlazaViewItem::OnTreeRightClick(CListItem *pListItem, 
											  HTREEITEM hTreeItem, 
											  CTreeCtrl *pTreeCtrl)
{
	return;
}

//左键双击
void __cdecl CPlazaViewItem::OnTreeLeftDBClick(CListItem *pListItem, 
											   HTREEITEM hTreeItem, 
											   CTreeCtrl *pTreeCtrl)
{
	//效验参数
	if(pListItem==NULL) 
	{
		return ;
	}

	//消息处理
	switch (pListItem->GetItemGenre())
	{
	//游戏类型
	case ItemGenre_Kind:	
		{
			CListKind *pListKind	= (CListKind *)pListItem;
			tagGameKind *pGameKind	= pListKind->GetItemInfo();

			//安装判断
			if (pListKind->m_bInstall==false)
			{
				TCHAR szBuffer[512]=TEXT("");
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("【%s】还没有安装，现在是否下载？"),pGameKind->szKindName);
				int nResult = ShowMessageBox(szBuffer,
										MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1);
				if(nResult == IDYES)	
				{
					g_GlobalAttemper.DownLoadClient(pGameKind->szKindName,
													pGameKind->wKindID,
													true);
				}
			}

			return;
		}

	//进程类型
	case ItemGenre_Process:	
		{
			CListProcess *pListProcess	= (CListProcess *)pListItem;
			tagGameProcess *pGameProcess= pListProcess->GetItemInfo();

			//构造命令行
			CString strCommonLine;
			tagGlobalUserData & UserData=g_GlobalUnits.GetGolbalUserData();
			strCommonLine.Format(TEXT("%s /UserToken:%ld#%s#%s /KindID:%ld /WndPlaza:%ld"),
							pGameProcess->szProcessName,
							UserData.dwUserID,
							UserData.szAccounts,
							UserData.szPassWord,
							pGameProcess->wKindID,
							m_hWnd);

			//启动游戏客户端
			STARTUPINFO			StartInfo;
			PROCESS_INFORMATION ProcessInfo;
			
			memset(&StartInfo,0,sizeof(StartInfo));
			memset(&ProcessInfo,0,sizeof(ProcessInfo));
			
			StartInfo.cb=sizeof(StartInfo);
			StartInfo.wShowWindow=SW_SHOWMAXIMIZED;
			BOOL bSuccess=CreateProcess(NULL,strCommonLine.GetBuffer(),NULL,NULL,FALSE,CREATE_DEFAULT_ERROR_MODE,NULL,NULL,&StartInfo,&ProcessInfo);
			strCommonLine.ReleaseBuffer();
			if(bSuccess == FALSE)
			{
				//下载询问
				TCHAR szBuffer[512]=TEXT("");
				_snprintf(szBuffer,sizeof(szBuffer),TEXT("【%s】还没有安装，现在是否下载？"),pGameProcess->szKindName);
				int nResult=ShowMessageBox(szBuffer,MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1);
				if (nResult==IDYES)	g_GlobalAttemper.DownLoadClient(pGameProcess->szKindName,pGameProcess->wKindID,true);
			}
			else
			{
				CloseHandle(ProcessInfo.hThread);
				CloseHandle(ProcessInfo.hProcess);
			}

			return;
		}

	//游戏房间
	case ItemGenre_Server:	
		{
			CListServer * pListServer=(CListServer *)pListItem;
			g_pControlBar->CreateRoomViewItem(pListServer);
			return;
		}
	}

	return;
}

//右键双击
void __cdecl CPlazaViewItem::OnTreeRightDBClick(CListItem *pListItem, 
												HTREEITEM hTreeItem, 
												CTreeCtrl *pTreeCtrl)
{
	return;
}

//选择改变
void __cdecl CPlazaViewItem::OnTreeSelectChanged(CListItem *pListItem, 
												 HTREEITEM hTreeItem, 
												 CTreeCtrl *pTreeCtrl)
{
	return;
}

//子项展开
void __cdecl CPlazaViewItem::OnTreeItemexpanded(CListItem *pListItem, 
												HTREEITEM hTreeItem, 
												CTreeCtrl * pTreeCtrl)
{
	return;
}

//网络连接消息
bool __cdecl CPlazaViewItem::OnSocketConnect(int iErrorCode, 
											LPCTSTR pszErrorDesc, 
											IClientSocket *pIClientSocke)
{
	//错误处理
	if (iErrorCode!=0)
	{
		g_GlobalAttemper.DestroyStatusWnd(this);
		ShowMessageBox(pszErrorDesc,MB_ICONINFORMATION);
		SendLogonMessage();
		return true;
	}

	//发送登录包
	if (m_bLogonPlaza==false)
	{
		m_DlgLogon.SendLogonPacket(pIClientSocke);
		g_GlobalAttemper.ShowStatusMessage(TEXT("正在验证用户登录信息..."),this);
	}

	return true;
}

//网络读取消息
bool __cdecl CPlazaViewItem::OnSocketRead(CMD_Command Command, 
										  void *pBuffer, 
										  WORD wDataSize, 
										  IClientSocket *pIClientSocke)
{
	switch (Command.wMainCmdID)
	{
	//登录消息
	case MDM_GP_LOGON:				
		{
			return OnSocketMainLogon(Command,
								pBuffer,
								wDataSize,
								pIClientSocke);
		}

	//列表消息
	case MDM_GP_SERVER_LIST:		
		{
			return OnSocketMainServerList(Command,
								pBuffer,
								wDataSize,
								pIClientSocke);
		}

	//系统消息
	case MDM_GP_SYSTEM:				
		{
			return OnSocketMainSystem(Command,
								pBuffer,
								wDataSize,
								pIClientSocke);
		}
	}
	return true;
}

//网络关闭消息
bool __cdecl CPlazaViewItem::OnSocketClose(IClientSocket *pIClientSocke, 
										   bool bCloseByServer)
{
	if (m_bLogonPlaza==false)
	{
		g_GlobalAttemper.DestroyStatusWnd(this);
		if(pIClientSocke->GetRecvPacketCount()==0)
		{
			ShowMessageBox(TEXT("服务器繁忙，请选择其他服务器登录！"),MB_ICONINFORMATION);
		}
		SendLogonMessage();
	}
	return true;
}

//登录消息
bool CPlazaViewItem::OnSocketMainLogon(CMD_Command Command, void *pBuffer, WORD wDataSize, IClientSocket *pIClientSocke)
{
	ASSERT(Command.wMainCmdID==MDM_GP_LOGON);
	switch (Command.wSubCmdID)
	{
	case SUB_GP_LOGON_SUCCESS:		//登录成功
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(CMD_GP_LogonSuccess));
			if (wDataSize<sizeof(CMD_GP_LogonSuccess)) 
			{
				return false;
			}

			//保存信息
			tagGlobalUserData & UserData=g_GlobalUnits.GetGolbalUserData();
			CMD_GP_LogonSuccess * pLogonSuccess=(CMD_GP_LogonSuccess *)pBuffer;
			UserData.wFaceID=pLogonSuccess->wFaceID;
			UserData.cbGender=pLogonSuccess->cbGender;
			UserData.cbMember=pLogonSuccess->cbMember;
			UserData.dwUserID=pLogonSuccess->dwUserID;
			UserData.dwGameID=pLogonSuccess->dwGameID;
			UserData.dwExperience=pLogonSuccess->dwExperience;

			//扩展信息
			void * pDataBuffer=NULL;
			tagDataDescribe DataDescribe;
			CRecvPacketHelper RecvPacket(pLogonSuccess+1,wDataSize-sizeof(CMD_GP_LogonSuccess));
			while (true)
			{
				pDataBuffer=RecvPacket.GetData(DataDescribe);
				if (DataDescribe.wDataDescribe==DTP_NULL) break;
				switch (DataDescribe.wDataDescribe)
				{
				case DTP_USER_ACCOUNTS:		//用户帐户
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize>0);
						ASSERT(DataDescribe.wDataSize<=sizeof(UserData.szAccounts));
						if (DataDescribe.wDataSize<=sizeof(UserData.szAccounts))
						{
							CopyMemory(UserData.szAccounts,pDataBuffer,DataDescribe.wDataSize);
							UserData.szAccounts[CountArray(UserData.szAccounts)-1]=0;
						}
						break;
					}
				case DTP_USER_PASS:			//用户密码
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize>0);
						ASSERT(DataDescribe.wDataSize<=sizeof(UserData.szPassWord));
						if (DataDescribe.wDataSize<=sizeof(UserData.szPassWord))
						{
							CopyMemory(UserData.szPassWord,pDataBuffer,DataDescribe.wDataSize);
							UserData.szPassWord[CountArray(UserData.szPassWord)-1]=0;
						}
						break;
					}
				case DTP_UNDER_WRITE:		//个性签名
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize>0);
						ASSERT(DataDescribe.wDataSize<=sizeof(UserData.szUnderWrite));
						if (DataDescribe.wDataSize<=sizeof(UserData.szUnderWrite))
						{
							CopyMemory(UserData.szUnderWrite,pDataBuffer,DataDescribe.wDataSize);
							UserData.szUnderWrite[CountArray(UserData.szUnderWrite)-1]=0;
						}
						break;
					}
				case DTP_USER_GROUP_NAME:	//社团名字
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize>0);
						ASSERT(DataDescribe.wDataSize<=sizeof(UserData.szGroupName));
						if (DataDescribe.wDataSize<=sizeof(UserData.szGroupName))
						{
							CopyMemory(UserData.szGroupName,pDataBuffer,DataDescribe.wDataSize);
							UserData.szGroupName[CountArray(UserData.szGroupName)-1]=0;
						}
						break;
					}
				case DTP_STATION_PAGE:		//游戏主站
					{
						ASSERT(pDataBuffer!=NULL);
						if (pDataBuffer!=NULL) 
						{
							g_GlobalUnits.SetStationPage((LPCTSTR)pDataBuffer);
							m_pHtmlBrower->Navigate(g_GlobalUnits.GetStationPage());
						}
						break;
					}
				default: { ASSERT(FALSE); }
				}
			}

			//设置提示
			g_GlobalAttemper.ShowStatusMessage(TEXT("正在读取服务器列表信息..."),this);


			return true;
		}
	case SUB_GP_LOGON_ERROR:		//登录失败
		{
			//效验参数
			CMD_GP_LogonError *pLogonError = (CMD_GP_LogonError *)pBuffer;
			ASSERT(wDataSize>=(sizeof(CMD_GP_LogonError)-sizeof(pLogonError->szErrorDescribe)));
			if (wDataSize<(sizeof(CMD_GP_LogonError)-sizeof(pLogonError->szErrorDescribe))) return false;

			//关闭连接
			g_GlobalAttemper.DestroyStatusWnd(this);
			pIClientSocke->CloseSocket(false);

			//显示消息
			WORD wDescribeSize=wDataSize-(sizeof(CMD_GP_LogonError)-sizeof(pLogonError->szErrorDescribe));
			if (wDescribeSize>0)
			{
				pLogonError->szErrorDescribe[wDescribeSize-1]=0;
				ShowMessageBox(pLogonError->szErrorDescribe,MB_ICONINFORMATION);
			}

			//发送登录
			SendLogonMessage();

			return true;
		}
	case SUB_GP_LOGON_FINISH:		//登录完成
		{
			//关闭提示
			g_GlobalAttemper.DestroyStatusWnd(this);

			//展开类型
			INT_PTR nIndex=0;
			CListType * pListType=NULL;
			do
			{
				pListType=g_GlobalUnits.m_ServerListManager.EnumTypeItem(nIndex++);
				if (pListType==NULL) break;
				g_GlobalUnits.m_ServerListManager.ExpandListItem(pListType);
			} while (true);

			//展开列表
			nIndex=0;
			CListInside * pListInside=NULL;
			do
			{
				pListInside=g_GlobalUnits.m_ServerListManager.EnumInsideItem(nIndex++);
				if (pListInside==NULL) break;
				g_GlobalUnits.m_ServerListManager.ExpandListItem(pListInside);
			} while (true);

			//记录信息
			m_bLogonPlaza=true;
			m_DlgLogon.OnLogonSuccess();
			m_pHtmlBrower->EnableBrowser(true);

			//显示头像
			m_UserInfoView.ShowUserInfo(true);

			//关闭连接
			pIClientSocke->CloseSocket(false);

			//显示主窗口
			CWnd *pParent = this->GetParentFrame();
			if(pParent != NULL)pParent->ShowWindow(SW_SHOW);

			return true;
		}
	}

	return true;
}

//游戏列表消息
bool CPlazaViewItem::OnSocketMainServerList(CMD_Command Command, void * pBuffer, WORD wDataSize, IClientSocket * pIClientSocke)
{
	ASSERT(Command.wMainCmdID==MDM_GP_SERVER_LIST);
	
	switch (Command.wSubCmdID)
	{
	case SUB_GP_LIST_TYPE:			//类型信息
		{
			//效验参数
			ASSERT(wDataSize%sizeof(tagGameType)==0);
			if (wDataSize%sizeof(tagGameType)!=0) return false;

			//处理消息
			tagGameType * pGameType=(tagGameType *)pBuffer;
			WORD wItemCount=wDataSize/sizeof(tagGameType);
			g_GlobalUnits.m_ServerListManager.InsertTypeItem(pGameType,wItemCount);

			return true;
		}
	case SUB_GP_LIST_KIND:			//种类消息
		{
			//效验参数
			ASSERT(wDataSize%sizeof(tagGameKind)==0);
			if (wDataSize%sizeof(tagGameKind)!=0) return false;

			//处理消息
			tagGameKind * pGameKind=(tagGameKind *)pBuffer;
			WORD wItemCount=wDataSize/sizeof(tagGameKind);
			g_GlobalUnits.m_ServerListManager.InsertKindItem(pGameKind,wItemCount);

			return true;
		}
	case SUB_GP_LIST_PROCESS:		//进程信息
		{
			//效验参数
			ASSERT(wDataSize%sizeof(tagGameProcess)==0);
			if (wDataSize%sizeof(tagGameProcess)!=0) return false;

			//处理消息
			tagGameProcess * pGameProcess=(tagGameProcess *)pBuffer;
			WORD wItemCount=wDataSize/sizeof(tagGameProcess);
			g_GlobalUnits.m_ServerListManager.InsertProcessItem(pGameProcess,wItemCount);

			return true;
		}
	case SUB_GP_LIST_STATION:		//站点消息
		{
			//效验参数
			ASSERT(wDataSize%sizeof(tagGameStation)==0);
			if (wDataSize%sizeof(tagGameStation)!=0) return false;

			//处理消息
			tagGameStation * pGameStation=(tagGameStation *)pBuffer;
			WORD wItemCount=wDataSize/sizeof(tagGameStation);
			g_GlobalUnits.m_ServerListManager.InsertStationItem(pGameStation,wItemCount);

			return true;
		}
	case SUB_GP_LIST_SERVER:		//服务器房间
		{
			//效验参数
			ASSERT(wDataSize%sizeof(tagGameServer)==0);
			if (wDataSize%sizeof(tagGameServer)!=0) return false;

			//处理消息
			tagGameServer * pGameServer=(tagGameServer *)pBuffer;
			WORD wItemCount=wDataSize/sizeof(tagGameServer);
			g_GlobalUnits.m_ServerListManager.InsertServerItem(pGameServer,wItemCount);

			return true;
		}
	case SUB_GP_LIST_FINISH:		//列表发送完成
		{
			//更新人数
			INT_PTR nIndex=0;
			DWORD dwAllOnLineCount=0L;
			CListKind * pListKind=NULL;
			do
			{
				pListKind=g_GlobalUnits.m_ServerListManager.EnumKindItem(nIndex++);
				if (pListKind==NULL) break;
				dwAllOnLineCount+=pListKind->GetItemInfo()->dwOnLineCount;
			} while (true);
			g_GlobalUnits.m_ServerListManager.UpdateGameOnLineCount(dwAllOnLineCount);

			return true;
		}
	case SUB_GP_LIST_CONFIG:		//列表配置
		{
			//效验参数
			ASSERT(wDataSize%sizeof(CMD_GP_ListConfig)==0);
			if (wDataSize%sizeof(CMD_GP_ListConfig)!=0) return false;

			//处理消息
			CMD_GP_ListConfig * pListConfig=(CMD_GP_ListConfig *)pBuffer;
			m_ServerItemView.ShowOnLineCount(pListConfig->bShowOnLineCount?true:false);

			return true;
		}
	}

	return true;
}

//系统消息
bool CPlazaViewItem::OnSocketMainSystem(CMD_Command Command, void * pBuffer, WORD wDataSize, IClientSocket * pIClientSocke)
{
	ASSERT(Command.wMainCmdID==MDM_GP_SYSTEM);
	switch (Command.wSubCmdID)
	{
	case SUB_GP_VERSION:			//版本信息
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(CMD_GP_Version));
			if (wDataSize<sizeof(CMD_GP_Version)) return false;

			//消息处理
			CMD_GP_Version * pVersion=(CMD_GP_Version *)pBuffer;
			if (pVersion->bAllowConnect)
			{
				TCHAR szMessage[]=TEXT("游戏大厅版本已经升级，现在的版本还可以继续使用，现在是否马上下载新版本？");
				int iResult=::MessageBox(m_hWnd,szMessage,TEXT("游戏广场"),MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1);
				if (iResult==IDYES)
				{
					g_GlobalAttemper.DestroyStatusWnd(this);
					m_ClientSocket->CloseSocket(false);
					tagGlobalUserData & GlobalUserData=g_GlobalUnits.GetGolbalUserData();
					memset(&GlobalUserData,0,sizeof(GlobalUserData));
					g_GlobalAttemper.DownLoadClient(TEXT("游戏广场"),0,true);
				}
			}
			else
			{
				g_GlobalAttemper.DestroyStatusWnd(this);
				m_ClientSocket->CloseSocket(false);
				TCHAR szMessage[]=TEXT("游戏大厅版本已经升级，现在的版本不可以继续使用，现在是否马上下载新版本？");
				int iResult=::MessageBox(m_hWnd,szMessage,TEXT("游戏广场"),MB_ICONSTOP|MB_YESNO|MB_DEFBUTTON1);
				if(iResult != IDYES)
				{
					tagGlobalUserData & GlobalUserData=g_GlobalUnits.GetGolbalUserData();
					memset(&GlobalUserData,0,sizeof(GlobalUserData));
					AfxGetMainWnd()->PostMessage(WM_CLOSE);
				}
				else 
				{
					g_GlobalAttemper.DownLoadClient(TEXT("游戏广场"),0,true);
				}
			}

			return true;
		}
	}
	return true;
}

//登录消息
bool __cdecl CPlazaViewItem::SendLogonMessage()
{
	m_bLogonPlaza=false;
	m_UserInfoView.ShowUserInfo(false);
	PostMessage(WM_COMMAND,IDM_LOGON_PLAZA,0);
	return true;
}

//连接消息
bool __cdecl CPlazaViewItem::SendConnectMessage()
{
	PostMessage(WM_COMMAND,IDM_CONNECT_SERVER,0);
	return true;
}

//显示网页
void CPlazaViewItem::WebBrowse(LPCTSTR lpszUrl, bool bForceBrowse)
{
	if (m_pHtmlBrower->GetSafeHwnd())
	{
		m_pHtmlBrower->Navigate(lpszUrl);
		if (bForceBrowse==true) 
		{
			m_pHtmlBrower->EnableBrowser(true);
		}
	}
	return;
}

//显示消息
int CPlazaViewItem::ShowMessageBox(LPCTSTR pszMessage, UINT nType)
{
	int nResult = MessageBox(pszMessage,TEXT("游戏大厅"),nType);
	return nResult;
}

//位置消息
void CPlazaViewItem::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	//调整控件
	RectifyControl(cx,cy);

	return;
}

//显示消息
void CPlazaViewItem::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);

	//设置标题
	if (bShow) 
	{
		UpdateTitleText();
	}

	return;
}

//绘画消息
void CPlazaViewItem::OnPaint()
{
	CPaintDC dc(this);

	//绘画界面
	DrawWebFrame(&dc);
	DrawListFrame(&dc);

	return;
}

//绘画背景
BOOL CPlazaViewItem::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//登录广场
void CPlazaViewItem::OnCommandLogon()
{
	//创建登录框
	if (m_DlgLogon.m_hWnd==NULL) 
	{
		m_DlgLogon.Create(IDD_LOGON,this);
	}

	//显示登录框
	m_DlgLogon.ShowWindow(SW_SHOW);
	m_DlgLogon.SetActiveWindow();
	m_DlgLogon.SetForegroundWindow();

	return;
}

//连接服务器
void CPlazaViewItem::OnCommandConnect()
{
	//创建组件
	if (m_ClientSocket.GetInterface()==NULL)
	{
		try
		{
			IUnknownEx * pIUnknownEx=(IUnknownEx *)QueryInterface(IID_IUnknownEx,VER_IUnknownEx);
			if (m_ClientSocket.CreateInstance()==false) 
			{
				throw TEXT("网络组件创建失败");
			}
			if (m_ClientSocket->SetSocketSink(pIUnknownEx)==false) 
			{
				throw TEXT("网络组件回调接口设置失败");
			}
		}
		catch (...)
		{
			ShowMessageBox(TEXT("网络组件创建失败，请重新下载游戏大厅！"),MB_ICONSTOP);
			PostMessage(WM_COMMAND,IDM_LOGON_PLAZA,0);
			return;
		}
	}

	//地址解释
	CRegKey RegServer;
	LPCTSTR pszServerIP=m_DlgLogon.GetLogonServer();
	TCHAR szRegServer[256]=TEXT(""),szServerAddr[64]=TEXT("");
	_snprintf(szRegServer,sizeof(szRegServer),TEXT("%s\\%s"),REG_LOGON_SERVER,pszServerIP);
	
	if (RegServer.Open(HKEY_CURRENT_USER,szRegServer,KEY_READ)==ERROR_SUCCESS)
	{
		TCHAR szReadData[1024]=TEXT("");
		DWORD dwReadData=0L,dwDataType=0L,dwDataSize=sizeof(szReadData);
		LONG lErrorCode=RegServer.QueryValue(TEXT("ServerAddr"),&dwDataType,szReadData,&dwDataSize);
		if (lErrorCode==ERROR_SUCCESS) 
		{
			CXOREncrypt::CrevasseData(szReadData,szServerAddr,sizeof(szServerAddr));
			pszServerIP=szServerAddr;
		}
	}

	//连接服务器
	try
	{
		//连接服务器
		m_ClientSocket->CloseSocket(false);
		if (m_ClientSocket->Connect(pszServerIP,PORT_LOGON_SERVER)==false)
		{
			throw TEXT("服务器连接错误，可能是你的系统还没有成功连接上网络！");
		}
	}
	catch (LPCTSTR pszError)
	{
		ShowMessageBox(pszError,MB_ICONINFORMATION);
		SendLogonMessage();
		return;
	}

	//显示提示框
	CString strMessage=TEXT("正在连接服务器，请稍候...");
	g_GlobalAttemper.ShowStatusMessage(strMessage,this);
	
	return;
}

//取消连接
void CPlazaViewItem::OnCommandCancelConnect()
{
	g_GlobalAttemper.DestroyStatusWnd(this);
	m_ClientSocket->CloseSocket(false);
	SendLogonMessage();
	return;
}

//更新标题
void CPlazaViewItem::UpdateTitleText()
{
	CString strTitle;
	strTitle.LoadString(AfxGetInstanceHandle(),IDS_MAIN_TITLE);
	AfxGetMainWnd()->SetWindowText(strTitle);

	return;
}

//绘画网页
void CPlazaViewItem::DrawWebFrame(CDC * pDC)
{
	//获取位置
	CRect rcClient,rcSplitter;
	GetClientRect(&rcClient);
	m_Splitter.GetWindowRect(&rcSplitter);
	ScreenToClient(&rcSplitter);

	//加载资源
	HDC hDC=pDC->m_hDC;
	CImageHandle ImageWebBLHandle(&m_ImageWebBL);
	CImageHandle ImageWebBMHandle(&m_ImageWebBM);
	CImageHandle ImageWebBRHandle(&m_ImageWebBR);
	CImageHandle ImageWebLMHandle(&m_ImageWebLM);
	CImageHandle ImageWebRMHandle(&m_ImageWebRM);
	CImageHandle ImageWebTLHandle(&m_ImageWebTL);
	CImageHandle ImageWebTMHandle(&m_ImageWebTM);
	CImageHandle ImageWebTRHandle(&m_ImageWebTR);

	//绘画上框
	m_ImageWebTL.BitBlt(hDC,rcSplitter.right,0);
	for (int nXPos=rcSplitter.right+m_ImageWebTL.GetWidth();nXPos<rcClient.Width();nXPos+=m_ImageWebTM.GetWidth())
	{
		m_ImageWebTM.BitBlt(hDC,nXPos,0);
	}
	m_ImageWebTR.BitBlt(hDC,rcClient.Width()-m_ImageWebTR.GetWidth(),0);

	//绘画左右
	for (int nYPos=m_ImageWebTL.GetHeight();nYPos<rcClient.Height();nYPos+=m_ImageWebLM.GetHeight())
	{
		m_ImageWebLM.BitBlt(hDC,rcSplitter.right,nYPos);
		m_ImageWebRM.BitBlt(hDC,rcClient.Width()-m_ImageWebRM.GetWidth(),nYPos);
	}

	//绘画下框
	m_ImageWebBL.BitBlt(hDC,rcSplitter.right,rcClient.Height()-m_ImageWebBL.GetHeight());
	for (int nXPos=rcSplitter.right+m_ImageWebBL.GetWidth();nXPos<rcClient.Width();nXPos+=m_ImageWebBM.GetWidth())
	{
		m_ImageWebBM.BitBlt(hDC,nXPos,rcClient.Height()-m_ImageWebBM.GetHeight());
	}
	m_ImageWebBR.BitBlt(hDC,rcClient.Width()-m_ImageWebBR.GetWidth(),rcClient.Height()-m_ImageWebBR.GetHeight());

	return;
}

//绘画列表
void CPlazaViewItem::DrawListFrame(CDC * pDC)
{
	//获取位置
	CRect rcClient,rcSplitter;
	GetClientRect(&rcClient);
	m_Splitter.GetWindowRect(&rcSplitter);
	ScreenToClient(&rcSplitter);

	//加载资源
	HDC hDC=pDC->m_hDC;
	CImageHandle ImageServerBLHandle(&m_ImageServerBL);
	CImageHandle ImageServerBMHandle(&m_ImageServerBM);
	CImageHandle ImageServerBRHandle(&m_ImageServerBR);
	CImageHandle ImageServerLMHandle(&m_ImageServerLM);
	CImageHandle ImageServerRMHandle(&m_ImageServerRM);
	CImageHandle ImageServerTLHandle(&m_ImageServerTL);
	CImageHandle ImageServerTMHandle(&m_ImageServerTM);
	CImageHandle ImageServerTRHandle(&m_ImageServerTR);

	//列表高度
	int nUserInfo=m_UserInfoView.GetTitleHeight();

	//绘画上框
	m_ImageServerTL.BitBlt(hDC,0,nUserInfo);
	m_ImageServerTM.StretchBlt(hDC,m_ImageServerTL.GetWidth(),nUserInfo,
		rcSplitter.left-m_ImageServerTL.GetWidth()-m_ImageServerTR.GetWidth(),
		m_ImageServerTM.GetHeight());
	m_ImageServerTR.BitBlt(hDC,rcSplitter.left-m_ImageServerTR.GetWidth(),nUserInfo);

	//绘画左右
	for (int nYPos=m_ImageServerTL.GetHeight()+nUserInfo;nYPos<rcClient.Height();nYPos+=m_ImageServerLM.GetHeight())
	{
		m_ImageServerLM.BitBlt(hDC,0,nYPos);
		m_ImageServerRM.BitBlt(hDC,rcSplitter.left-m_ImageServerRM.GetWidth(),nYPos);
	}

	//绘画下框
	m_ImageServerBL.BitBlt(hDC,0,rcClient.Height()-m_ImageServerBL.GetHeight());
	m_ImageServerBM.StretchBlt(hDC,m_ImageServerBL.GetWidth(),rcClient.Height()-m_ImageServerBM.GetHeight(),
		rcSplitter.left-m_ImageServerBR.GetWidth(),m_ImageServerBM.GetHeight());
	m_ImageServerBR.BitBlt(hDC,rcSplitter.left-m_ImageServerBR.GetWidth(),rcClient.Height()-m_ImageServerBR.GetHeight());

	return;
}

//调整控件
void CPlazaViewItem::RectifyControl(int nWidth, int nHeight)
{
	//状态判断
	if (m_bInitDialog==false) return;
	if ((nWidth==0)||(nHeight==0)) return;

	//变量定义
	CRect rcSplitter;
	const int nSplitterWidth=6;
	const UINT uFlags=SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOCOPYBITS;

	//获取位置
	m_Splitter.GetWindowRect(&rcSplitter);
	ScreenToClient(&rcSplitter);

	//调整拆分条
	if (m_bRectify==false)
	{
		m_bRectify=true;
		rcSplitter.left=nWidth/4;
		rcSplitter.right=rcSplitter.left+nSplitterWidth;
	}

	//设置拆分范围
	int nLessPos=nWidth/5,nMaxPos=nWidth*4/5;
	m_Splitter.SetSplitterRange(nLessPos,nMaxPos);
	if ((nWidth!=0)&&(nHeight!=0))
	{
		if (rcSplitter.left<nLessPos) 
		{
			rcSplitter.left=nLessPos;
			rcSplitter.right=rcSplitter.left+nSplitterWidth;
		}
		if (rcSplitter.right>nMaxPos)
		{
			rcSplitter.right=nMaxPos;
			rcSplitter.left=rcSplitter.right-nSplitterWidth;
		}
	}

	//移动控件
	HDWP hDwp=BeginDeferWindowPos(32);
	DeferWindowPos(hDwp,m_ServerItemView,NULL,LIST_LEFT,m_UserInfoView.GetTitleHeight()+LIST_TOP,rcSplitter.left-LIST_LEFT-LIST_RIGHT,
		nHeight-LIST_TOP-LIST_BOTTOM-m_UserInfoView.GetTitleHeight(),uFlags);
	DeferWindowPos(hDwp,m_UserInfoView,NULL,0,0,rcSplitter.left,m_UserInfoView.GetTitleHeight(),uFlags);
	DeferWindowPos(hDwp,m_Splitter,NULL,rcSplitter.left,0,rcSplitter.Width(),nHeight,uFlags);
	DeferWindowPos(hDwp,m_pHtmlBrower->GetSafeHwnd(),NULL,rcSplitter.right+WEB_LEFT,WEB_TOP,
		nWidth-rcSplitter.right-WEB_LEFT-WEB_RIGHT,nHeight-WEB_TOP-WEB_BOTTOM,uFlags);

	//导航按钮
	CRect rcButton;
	m_WEB_BEFORE.GetWindowRect(&rcButton);
	DeferWindowPos(hDwp,m_WEB_BEFORE,NULL,rcSplitter.right+10,3,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_WEB_NEXT,NULL,rcSplitter.right+10+rcButton.Width(),3,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_WEB_BRUSH,NULL,rcSplitter.right+10+rcButton.Width()*2,3,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_WEB_STOP,NULL,rcSplitter.right+10+rcButton.Width()*3,3,0,0,uFlags|SWP_NOSIZE);
	DeferWindowPos(hDwp,m_WEB_HOME,NULL,rcSplitter.right+10+rcButton.Width()*4,3,0,0,uFlags|SWP_NOSIZE);

	EndDeferWindowPos(hDwp);

	return;
}

afx_msg void CPlazaViewItem::OnBnClickedBEFORE()
{
	m_pHtmlBrower->GoBack();
}
afx_msg void CPlazaViewItem::OnBnClickedNEXT()
{
	m_pHtmlBrower->GoForward();
}
afx_msg void CPlazaViewItem::OnBnClickedMBRUSH()
{
	m_pHtmlBrower->Refresh();
}
afx_msg void CPlazaViewItem::OnBnClickedSTOP()
{
	m_pHtmlBrower->Stop();
}
afx_msg void CPlazaViewItem::OnBnClickedHOME()
{
	m_pHtmlBrower->Navigate(g_GlobalUnits.GetStationPage());
}
afx_msg void CPlazaViewItem::OnBnClickedBBS()
{
	m_pHtmlBrower->Navigate("http://www.dreamlib.com/bbs");
}

//-----------------------------------------------
//					the end
//-----------------------------------------------
