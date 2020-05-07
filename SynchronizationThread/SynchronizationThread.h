
// SynchronizationThread.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CSynchronizationThreadApp:
// Сведения о реализации этого класса: SynchronizationThread.cpp
//

class CSynchronizationThreadApp : public CWinApp
{
public:
	CSynchronizationThreadApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CSynchronizationThreadApp theApp;
