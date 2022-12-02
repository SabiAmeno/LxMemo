#include "LxMemo.h"
#include <QFile>
#include <QAction>
#include <QtWidgets/QApplication>
#include <QPropertyAnimation>
#include <Windows.h>
#include <TlHelp32.h>
#include <winuser.h>
#include "util.h"
#include "SqliteDB.h"

char* Wchar2char(const wchar_t* szStr)
{
    int nLen = WideCharToMultiByte(CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL);
    if (nLen == 0)
    {
        return NULL;
    }
    char* pResult = new char[nLen];
    WideCharToMultiByte(CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL);
    return pResult;
}

int KillProcess(LPCSTR processName)
{
    PROCESSENTRY32 pe;
    DWORD id = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hSnapshot, &pe))
    {
        return 0;
    }

    while (1)
    {
        pe.dwSize = sizeof(PROCESSENTRY32);
        if (Process32Next(hSnapshot, &pe) == FALSE)
        {
            break;
        }
        //find processName
        if (wcsicmp(pe.szExeFile, (const wchar_t*)processName) == 0)
        {
            id = pe.th32ProcessID;
            break;
        }
    }
    CloseHandle(hSnapshot);
    //if(id == 0)
    //  return ;

    //Kill The Process
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);
    if (hProcess != NULL)
    {
        TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
    }
    return 1;
}

//判断进程是否存在
DWORD IsExistProcess(const char* szProcessName, DWORD pid)
{
    PROCESSENTRY32 processEntry32;
    HANDLE toolHelp32Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (((int)toolHelp32Snapshot) != -1)
    {
        processEntry32.dwSize = sizeof(processEntry32);
        if (Process32First(toolHelp32Snapshot, &processEntry32))
        {
            do
            {
                int iLen = 2 * wcslen(processEntry32.szExeFile);
                char* chRtn = new char[iLen + 1];
                //转换成功返回为非负值
                wcstombs(chRtn, processEntry32.szExeFile, iLen + 1);
                if (strcmp(szProcessName, chRtn) == 0 && processEntry32.th32ProcessID != pid)
                {
                    return processEntry32.th32ProcessID;
                }
            } while (Process32Next(toolHelp32Snapshot, &processEntry32));
        }
        CloseHandle(toolHelp32Snapshot);
    }
    //
    return -1;
}

typedef struct
{
    HWND    hwndWindow;     // 窗口句柄
    DWORD   dwProcessID;    // 进程ID
}EnumWindowsArg;
///< 枚举窗口回调函数
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    EnumWindowsArg* pArg = (EnumWindowsArg*)lParam;
    DWORD  dwProcessID = 0;
    // 通过窗口句柄取得进程ID
    ::GetWindowThreadProcessId(hwnd, &dwProcessID);
    if (dwProcessID == pArg->dwProcessID)
    {
        pArg->hwndWindow = hwnd;
        // 找到了返回FALSE
        return FALSE;
    }
    // 没找到，继续找，返回TRUE
    return TRUE;
}

HWND GetWindowHwndByPID(DWORD dwProcessID)
{
    HWND hwndRet = NULL;
    EnumWindowsArg ewa;
    ewa.dwProcessID = dwProcessID;
    ewa.hwndWindow = NULL;
    EnumWindows(EnumWindowsProc, (LPARAM)&ewa);
    if (ewa.hwndWindow)
    {
        hwndRet = ewa.hwndWindow;
    }
    return hwndRet;
}

int main(int argc, char* argv[])
{
#ifdef TEST
    SqliteDBTest test;
    test.Test();
#endif
    QApplication::setAttribute(Qt::AA_UseOpenGLES);

    QApplication a(argc, argv);

    QApplication::setWindowIcon(QIcon(":/LxMemo/icons/memo.png"));
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QFile fi(":/LxMemo/icons/style.qss");
    fi.open(QIODevice::ReadOnly);
    a.setStyleSheet(fi.readAll());
    fi.close();

    DWORD opid = -1;
    auto pid = GetCurrentProcessId();
    if ((opid = IsExistProcess("LxMemo.exe", pid)) != -1) {
        HWND hwnd = GetWindowHwndByPID(opid);
        ShowWindow(hwnd, SW_RESTORE);
        SetForegroundWindow(hwnd);
        return 0;
    }

#if 1
    //LucencyDialog dlg;
    ////dlg.SetPureStyle("rgb(235,229,209)");
    //dlg.SetOpacity(0);
    ////dlg.setStyleSheet("background-color: #55000000");
    //dlg.SetAttribute(~WA_MINIMUM_WINDOW & ~WA_MAXIMUM_WINDOW);
    //dlg.SetFixedEnable(false);
    //dlg.SetIcon(":/LxMemo/icons/memo.png");
    //dlg.setWindowTitle(("LxMemo"));

    //LxMemo* w = new LxMemo();
    //dlg.SetWidget(w);
    //dlg.resize(320, 500);
    //dlg.show();
    LxMemo w;
    w.show();
    //auto addBtn = dlg.AddAction(QIcon(":/LxMemo/icons/add.png"), QObject::tr("Add Memo"));
    //auto reclyBtn = dlg.AddAction(QIcon(":/LxMemo/icons/recycle-fill.png"), QObject::tr("Recycle"));

    //std::function<void(bool)> cb = [&](bool)
    //{
    //    addBtn->setVisible(reclyBtn->toolTip() == "Recycle");
    //};

    //QObject::connect(addBtn, &QToolButton::clicked, w, &LxMemo::onAddMemo);
    //QObject::connect(reclyBtn, &QToolButton::clicked, w, &LxMemo::onExchangePanel);
    //QObject::connect(w, &LxMemo::ReDisplay, &dlg, [&dlg] {dlg.show(); });
    //QObject::connect(reclyBtn, &QToolButton::clicked, addBtn, cb);
    //QObject::connect(&dlg, &LucencyDialog::windowClosed, w, &LxMemo::onWindowClosed);

    StartOpacityAnimation(&w, 0, 1, QEasingCurve::InQuint);
#endif
#if 0
    LxMemo w;
    w.show();
#endif

#if 0
    QWidget* mw = new QWidget();
    //mw->resize(600, 500);
    mw->setWindowOpacity(0);
    mw->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    mw->show();

    anim::StartOpacityAnimation(mw, 0, 1, QEasingCurve::Linear);
    anim::StartGeometryAnimation(mw, QRect(400, 400, 1, 600), QRect(400, 400, 200, 600), QEasingCurve::Linear, 500);
#endif
    return a.exec();
}
