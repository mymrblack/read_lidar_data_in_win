
// MFCApplicationDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CMFCApplicationDlg �Ի���
class CMFCApplicationDlg : public CDialogEx
{
// ����
public:
	CMFCApplicationDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MFCAPPLICATION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_Path;
	afx_msg void PathBtnOnClicked();
	afx_msg void OnBnClickedOk();
	CButton m_BtnOk;
};
