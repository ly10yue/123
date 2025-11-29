#include "dlc_music.h"
#include <stdio.h>

int sset[] = {
    // 小字一组 C4-B4 (7个音)
    262, 294, 330, 349, 392, 440, 494,
    // 小字二组 C5-B5
    523, 587, 659, 698, 784, 880, 988,
    // 小字三组 C6-B6
    1047, 1175, 1319, 1397, 1568, 1760, 1976,
    // 小字四组 C7-B7
    2093, 2349, 2637, 2794, 3136, 3520, 3951,
    // 小字五组 C8-G8 (扩展到更高音)
    4186, 4699, 5274, 5588, 6272, 7040, 7902,
    // 小字六组 C9 (最高音区)
    8372, 9397, 10548, 11175, 12544};

int bset[] = {
    // 小字一组 降音版本 (7个音)
    247, 277, 311, 330, 370, 415, 466,
    // 小字二组 降音版本
    494, 554, 622, 659, 740, 831, 932,
    // 小字三组 降音版本
    988, 1109, 1245, 1319, 1480, 1661, 1865,
    // 小字四组 降音版本
    1976, 2217, 2489, 2637, 2960, 3322, 3729,
    // 小字五组 降音版本
    3951, 4435, 4978, 5274, 5920, 6645, 7459,
    // 小字六组 降音版本
    7902, 8870, 9956, 10548, 11840};

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

void playViolin(int frequency, double duration_ms)
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

        // 极速包络 - 立即起音
        double currentTime = (double)i / sampleRate;
        double totalTime = (double)duration_ms / 1000.0;
        double envelope = 1.0;

        // 立即起音 - 第一个采样就达到最大音量
        if (currentTime > totalTime - 0.01) // 只在最后1ms衰减
        {
            // 确保在结束时完全衰减到0
            double fadeOut = (totalTime - currentTime) / 0.01;
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

void playViolinset(char put[], int bpm, int asset)
{
    double tbpm = 60000 / bpm;
    int i = 0;
    int deco = 0;
    while(put[i]!='\0'){

        int deset = asset;
        deco = 0;


        if (put[i] == 'l'){
            deset -= 7;
            i++;
        }
        if (put[i] == 'h')
        {
            deset += 7;
            i++;
        }
        if (put[i] == 'q')
        {
            tbpm/=2;
            i++;
            continue;
        }
        if (put[i] == 's')
        {
            tbpm*=2;
            i++;
            continue;
        }

        if (put[i] == 'f')
        {
            asset += 7;
            i++;
            continue;
        }
        if (put[i] == 'p')
        {
            asset -= 7;
            i++;
            continue;
        }
        double k = 1.0;
        int pk = i;
        while (put[pk + 1] == '0')
        {
            k += 1;
            pk++;
        }
        while (put[pk + 1] == '.')
        {
            k += 0.5;
            pk++;
        }
        if (put[i] >= '1' && put[i] <='9')
            playViolin(sset[put[i] - '1' + deset + 7], tbpm * k);
        i++;
    }
}

void playViolinodot(char put, int bpm, int asset, int op[], int n)
{
    double tbpm = 60000 / bpm;
    for (int i = 0; i < n; i++)
    {
        sset[op[i] + 6] = bset[op[i] + 6];
    }
    playViolin(sset[put - '1' + asset + 7], tbpm);
}

void playTonedot(char put, int bpm, int asset,int op[],int n)
{
    double tbpm = 60000 / bpm;
    for (int i = 0; i < n;i++){
        sset[op[i] + 6] = bset[op[i] + 6];
    }
        playTone(sset[put - '1' + asset + 7], tbpm);
}

void playViolindot(char put, int bpm, int asset, int op[], int n)
{
    double tbpm = 60000 / bpm;
    int set[] = {262, 294, 330, 349, 392, 440, 494, 523, 587, 659, 698, 784, 880, 988, 1047};
    int bset[] = {247, 277, 311, 330, 370, 415, 466, 494, 554, 622, 659, 740, 831, 932, 988};
    for (int i = 0; i < n; i++)
    {
        set[op[i] + 6] = bset[op[i] + 6];
    }
    playViolin(set[put - '1' + asset + 7], tbpm);
}

DWORD WINAPI soundTrackThread(LPVOID lpParam)
{


    TrackParams *params = (TrackParams *)lpParam;

    int b[] = {7};
    if (params->instrument == 0)
    {
        playViolindot(params->note, params->duration, 0, b, 1);
    }
    else if (params->instrument == 1)
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
        if (params1 == NULL)
        {
            fprintf(stderr, "Memory allocation failed for TrackParams\n");
            return; // 或者适当的错误处理
        }

        params1->note = op1;
        params1->instrument = 0; // piano
        params1->duration = 600;

        DWORD threadId;
        HANDLE hThread = CreateThread(NULL, 0, soundTrackThread, params1, 0, &threadId);
        if (hThread == NULL) {
            fprintf(stderr, "Thread creation failed, error: %lu\n", GetLastError());
            free(params1); // 清理分配的内存
        } 
        else
            CloseHandle(hThread);
    }

    // 创建音轨2线程
    if (op2 != '0')
    {
        TrackParams *params2 = malloc(sizeof(TrackParams));
        if (params2 == NULL)
        {
            fprintf(stderr, "Memory allocation failed for TrackParams\n");
            return; // 或者适当的错误处理
        }

        params2->note = op2;
        params2->instrument = 1; // tone
        params2->duration = 600;

        DWORD threadId;
        HANDLE hThread = CreateThread(NULL, 0, soundTrackThread, params2, 0, &threadId);
        if (hThread == NULL)
        {
            fprintf(stderr, "Thread creation failed, error: %lu\n", GetLastError());
            free(params2); // 清理分配的内存
        }
        else
            CloseHandle(hThread);
    }
}

DWORD WINAPI autoplayThread(LPVOID lpParam)
{
    for (int i = 0; i < 9600; i++)
    {
        playMoveSound(i);
        Sleep(300);
    }
    return 0;
}

// 非阻塞的自动播放音效函数
void autoplayMoveSound()
{
    if (!soundManager.enabled)
        return;

    // 创建新线程播放音效，不阻塞主程序
    DWORD threadId;
    HANDLE hThread = CreateThread(NULL, 0, autoplayThread, NULL, 0, &threadId);
    if (hThread)
    {
        CloseHandle(hThread); // 不等待线程结束
    }
}

DWORD WINAPI victorySoundThread(LPVOID lpParam)
{
    char a[] = "56789050";
    char b[] = "p8764500";
    playViolinset(a, 300, -4);
    playViolinset(b, 250, -4);
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

DWORD WINAPI ParagmatismThread(LPVOID lpParam)
{
    // 提高线程优先级
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    char a[] = "8000708050002051l7.1.50l7157.8.50061l7.1.501l71500270807.8.500006000";

    // 添加延迟确保其他线程完成初始化
    Sleep(50);

    playViolinset(a, 200, 0);
    char b[] = "50003.5.6201l70l701l700l600000000l60l70100l401l7l6l5020l7020400300000000000050003.5.6201l70l701l70020304300q43s200q1l7sp600406h10700f2001l7l70050060000000000";
    playViolinset(b, 400, 7);
    return 0;
}

// 胜利音效线程函数
void ParagmatismSound()
{
    if (!soundManager.enabled)
        return;

    // 创建新线程播放音效，不阻塞主程序
    DWORD threadId;
    HANDLE hThread = CreateThread(NULL, 0, ParagmatismThread, NULL, 0, &threadId);
    if (hThread)
    {
        CloseHandle(hThread); // 不等待线程结束
    }
}

