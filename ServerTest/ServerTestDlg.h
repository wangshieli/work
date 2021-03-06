
// ServerTestDlg.h: 头文件
//

#pragma once


// CServerTestDlg 对话框
class CServerTestDlg : public CDialogEx
{
// 构造
public:
	CServerTestDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVERTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	SOCKET m_sock;
	char recvbuf[1024 * 4 * 8];
	int recvlen;
	void DealData(msgpack::sbuffer& sBuf);
	int PaserserDate();
	static int nRegister;
	static int nKh;
public:
	afx_msg void OnBnClickedRegister();
	afx_msg void OnBnClickedAddkh();
	afx_msg void OnBnClickedKhlist();
	afx_msg void OnBnClickedUserlist();
	afx_msg void OnBnClickedNewcard();
	afx_msg void OnBnClickedChucard();
	afx_msg void OnBnClickedXf();
	afx_msg void OnBnClickedSimlist();
	afx_msg void OnBnClickedOn1m();
	afx_msg void OnBnClickedOn15d();
	afx_msg void OnBnClickedKhsimlist();
	afx_msg void OnBnClickedDu1m();
	afx_msg void OnBnClickedLogin();
	afx_msg void OnBnClickedTingji();
};
