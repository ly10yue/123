#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <string.h>

#include "dlc_music.h"
#include "dlc_time.h"
#include "dlc_skill.h"
#include "dlc_ascending.h"

void clean(){
    cleanupTimer();
    stopSkillManager();
    cleanupSkillManager();
}

void skill_ExtraTime()
{
    printf(" Extra Time! Gain additional 30 seconds\n");
    addTime(30);
}

void skill_ResetCooldowns()
{
    printf(" Reset Cooldowns! All skills are ready\n");
    // Reset all skill cooldowns
}

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
    return (a % 2) ? (inversions % 2 == 0) : ((inversions + blankRowFromBottom) % 2==1);
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

int main()
{

    printf("do you want to add the sound? (y/n)");
    char t;
    scanf("%c", &t);
    if (t == 'n' || t == 'N')
        soundManager.enabled = 0;
    printf("\n\n");
    printf("                                      \n");
    printf("         welcome to numbermaze        \n");
    printf("                                      \n");
    printf("\n\n");


    printf("1.enter the game \n");
    printf("2.see the data\n");
    int choice;
    scanf("%d", &choice);
    if (choice == 2)
    {
        printf("choose the scale ");
        int scale;
        scanf("%d", &scale);
        showRanks(scale);
        printf("\nok.");
        getch();
        main(); // 返回主菜单
        return 0;
    }

    char inp[1000];
    int ti;
    printf("choose the scale and start!!! (Suggested range: 2 - 5)");
    int a;
    scanf("%d", &a);
    set_time(a * a * 5);
    initializeTimer();
    startTimer();
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
    if (!iS(a, tu))
    {
        int temp = tu[0][0];
        tu[0][0] = tu[0][1];
        tu[0][1] = temp;
        if (index[0] == 0 && index[1] == 1)
            index[1] = 0;
        else if (index[0] == 0 && index[1] == 0)
            index[1] = 1;
    }
    int count_anti_cheat = count * (-32) + 196;

    scanf("%c", &op);
    initializeSkillManager();
    addSkill("Extra Time", 90, SKILL_KEY_1, skill_ExtraTime);
    addSkill("Reset CD", 120, SKILL_KEY_2, skill_ResetCooldowns);
    startSkillManager();

    autoplayMoveSound();
    while (flag)
    {
        system("cls");
        displayTimeLeft();
        printf("\n\n");
        print(a, tu);
        displaySkills();
        printf("choose next step(w,s,a,d and space for judge)");

        if (isTimeUp())
        {
            flag = 0;
            continue;
        }

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
        if (count != (count_anti_cheat - 196) / (-32) ){
            printf("Mom:why you want to CHEAT???????\n");
            scanf("%c");
            clean();
            return 0;
        }
    }

    stopTimer();

    // 检查是否在时间内完成
    if (isTimeUp())
    {
        printf("ohno! Time Limit Exceeded!:(\n");
        // 可以添加超时音效
    }
    int past = getElapsedTime();
    clean();
    int coul = 1;
    int flg = 1;

    for (int i = 0; i < a; i++)
    {
        for (int j = 0; j < a; j++)
        {
            if (tu[i][j] != coul && flg)
            {
                printf("\nthere's some problem.you lose:(\n");
                flg = 0;
            }
            coul = (coul + 1) % (a * a);
        }
    }
    if (flg)
    {
        printf("\nyou win!!!!!!\n");
        playVictorySound();
        int totalTime = a * a * 30 - past;

        // 保存记录
        saveRank(a, count, totalTime);

        // 显示当前难度排行榜
        printf("\n");
        showRanks(a);
    }
    
    scanf("%c", &op);
}