#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <string.h>

#include "dlc_error.h"
#include "dlc_music.h"
#include <windows.h>
#include <signal.h>
#include <setjmp.h>

#define COLOR_BLACK 0
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_CYAN 3
#define COLOR_RED 4
#define COLOR_MAGENTA 5
#define COLOR_YELLOW 6
#define COLOR_WHITE 7
#define COLOR_GRAY 8
#define COLOR_BRIGHT_BLUE 9
#define COLOR_BRIGHT_GREEN 10
#define COLOR_BRIGHT_CYAN 11
#define COLOR_BRIGHT_RED 12
#define COLOR_BRIGHT_MAGENTA 13
#define COLOR_BRIGHT_YELLOW 14
#define COLOR_BRIGHT_WHITE 15


void setColor(int textColor, int bgColor)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);
}

// 重置为默认颜色
void resetColor()
{
    setColor(COLOR_WHITE, COLOR_BLACK);
}

int allowExit = 0; // 是否允许退出
HWND consoleHwnd = NULL;

jmp_buf exit_buf;

BOOL WINAPI ConsoleHandler(DWORD signal)
{
    switch (signal)
    {
    case CTRL_CLOSE_EVENT: // 点击X按钮
    case CTRL_C_EVENT:     // Ctrl+C
    case CTRL_BREAK_EVENT: // Ctrl+Break
        if (allowExit)
        {
            return TRUE; // 允许关闭
        }
        else
        {

            // 强力阻止：创建一个新的控制台窗口
            // 修改ConsoleHandler中的创建进程部分
            STARTUPINFO si1 = {0};
            STARTUPINFO si2 = {0};
            PROCESS_INFORMATION pi1 = {0};
            PROCESS_INFORMATION pi2 = {0};

            si1.cb = sizeof(si1);
            si1.dwFlags = STARTF_USESHOWWINDOW;
            si1.wShowWindow = SW_SHOW;

            si2.cb = sizeof(si2);
            si2.dwFlags = STARTF_USESHOWWINDOW;
            si2.wShowWindow = SW_SHOW;

            char cmdLine[MAX_PATH];
            GetModuleFileNameA(NULL, cmdLine, MAX_PATH);

            // 为两个新窗口准备不同的命令行参数
            char cmdLineHello1[MAX_PATH + 50];
            char cmdLineHello2[MAX_PATH + 50];
            sprintf(cmdLineHello1, "%s --hello 1", cmdLine);
            sprintf(cmdLineHello2, "%s --hello 2", cmdLine);

            // 启动第一个新实例
            if (CreateProcessA(NULL, cmdLineHello1, NULL, NULL, FALSE,
                               CREATE_NEW_CONSOLE,
                               NULL, NULL, &si1, &pi1))
            {
                CloseHandle(pi1.hProcess);
                CloseHandle(pi1.hThread);
            }

            // 启动第二个新实例
            if (CreateProcessA(NULL, cmdLineHello2, NULL, NULL, FALSE,
                               CREATE_NEW_CONSOLE,
                               NULL, NULL, &si2, &pi2))
            {
                CloseHandle(pi2.hProcess);
                CloseHandle(pi2.hThread);
            }

            setColor(COLOR_RED, COLOR_BLACK);
            printf("as i've told you,you can NEVER escape the maze.");
            resetColor();
            system("cls");
            Sleep(5000);
            return TRUE;
        }
    default:
        return FALSE;
    }
}

void setupCloseProtection()
{
    // 设置控制台事件处理
    SetConsoleCtrlHandler(ConsoleHandler, TRUE);
}

// 定义颜色常量


// 设置文本颜色函数


int countInversions(int arr[], int size)
{
    int inversions = 0;
    for (int i = 0; i < size - 1; i++)
    {
        if (arr[i] == 0)
            continue;
        for (int j = i + 1; j < size; j++)
        {
            if (arr[j] != 0 && arr[i] > arr[j])
            {
                inversions++;
            }
        }
    }
    return inversions;
}

