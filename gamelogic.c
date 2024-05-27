#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
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
#define SCREEN_HEIGHT 600
#define GRAVITY 0.5f
#define MESSAGE_ID 1234
#define MODE_STATIC_DIS 0

static int score;
static int msgID;
BUTTON_MSG_T rcv;
pthread_t thread[10];
static int stage = 0;
static int aim_x = SCREEN_WIDTH / 2; // 에임 초기 위치
static int aim_y = SCREEN_HEIGHT / 2;
static int accel_x = 0;
static int accel_y = 0;
int start = 0;
int enemyNumber = 0;
int round_N = 0;
int gameover = 0;
int player_record = 0;
int missioncomplete = 0;

typedef struct {
    float x, y;
    float vx, vy;
    bool active;
} Projectile;

Projectile projectile;

typedef struct {
    int x, y;
    int width, height;
    int hp;
    float speed;
    bool active;
} Enemy;

Enemy enemies[100];

void startGame() {
    start = 1;
    if (start == 1) {
        pthread_create(&thread[1], NULL, thread_background_music, NULL);
        if (rcv.keyInput == 1 && rcv.pressed == 1) {
            round_N++;
        }
    } else {
        start = 0;
    }
}

void endGame(const char* message, const char* music) {
    lcdtextWrite(message, "");
    system(music);
    sleep(10);
    startGame();
}

void gameover() {
    gameover = 1;
    endGame("GAME OVER", "sudo aplay ./gameover.wav");
}

void missioncomplete() {
    missioncomplete = 1;
    endGame("MISSION COMPLETE", "sudo aplay ./missioncomplete.wav");
}

void initProjectile(Projectile* p, float startX, float startY, float angle, float speed) {
    p->x = startX;
    p->y = startY;
    p->vx = speed * cos(angle);
    p->vy = speed * sin(angle);
    p->active = true;
}

