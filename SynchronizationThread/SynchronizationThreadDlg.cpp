
// SynchronizationThreadDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "SynchronizationThread.h"
#include "SynchronizationThreadDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Диалоговое окно CAboutDlg используется для описания сведений о приложении

DWORD WINAPI ThreadFunc(PVOID pvParam)		//функция потока
{
	CSynchronizationThreadDlg* pointer = (CSynchronizationThreadDlg*)pvParam;
	while (true)
	{
		DWORD dw = WaitForMultipleObjects(3, pointer->Events, false, INFINITE);		//приостанавливаем поток для ожидания освобождения объекта ядра
		switch (dw - WAIT_OBJECT_0)		//вычитанием из возвращаемого значения WAIT_OBJECT_0 получаем индекс в массиве описателей poiner->Events
		{
			case 0:		//для 1 события
			{
				break;
			}
			case 1:		//для 2 события
			{
				pointer->SetWindowTextW(_T("Отправитель: ожидание получателя..."));
				pointer->StatusOfProgramm = true;
				pointer->GetDlgItem(IDC_EDIT_TEXT)->EnableWindow(1);
				return 1;
			}
			case 2:		//для 3 события
			{
				pointer->Read();
				break;
			}
		}
	}
	return 0;
}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

// Реализация
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Диалоговое окно CSynchronizationThreadDlg



CSynchronizationThreadDlg::CSynchronizationThreadDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SYNCHRONIZATIONTHREAD_DIALOG, pParent)
	, e_text(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSynchronizationThreadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TEXT, e_text);
}

BEGIN_MESSAGE_MAP(CSynchronizationThreadDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CSynchronizationThreadDlg::OnBnClickedButtonExit)
	ON_EN_CHANGE(IDC_EDIT_TEXT, &CSynchronizationThreadDlg::OnChangeEditText)
END_MESSAGE_MAP()


// Обработчики сообщений CSynchronizationThreadDlg

BOOL CSynchronizationThreadDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Добавление пункта "О программе..." в системное меню.

	// IDM_ABOUTBOX должен быть в пределах системной команды.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию

	//создаем события

	//получили текущие стили
	DWORD dw = GetWindowLong(m_hWnd, GWL_STYLE);
	//убрали стиль системного меню
	dw &= ~WS_SYSMENU;
	//поставили новый стиль
	SetWindowLong(m_hWnd, GWL_STYLE, dw);
	//сообщаем что изменили стили, и стоит перерисоваться - но без изменения размеров
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER);

	
	//Создаем события
	Events[0] = CreateEvent(NULL, FALSE, TRUE, _T("main"));		//с автосбросом, поэтому в функции потока ResetEvent не требуется
	Events[1] = CreateEvent(NULL, FALSE, FALSE, _T("window"));
	Events[2] = CreateEvent(NULL, TRUE, FALSE, _T("text"));

	if (WaitForSingleObject(Events[0], 0) == WAIT_OBJECT_0)		//запуск первого экземпляра 
	{
		ResetEvent(Events[0]);
		SetWindowText(_T("Отправитель: ожидание получателя..."));
		StatusOfProgramm = true;
	}
	//последующих экземпляров
	else
	{
		GetDlgItem(IDC_EDIT_TEXT)->EnableWindow(0);
		SetWindowText(_T("Получатель: готов к приему данных..."));
		StatusOfProgramm = false;
		hThread = CreateThread(NULL, 0, ThreadFunc, this, 0, &dwThreadID);
	}
	
	UpdateData(FALSE);
	
	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 512, TEXT("FileMapping"));		//создаем объект файлового отображения
	if (hMapFile == NULL)
	{
		MessageBox(TEXT("Объект File-Mapping не был открыт (%d). \n"), TEXT("ERROR"), GetLastError());
	}
	else
	{
		pBuf = (LPTSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);		//получаем доступ к данным
	}

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void CSynchronizationThreadDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CSynchronizationThreadDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CSynchronizationThreadDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSynchronizationThreadDlg::OnBnClickedButtonExit()
{
	// TODO: добавьте свой код обработчика уведомлений
	if (StatusOfProgramm)
	{
		SetEvent(Events[1]);	//перевод в свободное состояние
	}
	else
	{
		TerminateThread(hThread, 0);		//убиваем поток
		CloseHandle(hThread);
	}
	UnmapViewOfFile(pBuf);		//закрываем доступ к данным
	CloseHandle(hMapFile);		//закрываем объект
	CDialogEx::OnCancel();
}

void CSynchronizationThreadDlg::Write()
{
	if (pBuf != NULL)
	{
		GetDlgItem(IDC_EDIT_TEXT)->GetWindowText((LPTSTR)pBuf, 512);
	}
}

void CSynchronizationThreadDlg::Read()
{
	if (pBuf != NULL)
	{
		GetDlgItem(IDC_EDIT_TEXT)->SetWindowText((LPTSTR)pBuf);
	}
}

void CSynchronizationThreadDlg::OnChangeEditText()
{
	// TODO:  Если это элемент управления RICHEDIT, то элемент управления не будет
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// функция и вызов CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Добавьте код элемента управления
	if (StatusOfProgramm)
	{
		Write();
		PulseEvent(Events[2]);
	}
}
