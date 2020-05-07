
// SynchronizationThreadDlg.h: файл заголовка
//

#pragma once


// Диалоговое окно CSynchronizationThreadDlg
class CSynchronizationThreadDlg : public CDialogEx
{
// Создание
public:
	CSynchronizationThreadDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYNCHRONIZATIONTHREAD_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString e_text;		//переменная, связанная с текстбоксом программы

	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnChangeEditText();
	void CSynchronizationThreadDlg::Write();
	void CSynchronizationThreadDlg::Read();

	bool StatusOfProgramm;		//"состояние процесса"

	HANDLE Events[3];		//события
	HANDLE hThread;		//HANDLE потока
	DWORD dwThreadID;		//Id потока, созданного CreateThread
	HANDLE hMapFile;		//FileMapping
	PVOID pBuf;		//для FileMapping
};
