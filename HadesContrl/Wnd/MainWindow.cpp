#include "MainWindow.h"

LPCTSTR MainWindow::GetWindowClassName() const
{
	return _T("HadesMainWindow");
}
CDuiString MainWindow::GetSkinFile()
{
	return _T("MainWindow.xml");
}
CDuiString MainWindow::GetSkinFolder()
{
	return _T("");
}
LRESULT MainWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lRes = __super::OnCreate(uMsg, wParam, lParam, bHandled);
	
	// Create Meue
	m_pMenu = new Menu();
	m_pMenu->Create(m_hWnd, _T(""), WS_POPUP, WS_EX_TOOLWINDOW);
	m_pMenu->ShowWindow(false);

	return lRes;
}
LRESULT MainWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return __super::OnClose(uMsg, wParam, lParam, bHandled);
}
void MainWindow::Notify(TNotifyUI& msg)
{
	CDuiString strClassName = msg.pSender->GetClass();
	CDuiString strControlName = msg.pSender->GetName();

	if (msg.sType == DUI_MSGTYPE_WINDOWINIT);
	else if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (strClassName == DUI_CTR_BUTTON)
		{
			if (strControlName == _T("MainCloseBtn"))
				Close();
			else if (strControlName == _T("MainMenuBtn"))
			{//菜单
				int xPos = msg.pSender->GetPos().left - 36;
				int yPos = msg.pSender->GetPos().bottom;
				POINT pt = { xPos, yPos };
				ClientToScreen(m_hWnd, &pt);
				m_pMenu->ShowWindow(true);
				::SetWindowPos(m_pMenu->GetHWND(), NULL, pt.x, pt.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
			}
			else if (strControlName == _T("MainMonCveBtn"))
			{//规则配置

			}
		}
		else if (strClassName == DUI_CTR_OPTION)
		{
			if (_tcscmp(static_cast<COptionUI*>(msg.pSender)->GetGroup(), _T("MainOpView")) == 0)
			{
				CHorizontalLayoutUI* pMainOptemp = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("MainOptemperature_VLayout")));
				CHorizontalLayoutUI* pMainOpcpu = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("MainOpCpu_VLayout")));
				CHorizontalLayoutUI* pMainOpbox = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("MainOpBox_VLayout")));

				if (strControlName == _T("MainMontemperatureOpt"))
				{
					// MainOptemperature_VLayout
					pMainOptemp->SetVisible(true);
					pMainOpcpu->SetVisible(false);
					pMainOpbox->SetVisible(false);
				}
				else if (strControlName == _T("MainMonCpuOpt"))
				{
					// MainOpCpu_VLayout
					pMainOptemp->SetVisible(false);
					pMainOpcpu->SetVisible(true);
					pMainOpbox->SetVisible(false);
				}
				else if (strControlName == _T("MainMonBoxOpt"))
				{
					// MainOpBox_VLayout
					pMainOptemp->SetVisible(false);
					pMainOpcpu->SetVisible(false);
					pMainOpbox->SetVisible(true);
				}
			}
			else if (strControlName == _T("MainMonUserBtn"))
			{//下发用户态监控指令
				HWND m_SvcHwnd = FindWindow(L"HadesSvc", L"HadesSvc");
				COPYDATASTRUCT c2_;
				c2_.dwData = 1;
				c2_.cbData = 0;
				c2_.lpData = NULL;
				//发送消息
				::SendMessage(m_SvcHwnd, WM_COPYDATA, NULL, (LPARAM)&c2_);
			}
			else if (strControlName == _T("MainMonKerBtn"))
			{//下发内核态监控指令
				HWND m_SvcHwnd = FindWindow(L"HadesSvc", L"HadesSvc");
				COPYDATASTRUCT c2_;
				c2_.dwData = 2;
				c2_.cbData = 0;
				c2_.lpData = NULL;
				::SendMessage(m_SvcHwnd, WM_COPYDATA, NULL, (LPARAM)&c2_);
			}
		}
	}
}
LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;

	

	return __super::HandleMessage(uMsg, wParam, lParam);
}