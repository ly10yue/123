#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <stdlib.h>
#include <time.h>

// 全局窗口列表和计数器
#define MAX_WINDOWS 50
static HWND g_windows[MAX_WINDOWS];
static int g_windowCount = 0;
static CRITICAL_SECTION g_cs;
static HFONT g_hFont = NULL;

// 创建字体
void CreateCustomFont()
{
    if (g_hFont)
        return;

    // 创建漂亮的字体
    g_hFont = CreateFont(
        18,                       // 字体高度
        0,                        // 宽度
        0,                        // 倾斜度
        0,                        // 方向
        FW_NORMAL,                // 粗细
        FALSE,                    // 斜体
        FALSE,                    // 下划线
        FALSE,                    // 删除线
        DEFAULT_CHARSET,          // 字符集
        OUT_DEFAULT_PRECIS,       // 输出精度
        CLIP_DEFAULT_PRECIS,      // 剪裁精度
        CLEARTYPE_QUALITY,        // 质量
        DEFAULT_PITCH | FF_SWISS, // 间距和字体族
        "Segoe UI"                // 字体名称
    );

    // 如果Segoe UI不可用，fallback到Tahoma
    if (!g_hFont)
    {
        g_hFont = CreateFont(
            18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS,
            "Tahoma");
    }

    // 如果还不行，使用系统默认字体
    if (!g_hFont)
    {
        g_hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    }
}

// 自定义消息框窗口过程
LRESULT CALLBACK CustomMessageBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HWND hStatic = NULL;
    static HWND hButton = NULL;

    switch (msg)
    {
    case WM_CREATE:
    {
        // 获取创建参数
        CREATESTRUCT *create = (CREATESTRUCT *)lParam;
        char *message = (char *)create->lpCreateParams;

        // 创建静态文本
        hStatic = CreateWindow("STATIC", message,
                               WS_VISIBLE | WS_CHILD | SS_CENTER,
                               15, 25, 270, 40, hwnd, NULL, GetModuleHandle(NULL), NULL);

        // 设置字体
        if (g_hFont && hStatic)
        {
            SendMessage(hStatic, WM_SETFONT, (WPARAM)g_hFont, TRUE);
        }

        // 创建OK按钮
        hButton = CreateWindow("BUTTON", "NONONONO",
                               WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_TABSTOP,
                               110, 75, 80, 30, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);

        // 设置按钮字体
        if (g_hFont && hButton)
        {
            SendMessage(hButton, WM_SETFONT, (WPARAM)g_hFont, TRUE);
        }

        break;
    }
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE)
        {
            SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == 1)
        {
            EnterCriticalSection(&g_cs);
            for (int i = 0; i < g_windowCount; i++)
            {
                if (g_windows[i] == hwnd)
                {
                    for (int j = i; j < g_windowCount - 1; j++)
                    {
                        g_windows[j] = g_windows[j + 1];
                    }
                    g_windowCount--;
                    break;
                }
            }
            LeaveCriticalSection(&g_cs);

            SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            DestroyWindow(hwnd);
        }
        break;
    case WM_DESTROY:
    {
        // 释放消息内存
        CREATESTRUCT *create = (CREATESTRUCT *)lParam;
        if (create && create->lpCreateParams)
        {
            free((void *)create->lpCreateParams);
        }
        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// 生成随机位置
void GetRandomPosition(int *x, int *y)
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    *x = 50 + rand() % (screenWidth - 400);
    *y = 50 + rand() % (screenHeight - 200);
}

BOOL IsPositionOverlap(int x, int y)
{
    EnterCriticalSection(&g_cs);
    for (int i = 0; i < g_windowCount; i++)
    {
        RECT rect;
        if (GetWindowRect(g_windows[i], &rect))
        {
            if (x < rect.right + 20 && x + 320 + 20 > rect.left &&
                y < rect.bottom + 20 && y + 140 + 20 > rect.top)
            {
                LeaveCriticalSection(&g_cs);
                return TRUE;
            }
        }
    }
    LeaveCriticalSection(&g_cs);
    return FALSE;
}

void ArrangeAllWindows()
{
    EnterCriticalSection(&g_cs);
    for (int i = 0; i < g_windowCount; i++)
    {
        if (g_windows[i] && IsWindow(g_windows[i]))
        {
            SetWindowPos(g_windows[i], HWND_TOP, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }
    LeaveCriticalSection(&g_cs);
}

HWND CreateCustomMessageBox(const char *message, const char *title)
{
    static BOOL classRegistered = FALSE;
    if (!classRegistered)
    {
        WNDCLASS wc = {0};
        wc.lpfnWndProc = CustomMessageBoxProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = "CustomMessageBox";
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // 使用系统颜色
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

        if (RegisterClass(&wc))
        {
            classRegistered = TRUE;
        }
    }

    int posX, posY;
    int attempts = 0;
    do
    {
        GetRandomPosition(&posX, &posY);
        attempts++;
    } while (IsPositionOverlap(posX, posY) && attempts < 50);

    char *messageCopy = _strdup(message);
    if (!messageCopy)
        return NULL;

    // 创建窗口
    HWND hwnd = CreateWindow(
        "CustomMessageBox",
        title,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        posX,
        posY,
        320, // 宽度
        140, // 高度
        NULL,
        NULL,
        GetModuleHandle(NULL),
        messageCopy);

    if (hwnd)
    {
        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

        EnterCriticalSection(&g_cs);
        if (g_windowCount < MAX_WINDOWS)
        {
            g_windows[g_windowCount++] = hwnd;
        }
        LeaveCriticalSection(&g_cs);

        ArrangeAllWindows();
    }
    else
    {
        free(messageCopy);
    }

    return hwnd;
}

unsigned __stdcall MessageBoxThread(void *param)
{
    char *message = ((char **)param)[0];
    char *title = ((char **)param)[1];

    HWND hwnd = CreateCustomMessageBox(message, title);

    if (hwnd)
    {
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    else
    {
        free(message);
        free(title);
        free(param);
    }

    return 0;
}

void ShowMessage(const char *message, const char *title, const char *colorName)
{
    char **params = (char **)malloc(2 * sizeof(char *));
    if (!params)
        return;

    params[0] = _strdup(message);
    params[1] = _strdup(title);

    if (params[0] && params[1])
    {
        _beginthreadex(NULL, 0, MessageBoxThread, params, 0, NULL);
    }
    else
    {
        free(params[0]);
        free(params[1]);
        free(params);
    }
}

void error(int t)
{
    // 创建漂亮字体
    CreateCustomFont();

    srand((unsigned int)time(NULL));
    InitializeCriticalSection(&g_cs);

    int threadCount =  3 * t;
    for (int i = 0; i < threadCount; i++)
    {
        char msg[100];
        sprintf_s(msg, sizeof(msg), "are YOU alone??");
        ShowMessage(msg, "Error", "RED");
        Sleep(200);
    }
    while (1)
    {
        Sleep(500);
        EnterCriticalSection(&g_cs);
        int remaining = g_windowCount;
        LeaveCriticalSection(&g_cs);

        if (remaining == 0)
            break;
        ArrangeAllWindows();
    }

    // 清理字体
    if (g_hFont)
    {
        DeleteObject(g_hFont);
        g_hFont = NULL;
    }

    DeleteCriticalSection(&g_cs);
}