#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#define MAX_SCALE_STEP 96
#define BUZZER_BASE_SYS_PATH "/sys/bus/platform/devices/"
#define BUZZER_FILENAME "peribuzzer"
#define BUZZER_ENABLE_NAME "enable"
#define BUZZER_FREQUENCY_NAME "frequency"
char gBuzzerBaseSysDir[128];

const int musicScale[MAX_SCALE_STEP] = {
    33, // 도      0
    35, // 도#     1
    37, // 레      2
    39, // 레#     3
    41, // 미      4
    44, // 파      5
    46, // 파#     6
    49, // 솔      7
    52, // 솔#     8
    55, // 라      9
    58, // 라#     10
    62, // 시      11
    65, // 도      12
    69, // 도#     13
    73, // 레      14
    78, // 레#     15
    82, // 미      16
    87, // 파      17
    92, // 파#     18
    98, // 솔      19
    104, // 솔#    20
    110, // 라     21
    117, // 라#    22
    123, // 시     23
    131, // 도     24
    139, // 도#    25
    147, // 레     26
    156, // 레#    27
    165, // 미     28
    175, // 파     29
    185, // 파#    30
    196, // 솔     31
    208, // 솔#    32
    220, // 라     33
    233, // 라#    34
    247, // 시     35
    262, // 도     36
    277, // 도#    37
    294, // 레     38
    311, // 레#    39
    330, // 미     40
    349, // 파     41
    370, // 파#    42
    392, // 솔     43
    415, // 솔#    44
    440, // 라     45
    466, // 라#    46
    493, // 시     47
    523, // 도     48
    554, // 도#    49
    587, // 레     50
    622, // 레#    51
    659, // 미     52
    698, // 파     53
    740, // 파#    54
    784, // 솔     55
    831, // 솔#    56
    880, // 라     57
    932, // 라#    58
    988, // 시     59
    1047, // 도    60
    1109, // 도#   61
    1175, // 레    62
    1245, // 레#   63
    1319, // 미    64
    1397, // 파    65
    1480, // 파#   66
    1568, // 솔    67
    1661, // 솔#   68
    1760, // 라    69
    1865, // 라#   70
    1976, // 시    71
    2093, // 도    72
    2217, // 도#   73
    2349, // 레    74
    2489, // 레#   75
    2637, // 미    76
    2793, // 파    77
    2960, // 파#   78
    3136, // 솔    79
    3322, // 솔#   80
    3520, // 라    81
    3729, // 라#   82
    3951, // 시    83
    4186, // 도    84
    4435, // 도#   85
    4699, // 레    86
    4978, // 레#   87
    5274, // 미    88
    5588, // 파    89
    5920, // 파#   90
    6272, // 솔    91
    6645, // 솔#   92
    7040, // 라    93
    7459, // 라#   94
    7902, // 시    95
};

int findBuzzerSysPath()
{
    DIR *dir_info = opendir(BUZZER_BASE_SYS_PATH);
    int ifFound = 0;
    if (dir_info != NULL)
    {
        while (1)
        {
            struct dirent *dir_entry;
            dir_entry = readdir(dir_info);
            if (dir_entry == NULL) break;
            if (strncasecmp(BUZZER_FILENAME, dir_entry->d_name, strlen(BUZZER_FILENAME)) == 0)
            {
                ifFound = 1;
                sprintf(gBuzzerBaseSysDir, "%s%s/", BUZZER_BASE_SYS_PATH, dir_entry->d_name);
            }
        }
    }
    printf("find %s\n", gBuzzerBaseSysDir);

    return ifFound;
}

static int fdEnable;
static int fdFreq;

int buzzerInit(void)
{
    if (findBuzzerSysPath() == 0)
        return 0;

    char path[200];
    sprintf(path, "%s%s", gBuzzerBaseSysDir, BUZZER_ENABLE_NAME);
    fdEnable = open(path, O_WRONLY);

    sprintf(path, "%s%s", gBuzzerBaseSysDir, BUZZER_FREQUENCY_NAME);
    fdFreq = open(path, O_WRONLY);

    return 1;
}

void buzzerEnable(int bEnable)
{
    if (bEnable)
        write(fdEnable, &"1", 1);
    else
        write(fdEnable, &"0", 1);
}

// 1 ~ 1000 Hz
void setFrequency(int frequency)
{
    dprintf(fdFreq, "%d", frequency);
}

int buzzerExit(void)
{
    buzzerEnable(0);
    close(fdEnable);
    close(fdFreq);
    return 0;
}

void buzzerPlayTone(int scale, int duration_ms)
{
    setFrequency(musicScale[scale]);
    buzzerEnable(1);
    usleep(duration_ms * 1000);
    buzzerEnable(0);
    usleep(50000); // 50 ms pause between notes
}

int buzzerPlaySong()
{
    int notes[] = {43, 36, 38, 40, 41, 43, 45, 47, 48, // 솔 - 도 - 레 - 미 - 파 - 솔 - 라 - 시 - 도
                   48, 47, 45, 43, 41, 40, 38, 36};  // 도 - 시 - 라 - 솔 - 파 - 미 - 레 - 도
    int durations[] = {500, 500, 500, 500, 500, 500, 500, 500, 1000, // durations in ms
                       500, 500, 500, 500, 500, 500, 500, 1000};    // durations for each note

    int num_notes = sizeof(notes) / sizeof(notes[0]);
    for (int i = 0; i < num_notes; i++)
    {
        buzzerPlayTone(notes[i], durations[i]);
    }

    return 0;
}

int main()
{
    if (buzzerInit() == 0)
    {
        printf("Failed to initialize buzzer\n");
        return 1;
    }

    buzzerPlaySong();
    buzzerExit();

    return 0;
}
