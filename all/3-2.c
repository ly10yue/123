#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <string.h>

#include "dlc_music.h"

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

void judge(int a, int tu[][a])
{
    int count = 1;
    for (int i = 0; i < a; i++)
    {
        for (int j = 0; j < a; j++)
        {
            if (tu[i][j] != count)
            {
                printf("there's some problem.you lose:(\n");
                return;
            }
            count = (count + 1) % (a * a);
        }
    }
    printf("you win!!!!!!");
    return;
}
void another_game()
{

    printf("\n\n");
    printf("                                      \n");
    printf("         welcome to ezamrebmun        \n");
    printf("                                      \n");
    printf("\n\n");
    printf("just follow the teacher's step.\n");
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
        playMoveSound(count);
        count_anti_cheat -= 32;
        printf("the turn: %d\n", count);
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
                printf("you lose:( just before dawn...\n");
                flg = 0;
            }
            coul++;
        }
    }
    if (flg)
    {
        printf("you win in finale!!!!!!\n");
    }
    scanf("%c", &op);
}
int main()
{
    printf("do you want to add the sound?y/n");
    char t;
    scanf("%c", &t);
    if (t == 'n' || t == 'N')
        soundManager.enabled = 0;
    playVictorySound();

    printf("\n\n");
    printf("                                      \n");
    printf("         welcome to numbermaze        \n");
    printf("                                      \n");
    printf("\n\n");
    printf("inject a line to enter ......or your flag?! \n\n");
    char inp[1000];

    char *fl = "LYCTF{v01D_0f_th3_m4z3}";
    scanf("%s", &inp);
    if (strcmp(inp, fl) == 0)
    {
        printf("wow,it seems that you g0t 1t.\nwant to play another game with me?\n");
        scanf("%c", &inp);
        another_game();
        return 0;
    }
    printf("choose the scale(3 goes for extra exploit)");

    int a;
    scanf("%d", &a);
    if (a == 3)
    {
        printf("\nMOM:good boy,if you complete the maze in less than 15 turns,i'll give you the flag.\n\n");
    }
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
        playMoveSound(count);
        printf("the turn: %d\n", count);
        if (count != (count_anti_cheat - 196) / (-32) ){
            printf("Mom:why you want to CHEAT???????\n");
            scanf("%c");
            return 0;
        }
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
            }
            coul = (coul + 1) % (a * a);
        }
    }
    if (flg)
    {
        printf("you win!!!!!!\n");
        playVictorySound();
        if (a >= 3 && count <= 15)
            printf("MOM:good job!you flag:LYCTF{v01D_0f_th3_m4z3}\n");
        else
            printf("......?\n");
    }
    scanf("%c", &op);
}
