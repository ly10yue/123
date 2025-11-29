#ifndef DLC_MUSIC_H
#define DLC_MUSIC_H

#define __USE_MINGW_ANSI_STDIO 1
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

// 音阶频率数组声明
extern int sset[];
extern int bset[];

// 音效管理器结构体
typedef struct
{
    int enabled;                // 音效开关
    float volume;               // 音量 0.0-1.0
    int backgroundMusicPlaying; // 背景音乐状态
    time_t lastPlayTime[10];    // 防止音效重叠
    int mp3Playing;             // MP3播放状态
} SoundManager;

// 全局音效管理器声明
extern SoundManager soundManager;

// 音轨参数结构体
typedef struct
{
    char note;
    int instrument; // 0=piano, 1=tone
    int duration;
} TrackParams;

// MP3播放参数结构体
typedef struct
{
    char* filename;
    int loop;       // 是否循环播放
    float volume;   // 音量
} MP3Params;

// 基础音调播放函数
void playTone(int frequency, int duration);

// 小提琴音色播放函数
void playViolin(int frequency, double duration_ms);

// 小提琴音色序列播放
void playViolinset(char put[], int bpm, int asset);

// 带降音的小提琴单音播放
void playViolinodot(char put, int bpm, int asset, int op[], int n);

// 带降音的基础音调单音播放
void playTonedot(char put, int bpm, int asset, int op[], int n);

// 小提琴降音播放（使用局部音阶）
void playViolindot(char put, int bpm, int asset, int op[], int n);

// 音轨线程函数
DWORD WINAPI soundTrackThread(LPVOID lpParam);

// 移动音效播放
void playMoveSound(int t);

// 胜利音效线程函数
DWORD WINAPI victorySoundThread(LPVOID lpParam);

// 非阻塞胜利音效播放
void playVictorySound();

void autoplayMoveSound();
// 范例音乐线程函数
DWORD WINAPI ParagmatismThread(LPVOID lpParam);

// 非阻塞范例音乐播放
void ParagmatismSound();

#endif // DLC_MUSIC_H