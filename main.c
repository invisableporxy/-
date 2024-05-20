#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <pthread.h>
#include <stdbool.h>
#include <math.h>
#include <linux/input.h>

#include "buzzer.h"
#include "button.h"
#include "fnd.h"
#include "colorled.h"
#include "led.h"
#include "textlcd.h"
#include "libBitmap.h"
#include "bitmapFileHeader.h"
#include "gyro.h"
#include "touch.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 800
#define GRAVITY 9.8
#define MESSAGE_ID 1234
#define MODE_STATIC_DIS 0

static int msgID;
BUTTON_MSG_T rcv;
pthread_t thread[10];
static int stage = 0;
static int aim_x = SCREEN_WIDTH / 2; // 에임 초기 위치
static int aim_y = SCREEN_HEIGHT / 2;
static int accel_x = 0;
static int accel_y = 0;

typedef struct {
    float x, y;
    float vx, vy;
    bool active;
} Projectile;

Projectile projectile;

void updateProjectile(Projectile* p, float dt) {
    if (!p->active) return;

    p->x += p->vx * dt;
    p->y += p->vy * dt;
    p->vy += GRAVITY * dt;

    // 화면을 벗어나면 발사체를 비활성화합니다.
    if (p->x < 0 || p->x >= SCREEN_WIDTH || p->y >= SCREEN_HEIGHT) {
        p->active = false;
    }
}

void drawProjectile(uint32_t* screen, int screenWidth, int screenHeight, Projectile* p) {
    int px = (int)p->x;
    int py = (int)p->y;

    if (px >= 0 && px < screenWidth && py >= 0 && py < screenHeight) {
        screen[py * screenWidth + px] = 0xFF0000; // 빨간색으로 발사체 표시
    }
}

void* thread_timer(void* arg) {
    while (stage != 4) {
        for (int t = 0; t <= 999999; t++) {
            fndDisp(t, MODE_STATIC_DIS);
            sleep(1);
            if(finish==1){
                break;
            }
        }
    }
    return NULL;
}

void* thread_background_music(void* arg) {
    while (stage == 1 || stage == 0) {
        system("sudo aplay ./warrior.wav");
    }
    return NULL;
}

void* thread_game_end_music(void* arg) {
    while (stage == 4) {
        if (*(int*)arg > 40) {
            system("sudo aplay ./weak.wav");
        } else {
            system("sudo aplay ./finishsong.wav");
        }
    }
    return NULL;
}

void initializeGame() {
    msgID = msgget(MESSAGE_ID, IPC_CREAT | 0666);
    buttonInit();
    pwmLedInit();
    ledLibInit();
    lcdtextInit();
    buzzerInit();
    touchInit();
    fndDisp(000000, 0);

    if (fb_init(&screen_width, &screen_height, &bits_per_pixel, &line_length) < 0) {
        printf("FrameBuffer Init Failed\r\n");
    }

    lcdtextWrite("GAME START", "");
    read_bmp("realreal.bmp", &data, &cols, &rows);
    fb_write(data, cols, rows);

    projectile.active = false;
}

void updateAimPosition() {
    accel_x = get_accel_x();
    accel_y = get_accel_y();

    if (accel_x >= 4000) {
        aim_y += 30;
    } else if (accel_x <= -4000) {
        aim_y -= 30;
    }
    if (accel_y >= 4000) {
        aim_x += 30;
    } else if (accel_y <= -4000) {
        aim_x -= 30;
    }

    if (aim_y > SCREEN_HEIGHT) {
        aim_y = SCREEN_HEIGHT;
    } else if (aim_y < 0) {
        aim_y = 0;
    }
    if (aim_x > SCREEN_WIDTH) {
        aim_x = SCREEN_WIDTH;
    } else if (aim_x < 0) {
        aim_x = 0;
    }
}

void fireProjectile() {
    if (!projectile.active) {
        projectile.x = SCREEN_WIDTH - 50;
        projectile.y = SCREEN_HEIGHT / 2;
        projectile.vx = (aim_x - projectile.x) / 10;
        projectile.vy = (aim_y - projectile.y) / 10;
        projectile.active = true;
    }
}

void gameLoop() {
    pthread_create(&thread[0], NULL, thread_timer, NULL);
    pthread_create(&thread[1], NULL, thread_background_music, NULL);

    while (stage != 4) {
        updateAimPosition();

        if (projectile.active) {
            updateProjectile(&projectile, 0.1);
        }

        if (msgrcv(msgID, &rcv, sizeof(rcv) - sizeof(long int), 0, IPC_NOWAIT) != -1) {
            if (rcv.keyInput == 2 && rcv.pressed == 2) {
                fireProjectile();
            }
        }

        
        memset(screen, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
        drawProjectile(screen, SCREEN_WIDTH, SCREEN_HEIGHT, &projectile);

        usleep(100000);
    }

    pthread_join(thread[0], NULL);
    pthread_join(thread[1], NULL);
}

int main(void) {
    initializeGame();
    gameLoop();
    return 0;
}