void updateProjectile(Projectile* p) {
    if (p->active) {
        p->x += p->vx;
        p->y += p->vy;
        p->vy += GRAVITY;

        if (p->x < 0 || p->x > SCREEN_WIDTH || p->y > SCREEN_HEIGHT) {
            p->active = false;
        }
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
        for (int t = 0; t < 60; t++) {
            fndDisp(t, MODE_STATIC_DIS);
            sleep(1);
            if (gameover == 1) {
                return NULL;
            }
        }
        round_N++;
        if (round_N > 3) {
            missioncomplete();
            return NULL;
        } else {
            lcdtextWrite("NEXT ROUND", "");
            sleep(2);
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

void initializeGame() {
    msgID = msgget(MESSAGE_ID, IPC_CREAT | 0666);
    buttonInit();
    pwmLedInit();
    ledLibInit();
    lcdtextInit();
    buzzerInit();
    touchInit();
    fndDisp(000000, 0);

    int screen_width, screen_height, bits_per_pixel, line_length;
    if (fb_init(&screen_width, &screen_height, &bits_per_pixel, &line_length) < 0) {
        printf("FrameBuffer Init Failed\r\n");
    }

    lcdtextWrite("GAME START", "");
    uint32_t* data;
    int cols, rows;
    read_bmp("realreal.bmp", &data, &cols, &rows);
    fb_write(data, cols, rows);

    projectile.active = false;
    round_N = 1;
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

void fireProjectile(Projectile* p) {
    if (!p->active) {
        float startX = 100;
        float startY = 500;

        float deltaX = aim_x - startX;
        float deltaY = aim_y - startY;
        float angle = atan2(deltaY, deltaX);

        float speed = 10.0f;
        initProjectile(p, startX, startY, angle, speed);
    }
}

void enemyDestroy() {
    system("sudo aplay ./weak.wav");
}

void createEnemy(int index, int x, int y, int width, int height, int hp, float speed) {
    enemies[index].x = x;
    enemies[index].y = y;
    enemies[index].width = width;
    enemies[index].height = height;
    enemies[index].hp = hp;
    enemies[index].speed = speed;
    enemies[index].active = true;
}

void updateEnemy(int index) {
for (int i = 0; i < enemyNumber; i++) {
        if (enemy[i].active) {
            // 각 라운드마다 적의 이동 속도를 증가시킴
            enemy[i].x += (round_N * ENEMY_SPEED_INCREMENT);

            // 게임 종료 조건: 적이 포탑에 도달하거나 시간이 종료될 경우
            if (enemy[i].x >= TOWER_X_COORDINATE || timeIsUp) {
                gameover();
            }
        }
}

    if (enemies[index].active) {
        enemies[index].x += enemies[index].speed;
        if (enemies[index].x > SCREEN_WIDTH) {
            gameover();
            return;
        }

        if (projectile.active &&
            projectile.x > enemies[index].x && projectile.x < enemies[index].x + enemies[index].width &&
            projectile.y > enemies[index].y && projectile.y < enemies[index].y + enemies[index].height) {
            enemies[index].hp -= 1;
            projectile.active = false;
            if (enemies[index].hp <= 0) {
                enemies[index].active = false;
                score++;
                enemyDestroy();
            }
        }
    }
}

void drawEnemy(uint32_t* screen, int screenWidth, int screenHeight, Enemy* e) {
    if (e->active) {
        for (int i = e->y; i < e->y + e->height; i++) {
            for (int j = e->x; j < e->x + e->width; j++) {
                if (i >= 0 && i < screenHeight && j >= 0 && j < screenWidth) {
                    screen[i * screenWidth + j] = 0x00FF00;
                }
            }
        }
    }
}
/*
void gameLoop() {
    pthread_create(&thread[0], NULL, thread_timer, NULL);
    pthread_create(&thread[1], NULL, thread_background_music, NULL);

    uint32_t screen[SCREEN_WIDTH * SCREEN_HEIGHT];

    while (round_N > 0) {
        updateAimPosition();

        if (projectile.active) {
            updateProjectile(&projectile);
        }

        if (msgrcv(msgID, &rcv, sizeof(rcv) - sizeof(long int), 0, IPC_NOWAIT) != -1) {
            if (rcv.keyInput == 2 && rcv.pressed == 2) {
                fireProjectile(&projectile);
            }
        }

        memset(screen, 0, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint32_t));
        drawProjectile(screen, SCREEN_WIDTH, SCREEN_HEIGHT, &projectile);

        for (int i = 0; i < enemyNumber; i++) {
            updateEnemy(i);
            drawEnemy(screen, SCREEN_WIDTH, SCREEN_HEIGHT, &enemy[i]);
        }

        fb_write(screen, SCREEN_WIDTH, SCREEN_HEIGHT);

        usleep(16000);
    }

    pthread_join(thread[0], NULL);
    pthread_join(thread[1], NULL);
}
*/




int main(void) {
    projectile.active = false;  // 초기에는 비활성화 상태
    initializeGame();
    createEnemy(0, 500, 300, 50, 50, 3);
    enemyNumber = 1;
    gameLoop();
    return 0;
}


/*// 코드 일부분만을 다룸

#define ENEMY_SPEED_INCREMENT 1 // 라운드마다 적의 이동 속도 증가량

typedef struct {
    int x, y;
    int width, height;
    int hp;
    bool active;
} Enemy;

// 적 구조체를 유형별로 나누어 정의
typedef enum {
    ENEMY_TYPE_1,
    ENEMY_TYPE_2,
    ENEMY_TYPE_3
} EnemyType;

// 적 유형별로 HP를 할당하는 함수
int assignEnemyHP(EnemyType type) {
    switch (type) {
        case ENEMY_TYPE_1:
            return 1;
        case ENEMY_TYPE_2:
            return 2;
        case ENEMY_TYPE_3:
            return 3;
        default:
            return 1;
    }
}

// 적 생성 함수
void createEnemy(EnemyType type, int x, int y) {
    enemy[enemyNumber].x = x;
    enemy[enemyNumber].y = y;
    enemy[enemyNumber].width = 50; // 적의 너비
    enemy[enemyNumber].height = 50; // 적의 높이
    enemy[enemyNumber].hp = assignEnemyHP(type);
    enemy[enemyNumber].active = true;
    enemyNumber++;
}

// 적 업데이트 함수
void updateEnemy() {
    for (int i = 0; i < enemyNumber; i++) {
        if (enemy[i].active) {
            // 각 라운드마다 적의 이동 속도를 증가시킴
            enemy[i].x += (round_N * ENEMY_SPEED_INCREMENT);

            // 게임 종료 조건: 적이 포탑에 도달하거나 시간이 종료될 경우
            if (enemy[i].x >= TOWER_X_COORDINATE || timeIsUp) {
                gameover();
            }

            // 발사체와의 충돌 검사
            if (projectile.active &&
                projectile.x > enemy[i].x && projectile.x < enemy[i].x + enemy[i].width &&
                projectile.y > enemy[i].y && projectile.y < enemy[i].y + enemy[i].height) {
                enemy[i].hp -= 1;
                projectile.active = false;
                if (enemy[i].hp <= 0) {
                    enemy[i].active = false;
                    score++;
                    enemyDestroy();
                }
            }
        }
    }
}

// 게임 루프
void gameLoop() {
    while (round_N <= MAX_ROUNDS) {
        // 10초마다 적 리스폰
        if (timeElapsed % 10 == 0) {
            createEnemy(ENEMY_TYPE_1, INITIAL_ENEMY_X, INITIAL_ENEMY_Y);
        }

        // 에임 조절
        updateAimPosition();

        // 발사체 업데이트
        if (projectile.active) {
            updateProjectile(&projectile);
        }

        // 적 업데이트
        updateEnemy();

        // 게임 종료 및 결과 표시
        if (gameIsOver) {
            if (round_N < MAX_ROUNDS) {
                if (gameover()) {
                    displayGameOverScreen();
                    playGameOverSound();
                    sleep(10);
                    displayMainMenu();
                }
            } else {
                if (missioncomplete()) {
                    displayMissionCompleteScreen();
                    playMissionCompleteSound();
                    sleep(10);
                    displayMainMenu();
                }
            }
        }

        usleep(16000);
        timeElapsed++;
    }
}
*/