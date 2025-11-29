#ifndef DLC_ASCENDING_H
#define DLC_ASCENDING_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
typedef struct
{
    char name[50];
    int time;
    int moves;
    int scale;
    time_t date;
} RankRecord;

// 函数声明
void saveRank(int scale, int moves, int timeUsed);
void showRanks(int scale);
int compareRanks(const void *a, const void *b);

// 在 main.c 中实现排行榜功能

// 比较函数用于排序
int compareRanks(const void *a, const void *b)
{
    RankRecord *ra = (RankRecord *)a;
    RankRecord *rb = (RankRecord *)b;

    // 先按时间排序，时间相同按步数排序
    if (ra->time != rb->time)
    {
        return ra->time - rb->time;
    }
    return ra->moves - rb->moves;
}

// 保存排行榜记录
void saveRank(int scale, int moves, int timeUsed)
{
    FILE *file = fopen("source/level.txt", "a");
    if (file == NULL)
    {
        // 如果目录不存在，尝试创建
        system("mkdir source 2>nul");
        file = fopen("source/level.txt", "a");
        if (file == NULL)
        {
            printf("error\n");
            return;
        }
    }

    char name[50];
    printf("\npress in your name");
    scanf("%s", name);

    fprintf(file, "%s,%d,%d,%d,%ld\n", name, scale, moves, timeUsed, time(NULL));
    fclose(file);

    printf("record saved\n");
}

// 显示排行榜
void showRanks(int scale)
{
    FILE *file = fopen("source/level.txt", "r");
    if (file == NULL)
    {
        printf("暂无排行榜数据\n");
        return;
    }

    RankRecord records[100];
    int count = 0;
    char line[256];

    // 读取所有记录
    while (fgets(line, sizeof(line), file) && count < 100)
    {
        char name[50];
        int s, moves, timeUsed;
        time_t date;

        if (sscanf(line, "%49[^,],%d,%d,%d,%ld", name, &s, &moves, &timeUsed, &date) == 5)
        {
            if (s == scale)
            { // 只显示当前难度的记录
                strcpy(records[count].name, name);
                records[count].scale = s;
                records[count].moves = moves;
                records[count].time = timeUsed;
                records[count].date = date;
                count++;
            }
        }
    }
    fclose(file);

    if (count == 0)
    {
        printf("no %dx%d record\n", scale, scale);
        return;
    }

    // 排序
    qsort(records, count, sizeof(RankRecord), compareRanks);

    // 显示前10名
    printf("\n====== %dx%d 难度排行榜 ======\n", scale, scale);
    printf("rank  name          time   turn    date\n");
    printf("----------------------------------------\n");

    for (int i = 0; i < count && i < 10; i++)
    {
        char dateStr[20];
        struct tm *timeinfo = localtime(&records[i].date);
        strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", timeinfo);

        int minutes = records[i].time / 60;
        int seconds = records[i].time % 60;

        printf("%2d.   %-12s %02d:%02d    %-6d %s\n",
               i + 1, records[i].name, minutes, seconds,
               records[i].moves, dateStr);
    }
}

#endif // DLC_ASCENDING_H