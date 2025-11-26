#include <time.h>
#include <stdint.h>
#include <windows.h>
#include <math.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
typedef struct
{
    int enabled;                // 音效开关
    float volume;               // 音量 0.0-1.0
    int backgroundMusicPlaying; // 背景音乐状态
    time_t lastPlayTime[10];    // 防止音效重叠
} SoundManager;

SoundManager soundManager = {1, 0.7f, 0, {0}};

void playTone(int frequency, int dur)
{
    int duration = dur ;
    if (frequency <= 0)
        return;

    DWORD sampleRate = 44100;
    int samples = duration * sampleRate / 1000;
    if (samples <= 0)
        return;

    // 分配音频缓冲区
    short *buffer = (short *)malloc(samples * sizeof(short));
    if (!buffer)
        return;

    // 生成正弦波
    double amplitude = 8000.0;
    for (int i = 0; i < samples; i++)
    {
        double time = (double)i / sampleRate;
        buffer[i] = (short)(amplitude * sin(2.0 * 3.14159 * frequency * time));
    }

    // 设置WAV格式
    WAVEFORMATEX waveFormat;
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nChannels = 1;
    waveFormat.nSamplesPerSec = sampleRate;
    waveFormat.nAvgBytesPerSec = sampleRate * sizeof(short);
    waveFormat.nBlockAlign = sizeof(short);
    waveFormat.wBitsPerSample = 16;
    waveFormat.cbSize = 0;

    HWAVEOUT hWaveOut;
    WAVEHDR waveHeader = {0};

    // 打开音频设备并播放
    if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, CALLBACK_NULL) == MMSYSERR_NOERROR)
    {
        waveHeader.lpData = (LPSTR)buffer;
        waveHeader.dwBufferLength = samples * sizeof(short);

        waveOutPrepareHeader(hWaveOut, &waveHeader, sizeof(WAVEHDR));
        waveOutWrite(hWaveOut, &waveHeader, sizeof(WAVEHDR));

        // 等待播放完成
        Sleep(duration + 50);

        waveOutUnprepareHeader(hWaveOut, &waveHeader, sizeof(WAVEHDR));
        waveOutClose(hWaveOut);
    }

    free(buffer);
}
void playPiano(int frequency, double duration_ms)
{
    if (frequency <= 0 || duration_ms <= 0)
        return;

    DWORD sampleRate = 44100;
    int samples = duration_ms * sampleRate / 1000;
    if (samples <= 0)
        return;

    // 分配音频缓冲区
    short *buffer = (short *)malloc(samples * sizeof(short));
    if (!buffer)
        return;

    // 钢琴音色参数
    double harmonics[] = {1.0, 0.5, 0.25, 0.125, 0.0625};
    double harmonicFreqs[] = {1.0, 2.0, 3.0, 4.0, 6.0};
    double amplitude = 8000.0;

    // 生成钢琴音色
    for (int i = 0; i < samples; i++)
    {
        double time = (double)i / sampleRate;

        // 简化的包络 - 确保在结束时完全衰减到0
        double currentTime = (double)i / sampleRate;
        double totalTime = (double)duration_ms / 1000.0;
        double envelope = 1.0;

        if (currentTime < 0.05)
        {
            // 快速起音
            envelope = currentTime / 0.05;
        }
        else if (currentTime > totalTime - 0.05)
        {
            // 确保在结束时完全衰减到0
            double fadeOut = (totalTime - currentTime) / 0.05;
            envelope = fadeOut;
            if (envelope < 0.01)
                envelope = 0; // 完全静音
        }

        // 合成谐波
        double sample = 0.0;
        for (int h = 0; h < 5; h++)
        {
            sample += harmonics[h] * sin(2.0 * 3.14159 * frequency * harmonicFreqs[h] * time);
        }

        buffer[i] = (short)(amplitude * envelope * sample);
    }

    // 设置WAV格式
    WAVEFORMATEX waveFormat;
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nChannels = 1;
    waveFormat.nSamplesPerSec = sampleRate;
    waveFormat.nAvgBytesPerSec = sampleRate * sizeof(short);
    waveFormat.nBlockAlign = sizeof(short);
    waveFormat.wBitsPerSample = 16;
    waveFormat.cbSize = 0;

    HWAVEOUT hWaveOut;
    WAVEHDR waveHeader = {0};

    // 打开音频设备并播放
    if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, CALLBACK_NULL) == MMSYSERR_NOERROR)
    {
        waveHeader.lpData = (LPSTR)buffer;
        waveHeader.dwBufferLength = samples * sizeof(short);
        waveHeader.dwFlags = 0;

        waveOutPrepareHeader(hWaveOut, &waveHeader, sizeof(WAVEHDR));
        waveOutWrite(hWaveOut, &waveHeader, sizeof(WAVEHDR));

        // 关键：正确等待播放完成
        while (!(waveHeader.dwFlags & WHDR_DONE))
        {
            Sleep(1);
        }

        waveOutUnprepareHeader(hWaveOut, &waveHeader, sizeof(WAVEHDR));
        waveOutClose(hWaveOut);
    }

    free(buffer);
}
void playPianoset(char put[], int bpm,int asset)
{
    double tbpm = 60000 / bpm;
    int set[] = {262, 294, 330, 349, 392,440,494,523,  587,  659,  698,784, 880,  988, 1047};
    int i = 0;
    while(put[i]!='\0'){
        int k = 1;
        int pk = i;
        while(put[pk+1]=='0'){
            k++;
            pk++;
        }
        if(put[i]!='0')playPiano(set[put[i]-'1'+asset+7], tbpm*k);
        i++;
    }
}

