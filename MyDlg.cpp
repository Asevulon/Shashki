
// MyDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "Shashki.h"
#include "MyDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Диалоговое окно MyDlg



MyDlg::MyDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SHASHKI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void MyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_GAME_MODE, CBGameMode);
	DDX_Control(pDX, IDOK, OkCtr);
	DDX_Control(pDX, IDC_BESTSCORE, BSctr);
	DDX_Control(pDX, IDC_BESTSCORE2, ScoresList);
}

BEGIN_MESSAGE_MAP(MyDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &MyDlg::OnBnClickedOk)
	ON_COMMAND(ID_32771, &MyDlg::On32771)
	ON_COMMAND(ID_32773, &MyDlg::On32773)
	ON_COMMAND(ID_MENU_LOAD, &MyDlg::OnMenuLoad)
	ON_COMMAND(ID_32776, &MyDlg::OnSaveAll)
	ON_COMMAND(ID_32777, &MyDlg::OnLoadAll)
	ON_WM_TIMER()
	ON_MESSAGE(MS_FILLBOT, &MyDlg::OnMsFillBot)
	ON_COMMAND(ID_FORCED_TRAIN, &MyDlg::OnForcedTrain)
END_MESSAGE_MAP()


// Обработчики сообщений MyDlg

BOOL MyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию
	CBGameMode.InsertString(-1, L"2 игрока");
	CBGameMode.InsertString(-1, L"Игрок против нейросети");
	CBGameMode.InsertString(-1, L"Нейросеть против игрока");
	CBGameMode.SetCurSel(0);
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void MyDlg::OnPaint()
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
HCURSOR MyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT MyDlg::OnMsFillBot(WPARAM wParam, LPARAM lParam)
{
	trainer.FillOut();

	return LRESULT();
}


void MyDlg::FillInfo()
{
	int curtime = (clock() - trainer.StartTime) / 1000;
	int h = curtime / 3600;
	int m = (curtime % 3600) / 60;
	int s = curtime % 60;
	CString str, str2;
	str.Format(L"Лучший счет: %d\r\nИгр сыграно: %d\r\nПоколение: %d\r\nХоды: %d\r\n Время: %d : %d : %d",
		trainer.BestScore, trainer.GamesCount, trainer.GenerationCount, trainer.hod, h, m, s);
	BSctr.SetWindowTextW(str);

	str.Format(L"");
	for (int i = 0; i < 10; i++)
	{
		str2.Format(L"%d, ", trainer.scores[i]);
		str += str2;
	}
	str += L"\r\n";
	for (int i = 10; i < 20; i++)
	{
		str2.Format(L"%d, ", trainer.scores[i]);
		str += str2;
	}
	str += L"\r\n";
	for (int i = 20; i < 29; i++)
	{
		str2.Format(L"%d, ", trainer.scores[i]);
		str += str2;
	}
	str2.Format(L"%d ", trainer.scores[29]);
	str += str2;
	ScoresList.SetWindowTextW(str);
}
void MyDlg::OnBnClickedOk()
{
	// TODO: добавьте свой код обработчика уведомлений
	int id = CBGameMode.GetCurSel();
	if (id == -1)return;

	CString str;
	CBGameMode.GetLBText(id, str);

	GameDlg dlg;
	if (str == L"2 игрока")
	{
		dlg.gm = pvp;
	}
	else
		if (str == L"Игрок против нейросети")
		{
			dlg.gm = pvn;
		}
		else
			if (str == L"Нейросеть против игрока")
			{
				dlg.gm = nvp;
			}
	dlg.name = str;

	CRect rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	dlg.posx = rect.Width() / 2. - 400;
	dlg.posy = 50;
	dlg.bot = trainer.OutNW;
	

	dlg.DoModal();
}

//сохранить
void MyDlg::On32771()
{
	// TODO: добавьте свой код обработчика команд
	trainer.SaveBest();
}

DWORD WINAPI TrainingThreadFunc(LPVOID p)
{
	srand(time(NULL));
	MyDlg* dlg = (MyDlg*)p;
	dlg->OkCtr.EnableWindow(FALSE);
	dlg->trainer.train();
	dlg->KillTimer(dlg->timerid);
	dlg->OkCtr.EnableWindow(TRUE);
	dlg->SendMessage(MS_FILLBOT);
	dlg->FillInfo();
	return 0;
}
//обучение
void MyDlg::On32773()
{
	// TODO: добавьте свой код обработчика команд
	TerminateThread(TrainingThread, 0);
	CloseHandle(TrainingThread);
	KillTimer(timerid);

	TrainingThread = CreateThread(NULL, NULL, TrainingThreadFunc, this, NULL, NULL);
	timerid = SetTimer(123, 25, NULL);
}


void MyDlg::OnMenuLoad()
{
	// TODO: добавьте свой код обработчика команд
	trainer.Load();
}


void MyDlg::OnSaveAll()
{
	// TODO: добавьте свой код обработчика команд
	trainer.SaveAll();
}


void MyDlg::OnLoadAll()
{
	// TODO: добавьте свой код обработчика команд
	trainer.LoadALL();
}


void MyDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: добавьте свой код обработчика сообщений или вызов стандартного
	FillInfo();
	CDialogEx::OnTimer(nIDEvent);
}


void MyDlg::OnForcedTrain()
{
	// TODO: добавьте свой код обработчика команд
	if (ftd.DoModal() != IDOK)return;

	TerminateThread(TrainingThread, 0);
	CloseHandle(TrainingThread);
	KillTimer(timerid);

	trainer.ForceToTrain(ftd.n);

	TrainingThread = CreateThread(NULL, NULL, TrainingThreadFunc, this, NULL, NULL);
	timerid = SetTimer(123, 25, NULL);
}
