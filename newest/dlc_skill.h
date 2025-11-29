#ifndef DLC_SKILL_H
#define DLC_SKILL_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>

#include "dlc_time.h"

#define MAX_SKILLS 10
#define SKILL_NAME_LENGTH 50

// Virtual key codes for skills
#define SKILL_KEY_1 '1'
#define SKILL_KEY_2 '2'
#define SKILL_KEY_3 '3'
#define SKILL_KEY_4 '4'
#define SKILL_KEY_5 '5'
#define SKILL_KEY_6 '6'
#define SKILL_KEY_7 '7'
#define SKILL_KEY_8 '8'
#define SKILL_KEY_9 '9'
#define SKILL_KEY_10 '0'

// Skill structure
typedef struct
{
    char name[SKILL_NAME_LENGTH];
    int cooldown;           // Cooldown time in seconds
    int remainingCD;        // Remaining cooldown time
    int isReady;            // Whether skill is ready
    int skillID;            // Skill ID
    UINT vkCode;            // Virtual key code
    void (*activate)(void); // Skill activation function pointer
} Skill;

// Skill manager
typedef struct
{
    Skill skills[MAX_SKILLS];
    int skillCount;
    int isRunning;
    HANDLE skillThread;
    CRITICAL_SECTION cs;
} SkillManager;

// Global skill manager instance
SkillManager skillManager = {0};

// Function declarations
void initializeSkillManager()
{
    InitializeCriticalSection(&skillManager.cs);
    skillManager.skillCount = 0;
    skillManager.isRunning = 0;
    skillManager.skillThread = NULL;

    // Initialize all skill slots
    for (int i = 0; i < MAX_SKILLS; i++)
    {
        skillManager.skills[i].skillID = -1;
        skillManager.skills[i].isReady = 1;
        skillManager.skills[i].remainingCD = 0;
    }
}

void stopSkillManager()
{
    skillManager.isRunning = 0;

    if (skillManager.skillThread)
    {
        WaitForSingleObject(skillManager.skillThread, INFINITE);
        CloseHandle(skillManager.skillThread);
        skillManager.skillThread = NULL;
    }
}

void cleanupSkillManager()
{
    stopSkillManager();
    DeleteCriticalSection(&skillManager.cs);
}

int addSkill(const char *name, int cooldown, UINT vkCode, void (*activateFunc)(void))
{
    EnterCriticalSection(&skillManager.cs);

    if (skillManager.skillCount >= MAX_SKILLS)
    {
        LeaveCriticalSection(&skillManager.cs);
        return -1; // Skill slots full
    }

    int newID = skillManager.skillCount;
    Skill *newSkill = &skillManager.skills[newID];

    strncpy(newSkill->name, name, SKILL_NAME_LENGTH - 1);
    newSkill->name[SKILL_NAME_LENGTH - 1] = '\0';
    newSkill->cooldown = cooldown;
    newSkill->remainingCD = 0;
    newSkill->isReady = 1;
    newSkill->skillID = newID;
    newSkill->vkCode = vkCode;
    newSkill->activate = activateFunc;

    skillManager.skillCount++;

    LeaveCriticalSection(&skillManager.cs);
    return newID;
}

void removeSkill(int skillID)
{
    EnterCriticalSection(&skillManager.cs);

    if (skillID >= 0 && skillID < skillManager.skillCount)
    {
        // Shift skills forward
        for (int i = skillID; i < skillManager.skillCount - 1; i++)
        {
            skillManager.skills[i] = skillManager.skills[i + 1];
            skillManager.skills[i].skillID = i;
        }
        skillManager.skillCount--;

        // Clear last skill slot
        skillManager.skills[skillManager.skillCount].skillID = -1;
    }

    LeaveCriticalSection(&skillManager.cs);
}

void updateSkillCooldowns()
{
    EnterCriticalSection(&skillManager.cs);

    for (int i = 0; i < skillManager.skillCount; i++)
    {
        Skill *skill = &skillManager.skills[i];

        if (!skill->isReady && skill->remainingCD > 0)
        {
            skill->remainingCD--;

            if (skill->remainingCD <= 0)
            {
                skill->isReady = 1;
                skill->remainingCD = 0;
            }
        }
    }

    LeaveCriticalSection(&skillManager.cs);
}

void displaySkills()
{
    EnterCriticalSection(&skillManager.cs);

    printf("\n=== Skill List ===\n");
    for (int i = 0; i < skillManager.skillCount; i++)
    {
        Skill *skill = &skillManager.skills[i];

        if (skill->isReady)
        {
            printf("%c. %s -  READY\n", skill->vkCode, skill->name);
        }
        else
        {
            printf("%c. %s -  COOLDOWN (%ds)\n", skill->vkCode, skill->name, skill->remainingCD);
        }
    }
    printf("==================\n");

    LeaveCriticalSection(&skillManager.cs);
}

int getReadySkillCount()
{
    EnterCriticalSection(&skillManager.cs);

    int count = 0;
    for (int i = 0; i < skillManager.skillCount; i++)
    {
        if (skillManager.skills[i].isReady)
        {
            count++;
        }
    }

    LeaveCriticalSection(&skillManager.cs);
    return count;
}

// Check if skill key is pressed using &0x8000 method
int isSkillKeyPressed(UINT vkCode)
{
    return (GetAsyncKeyState(vkCode) & 0x8000) != 0;
}

// Check all skill keys and activate if pressed
void checkSkillKeys()
{
    EnterCriticalSection(&skillManager.cs);

    for (int i = 0; i < skillManager.skillCount; i++)
    {
        Skill *skill = &skillManager.skills[i];
        if (isSkillKeyPressed(skill->vkCode) && skill->isReady && skill->activate != NULL)
        {
            // Small delay to prevent multiple triggers
            static DWORD lastPressTime[MAX_SKILLS] = {0};
            DWORD currentTime = GetTickCount();

            if (currentTime - lastPressTime[i] > 500)
            { // 500ms debounce
                lastPressTime[i] = currentTime;
                skill->isReady = 0;
                skill->remainingCD = skill->cooldown;
                skill->activate();
            }
        }
    }

    LeaveCriticalSection(&skillManager.cs);
}

DWORD WINAPI skillThreadFunction(LPVOID lpParam)
{
    while (skillManager.isRunning)
    {
        Sleep(100); // Check keys more frequently (100ms)
        checkSkillKeys();
        updateSkillCooldowns();
    }
    return 0;
}

void startSkillManager()
{
    if (skillManager.isRunning)
    {
        return;
    }

    skillManager.isRunning = 1;
    skillManager.skillThread = CreateThread(
        NULL, 0, skillThreadFunction, NULL, 0, NULL);
}



// ========== Predefined Skill Effects ==========

/*
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


// Initialize default skills
void initializeDefaultSkills()
{
    addSkill("Extra Time", 90, SKILL_KEY_1, skill_ExtraTime);
    addSkill("Reset CD", 120, SKILL_KEY_2, skill_ResetCooldowns);
}
*/

#endif // DLC_SKILL_H