void playTonedot(char put, int bpm, int asset,int op[],int n)
{
    double tbpm = 60000 / bpm;
    int set[] = {262, 294, 330, 349, 392, 440, 494, 523, 587, 659, 698, 784, 880, 988, 1047};
    int bset[] ={247, 277, 311, 330, 370, 415, 466, 494, 554, 622, 659, 740, 831, 932, 988 };
    for (int i = 0; i < n;i++){
        set[op[i] + 6] = bset[op[i] + 6];
    }
        playTone(set[put - '1' + asset + 7], tbpm);
}

void playPianodot(char put, int bpm, int asset, int op[], int n)
{
    double tbpm = 60000 / bpm;
    int set[] = {262, 294, 330, 349, 392, 440, 494, 523, 587, 659, 698, 784, 880, 988, 1047};
    int bset[] = {247, 277, 311, 330, 370, 415, 466, 494, 554, 622, 659, 740, 831, 932, 988};
    for (int i = 0; i < n; i++)
    {
        set[op[i] + 6] = bset[op[i] + 6];
    }
    playPiano(set[put - '1' + asset + 7], tbpm);
}

typedef struct
{
    char note;
    int instrument; // 0=piano, 1=tone
    int duration;
} TrackParams;
// 统一的音轨线程函数
DWORD WINAPI soundTrackThread(LPVOID lpParam)
{


    TrackParams *params = (TrackParams *)lpParam;

    int b[] = {7};
    if (params->instrument == 0)
    {
        playPianodot(params->note, params->duration, 0, b, 1);
    }
    else
    {
        playTonedot(params->note, params->duration, 0, b, 1);
    }

    free(params);
    return 0;
}

void playMoveSound(int t)
{
    char a[] = "202402500203434605400400202402500203434605400400808876500203434545600600404456800706503605200200";
    char v[] = "266266266266266266266210266266266266266266266266566566266266266266266266466466266266266266266266";

    char op1 = a[(t - 1) % 96];
    char op2 = v[(t - 1) % 96];

    if (!soundManager.enabled)
        return;

    // 创建音轨1线程
    if (op1 != '0')
    {
        TrackParams *params1 = malloc(sizeof(TrackParams));
        params1->note = op1;
        params1->instrument = 0; // piano
        params1->duration = 600;

        DWORD threadId;
        HANDLE hThread = CreateThread(NULL, 0, soundTrackThread, params1, 0, &threadId);
        if (hThread)
            CloseHandle(hThread);
    }

    // 创建音轨2线程
    if (op2 != '0')
    {
        TrackParams *params2 = malloc(sizeof(TrackParams));
        params2->note = op2;
        params2->instrument = 1; // tone
        params2->duration = 600;

        DWORD threadId;
        HANDLE hThread = CreateThread(NULL, 0, soundTrackThread, params2, 0, &threadId);
        if (hThread)
            CloseHandle(hThread);
    }
}

DWORD WINAPI victorySoundThread(LPVOID lpParam)
{
    char a[] = "567890508764500";
    playPianoset(a, 220, -4);
    return 0;
}
// === 新增：非阻塞播放函数 ===
void playVictorySound()
{
    if (!soundManager.enabled)
        return;

    // 创建新线程播放音效，不阻塞主程序
    DWORD threadId;
    HANDLE hThread = CreateThread(NULL, 0, victorySoundThread, NULL, 0, &threadId);
    if (hThread)
    {
        CloseHandle(hThread); // 不等待线程结束
    }
}

// 胜利音效线程函数
