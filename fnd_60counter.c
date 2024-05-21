#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include "fnd.h"

#define MAX_ROUNDS 10


#define INITIAL_TIME 60


int currentRound = 0;
int timerValue = INITIAL_TIME;
int gameRunning = 1;


void startRound(int round);
void updateTimer();
void handleGameEnd(int signal);

int main() {

    if (!fndInit()) {
        printf("Failed to initialize FND\n");
        return 1;
    }

    signal(SIGINT, handleGameEnd); 

    for (currentRound = 1; currentRound <= MAX_ROUNDS && gameRunning; ++currentRound) {
        startRound(currentRound);
        if (!gameRunning) break;
        sleep(2); 
    }

    handleGameEnd(0);
    return 0;
}

void startRound(int round) {
    timerValue = INITIAL_TIME;
    printf("Starting ROUND %d\n", round);

    while (timerValue > 0 && gameRunning) {
        fndDisp(timerValue, 0); // Display timer value on FND
        printf("ROUND %d - Time left: %d seconds\n", round, timerValue);
        sleep(1);
        --timerValue;
    }

    if (!gameRunning) {
        printf("Game ended early during ROUND %d\n", round);
    } else {
        printf("ROUND %d completed\n", round);
    }
}

void handleGameEnd(int signal) {
    gameRunning = 0;
    currentRound = 0;
    timerValue = 0;
    fndDisp(0, 0); 
    fndExit();
    printf("Game terminated. All values set to 0.\n");
}
