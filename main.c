#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sts/stat.h>
#include <pthread.h>
#include <stdbool.h>
#include <math.h>
#include <linux/input.h>

#include "buzzer.h"
#include "buttong.h"
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


#define bulletx
#define bullety
#define enemyx
#define enemyy
const float GRAVITY = 0.5f
static int score;
static int msgID;
BUTTON_MSG_T rcv;
pthread_t thread[10];
static int stage = 0;
static int aim_x = SCREEN_WIDTH / 2; // 에임 초기 위치
static int aim_y = SCREEN_HEIGHT / 2;
static int accel_x = 0;
static int accel_y = 0;
int start=0;
int enemyNumber = 0;
int round_N = 0;
int gameover=0;
int player_record=0;
int missioncomplete=0;

typedef struct {
    float x, y;
    float vx, vy;
    bool active;
} Projectile;

Projectile projectile;



void start(){
    start=1;
    if(start==1){
        //text_Lcd =(print- start); //text_lcd start 출력
        thread_background_music();
    if(btn==1){
        round_N++;
        round();
    
    }}
    else start=0;
}

void gameover(){
     gameoevr=1;
     if(gameover==1){
       //text_Lcd =(print -gameover); // text_lcd gameover 출력
     if(text_Lcd=="gameover"){ //text_lcd에 gameover 출력되면
       //text_Lcd = (print -player_recod); // text_lcd에 player 기록 출력
         thread_game_end_music(); // gameover 음악 재생
        //text_lcd에 모든플레이어 기록 출력// ??
     if(btn==1){
        start();
     }}}
    else gameover=0;
}

void missioncomplete(){
     missioncomplete=1;
     if(missioncomplete==1){
       //text_Lcd =(print -missioncomplete); //text_lce missioncomplete 출력
    if(text_Lcd=="missioncomplete"){ //text_lcd에 missioncomplete이 출력되면
       //text_Lcd = (print -player_recod); // text_lcd에 player 기록 출력
          thread_background_music(); // 배경음악 (다른군가?) 재생
        //text_lcd에 모든플레이어 기록 출력// ??
     if(btn==1){
        start();
     }}}
    else missioncomplete=0;
}
      //게임 시작, 끝

void initProjectile(Projectile* p, float startX, float startY, float angle, float speed) {
    p->x = startX;
    p->y = startY;
    p->vx = speed * cos(angle);
    p->vy = speed * sin(angle);
    p->active = 1;
}

//발사체 초기화

void updateProjectile(Projectile* p) {
    if (p->active) {
        p->x += p->vx;
        p->y += p->vy;
        p->vy += GRAVITY;  // 중력 가속도 적용

        // 화면을 벗어나면 비활성화
        if (p->x < 0 || p->x > SCREEN_WIDTH || p->y > SCREEN_HEIGHT) {
            p->active = 0;
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
    //라운드 스타트 (N+1)
    KEY_MENU();
    round_N = round_N+;
    
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
    return accel_x,accel_y;
}



void activate_fire(){  
    
    



}


//슈도 코드
void fireProjectile(){
    if( round_N >0 ) {
        updateAimPosition();
        button_on = (fire- 1);
        if(fire==1){
            updateAimPosition();
            updateProjectile();
            active_fire();
        }
        
    }




}
/*
void fireProjectile() {
    if (!projectile.active) {
        projectile.x = SCREEN_WIDTH - 50;
        projectile.y = SCREEN_HEIGHT / 2;
        projectile.vx = (aim_x - projectile.x) / 10;
        projectile.vy = (aim_y - projectile.y) / 10;
        bulletx = projectile.vx;
        bullety = projectile.vy;
        projectile.active = true;
    }
} */


void enemyDistory() {

    system("sudo aplay ./weak.wav");
}


void enemy() {
    int enemyHp = (enemyNumber * 1);
    int hit = 0;
    if (bulletx == enemyx && bullety == enemyy) {
        hit = 1;
        enemyHp = enemyHp - hit;
        if (enemyHp = 0) {
            score++;
            enemyDistory();
        }
    }
}

//1. 적을 생성
//2. 적을 이동
//적을 먼저 직사각형으로 표현

//부터
typedef struct {
    	int x1 = 10;      // 왼쪽 상단 x 좌표
    	int y1 = 780;      // 왼쪽 하단 y 좌표
    	int width1 = 1;  // 직사각형의 너비
    	int height1 = 1; // 직사각형의 높이
	} enemy1;
	
	typedef struct {
    	int x2 = 10;      // 왼쪽 상단 x 좌표
    	int y2 = 780;      // 왼쪽 하단 y 좌표
    	int width =1;  // 직사각형의 너비
    	int height=2; // 직사각형의 높이
	} enemy2;

	typedef struct {
    	int x3 = 10;      // 왼쪽 상단 x 좌표
    	int y3 = 780;      // 왼쪽 하단 y 좌표
    	int width3 = 2;  // 직사각형의 너비
    	int height3 = 1; // 직사각형의 높이
	} enemy3;

int dx =(round_N * 3); // 라운드당 곱하기 3 
int dy =0;
//까지 전역수

void enemy appear (enemy1 a)
	{
	printf("Rect(x1: %d, y1: %d, width1: %d, height1: %d)\n", a.x, a.y, a.width, a.height);
	}

void enemy appear (enemy2 b)
	{
	printf("Rect(x2: %d, y2: %d, width2: %d, height2: %d)\n", b.x, b.y, b.width, b.height);
	}

void enemy appear (enemy3 c)
	{
	printf("Rect(x3: %d, y3: %d, width3: %d, height3: %d)\n", c.x, c.y, c.width, c.height);
	}

void enemy move(enemy1 *a, int dx, int dy)
	{
	a->x += dx;
	a->y += dy;
	}
void enemy move(enemy2 *b, int dx, int dy)
	{
	b->x += dx;
	b->y += dy;
	}
void enemy move(enemy3 *c, int dx, int dy)
	{
	c->x += dx;
	c->y += dy;
	}


void gameLoop() {
    pthread_create(&thread[0], NULL, thread_timer, NULL);
    pthread_create(&thread[1], NULL, thread_background_music, NULL);

    while (round_N>0) {
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
    Projectile projectile;
    projectile.active = 0;  // 초기에는 비활성화 상태
    initializeGame();
    gameLoop();
    return 0;
}