int iS(int a, int tu[][a])
{
    int size = a * a;
    int flat[size];
    int k = 0;

    for (int i = 0; i < a; i++)
    {
        for (int j = 0; j < a; j++)
        {
            flat[k++] = tu[i][j];
        }
    }
    int inversions = countInversions(flat, size);
    int blankRowFromBottom;
    for (int i = 0; i < a; i++)
    {
        for (int j = 0; j < a; j++)
        {
            if (tu[i][j] == 0)
            {
                blankRowFromBottom = a - i;
                break;
            }
        }
    }
    return (a % 2) ? (inversions % 2 == 0) : ((inversions + blankRowFromBottom) % 2 == 1);
}

void randin(int arr[], int size)
{
    for (int i = 0; i < size; i++)
    {
        arr[i] = i;
    }
    // Fisher-Yates洗牌算法
    for (int i = size - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

void print(int a, int t[][a])
{

    printf("the statement by now:\n");
    for (int i = 0; i < a; i++)
    {
        printf("________________\n");
        for (int j = 0; j < a; j++)
        {
            printf("|%2d  ", t[i][j]);
        }
        printf("|");
        printf("\n");
    }
    printf("________________\n");
}

void findspace(int index[2], int a, int tu[][a])
{
    for (int i = 0; i < a; i++)
    {
        for (int j = 0; j < a; j++)
        {
            if (tu[i][j] == 0)
            {
                index[0] = i;
                index[1] = j;
                return;
            }
        }
    }
    return;
}

void another_game()
{
    setColor(COLOR_BRIGHT_WHITE, COLOR_RED);

    printf("\n\n");
    printf("                                      \n");
    printf("         welcome to ezamrebmun        \n");
    printf("                                      \n");
    printf("\n\n");
    printf("just follow the 我是乱码's step.\n");
    int a = 3;
    int tu[3][3];
    int r = 3 * 3;
    int cs[r];
    srand(time(NULL));
    randin(cs, r);
    int cou = 0;

    for (int i = 0; i < a; i++)
    {
        for (int j = 0; j < a; j++)
        {
            tu[i][j] = cs[cou];
            cou++;
        }
    }
    int index[2];
    findspace(index, a, tu);
    char op;

    int count = 0;
    if (iS(a, tu))
    {
        int temp = tu[0][0];
        tu[0][0] = tu[0][1];
        tu[0][1] = temp;
        if (index[0] == 0 && index[1] == 1)
            index[1] = 0;
        else if (index[0] == 0 && index[1] == 0)
            index[1] = 1;
    }

    scanf("%c", &op);
    int flag = 1;
    int count_anti_cheat = count * (-32) + 196;
    while (flag)
    {
        print(a, tu);
        printf("choose next step(w,s,a,d and space for judge)");
        op = getch();
        switch (op)
        {
        case 'a':
            if (index[1] < a - 1)
            {
                tu[index[0]][index[1]] = tu[index[0]][index[1] + 1];
                tu[index[0]][index[1] + 1] = 0;
                index[1]++;
            }
            break;
        case 'd':
            if (index[1])
            {
                tu[index[0]][index[1]] = tu[index[0]][index[1] - 1];
                tu[index[0]][index[1] - 1] = 0;
                index[1]--;
            }
            break;
        case 's':
            if (index[0])
            {
                tu[index[0]][index[1]] = tu[index[0] - 1][index[1]];
                tu[index[0] - 1][index[1]] = 0;
                index[0]--;
            }
            break;
        case 'w':
            if (index[0] < a - 1)
            {
                tu[index[0]][index[1]] = tu[index[0] + 1][index[1]];
                tu[index[0] + 1][index[1]] = 0;
                index[0]++;
            }
            break;
        case ' ':
            flag = 0;
            break;
        default:
            break;
        }
        count = (count_anti_cheat - 196) / (-32) + 1;
        count_anti_cheat -= 32;
        printf("the turn: %d\n", count);
        if(count%10==0)
            error(count/10);
    }
    int coul = 0;
    int flg = 1;
    int jud[9] = {1, 2, 3, 8, 0, 4, 7, 6, 5};
    for (int i = 0; i < a; i++)
    {
        for (int j = 0; j < a; j++)
        {
            if (tu[i][j] != jud[coul] && flg)
            {
                printf("you lose:( \n");
                flg = 0;
            }
            coul++;
        }
    }
    if (flg)
    {
        resetColor();
        printf("somewhere,there was cracked.\n");
        allowExit = 1;
    }
    scanf("%c", &op);
    resetColor();
}

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "--hello") == 0)
    {
        setColor(COLOR_BRIGHT_RED, COLOR_BLACK);
        printf("as i've told you,you can NEVER escape the maze.\n");
        resetColor();
    }
    int bycount = 0;
    setupCloseProtection();
    printf("do you want to add the sound?y/n");
    char t;
    scanf("%c", &t);
    if (t == 'n' || t == 'N')
        soundManager.enabled = 0;
    else if(t=='l'){
        allowExit = 1;
    }
    while (1)
    {
        setColor(COLOR_BRIGHT_WHITE, COLOR_BLACK);
        playVictorySound();

        printf("\n\n");
        printf("                                      \n");
        printf("         welcome to numbermaze        \n");
        printf("                                      \n");
        printf("\n\n");
        Sleep(3000);
        printf("play it alone to get the best MAZE.\n");
        printf("choose the scale");

        int a;
        scanf("%d", &a);
        printf("Mom? : oh,you have no choice.the scale if 4.\n");
        a = 4;
        int tu[a][a];
        int r = a * a;
        int cs[r];
        srand(time(NULL));
        randin(cs, r);
        int cou = 0;

        for (int i = 0; i < a; i++)
        {
            for (int j = 0; j < a; j++)
            {
                tu[i][j] = cs[cou];
                cou++;
            }
        }
        int index[2];
        findspace(index, a, tu);
        char op;

        int flag = 1;
        int count = 0;
        if (iS(a, tu))
        {
            int temp = tu[0][0];
            tu[0][0] = tu[0][1];
            tu[0][1] = temp;
            if (index[0] == 0 && index[1] == 1)
                index[1] = 0;
            else if (index[0] == 0 && index[1] == 0)
                index[1] = 1;
        }

        scanf("%c", &op);
        while (flag)
        {
            print(a, tu);
            printf("choose next step(w,s,a,d and space for judge)");
            op = getch();
            switch (op)
            {
            case 'a':
                if (index[1] < a - 1)
                {

                    tu[index[0]][index[1]] = tu[index[0]][index[1] + 1];
                    tu[index[0]][index[1] + 1] = 0;
                    index[1]++;
                }
                break;
            case 'd':
                if (index[1])
                {

                    tu[index[0]][index[1]] = tu[index[0]][index[1] - 1];
                    tu[index[0]][index[1] - 1] = 0;
                    index[1]--;
                }
                break;
            case 's':
                if (index[0])
                {

                    tu[index[0]][index[1]] = tu[index[0] - 1][index[1]];
                    tu[index[0] - 1][index[1]] = 0;
                    index[0]--;
                }
                break;
            case 'w':
                if (index[0] < a - 1)
                {

                    tu[index[0]][index[1]] = tu[index[0] + 1][index[1]];
                    tu[index[0] + 1][index[1]] = 0;
                    index[0]++;
                }
                break;
            case ' ':
                flag = 0;
                break;
            default:
                break;
            }
            count++;
            playMoveSound(count);
            printf("the turn: %d\n", count);
        }
        int coul = 1;
        int flg = 1;
       
        for (int i = 0; i < a; i++)
        {
            for (int j = 0; j < a; j++)
            {
                if (tu[i][j] != coul && flg)
                {
                    printf("there's some problem.you lose:(\n");
                    flg = 0;
                    bycount++;
                }
                coul = (coul + 1) % (a * a);
            }
        }
        if (flg)
        {
            printf("you win!!!!!!\n");
            allowExit = 1;
        }
        if(bycount>=3)
        { 
printf("There's some problem with your program. Please wait for some time.\n");
Sleep(1000);
printf("20%%...\n");
Sleep(1000);
printf("40%%...\n");
Sleep(1000);
printf("55%%...\n");
Sleep(2000);
printf("70%%...\n");
Sleep(2000);
printf("85%%...\n");
Sleep(2500);
printf("99%%...\n\n");
Sleep(3000);
printf("the fix ");
setColor(COLOR_RED, COLOR_BLACK);
printf("Crashed due to unknown reasons.");
resetColor();
printf("please try it llllllllllater...\n\n");

Sleep(7000);
setColor(COLOR_RED, COLOR_BLACK);
printf("??? : wow,it seems that you g0t 1t.\nwant to play another game with me?\n");
resetColor();
Sleep(4000);
            another_game();
        }
        scanf("%c", &op);
    }
}
