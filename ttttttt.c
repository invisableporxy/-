#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <unistd.h> // for open/close
#include <fcntl.h> // for O_RDWR
#include <sys/ioctl.h> // for ioctl
#include <sys/types.h>
#include <sys/msg.h>
#include <pthread.h>
#include <stdbool.h> // 추가


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
#include <sys/kd.h>
#include <fcntl.h>

#define MESSAGE_ID 1234
#define MESSAGE_TID 1235
#define MODE_STATIC_DIS      0
static int msgID;
BUTTON_MSG_T rcv;
TOUCH_MSG_T recvMsg;
pthread_t thread[10];
static int timer_end = 0;
static int buzzeron = 0;
static int mode;
static int new_t = 0;
static int t = 3;
static int fail = 0;
static int finish = 0;
static int btn = 0;
static int btnplus = 0;
static int stage1_end = 0;
pthread_mutex_t lock;
static int step = 0;
static int running = 0;               // 참가자가 버튼 한번 누를때마다 달리는 거리에 쓰이는 변수
static char* data;
static int cols = 0, rows = 0;
static int led_on = 0;
static int stage = 0;
static int aim_x = 520;
static int aim_y = 380;
static int accel_x = 0;
static int accel_y = 0;
static int countdown = 0;
static int yrunning = 0;
static int start = 0;
static int scope_y = 350;
static int scope_x = -450;
int i = 0;
int screen_width;
int screen_height;
int bits_per_pixel;
int line_length;
static int stage2_end = 0;
static int gone1=0;
static int gone2=0;
static int gone3=0;
static int gone4=0;
static int gone5=0;
static int boo1_x = -462;
static int boo1_y = 50;
static int boo2_x = -100;
static int boo2_y = 180;
static int boo3_x = -300;
static int boo3_y = 450;
static int boo4_x = -800;
static int boo4_y = 300;
static int boo5_x = -500;
static int boo5_y = 250;
static int shot=0;


void* thread_object_1() {
    while (stage == 1 || stage == 0) {
       
        system("sudo aplay ./warrior.wav");
       
    }
}
 void* thread_object_7() {
    while (stage==3) {
        system("sudo aplay ./butter.wav");
   
    }
}


 void* thread_object_8() {
    while (stage==4&&t<=40) {
        system("sudo aplay ./finishsong.wav");
    }

}
 void* thread_object_9() {
    while (stage==4&&t>40) {
        system("sudo aplay ./weak.wav");
    }

}

void* thread_object_0() {                 // 타이머 생성

    while (stage!=4) {
        for (t = 0; t <= 999999; t++) {                //  타이머 생성
            mode = MODE_STATIC_DIS;
            fndDisp(t, mode);
            sleep(1);
            if (finish == 1) {
                break;
            }
        }
        mode = MODE_STATIC_DIS;
        fndDisp(t, mode);
            }
        }

void* thread_object_2(){

        while(stage==2){
system("sudo pkill aplay");
if(gone1 == 1){
            system("sudo aplay ./gunsound.wav");
break;
        }
  
}
}
void* thread_object_3(){

        while(stage==2){
system("sudo pkill aplay");
if(gone2 == 1){
            system("sudo aplay ./gunsound.wav");
break;
        }
  
}
}
void* thread_object_4(){

        while(stage==2){
system("sudo pkill aplay");
if(gone3 == 1){
            system("sudo aplay ./gunsound.wav");
break;
        }
  
}
}
void* thread_object_5(){

        while(stage==2){
system("sudo pkill aplay");
if(gone4 == 1){
            system("sudo aplay ./gunsound.wav");
break;
        }
  
}
}
void* thread_object_6(){

        while(stage==2){
system("sudo pkill aplay");
if(gone5 == 1){
            system("sudo aplay ./gunsound.wav");
break;
        }
  
}
}
void stage4() { //최종 화면

    system("sudo pkill aplay");

    fb_init(&screen_width, &screen_height, &bits_per_pixel, &line_length);
    while (finish == 1) {
        if (t > 40) {
pthread_create(&thread[9], NULL, thread_object_9, NULL);
            lcdtextWrite("FAIL..", "");
            pwmSetPercent(0, 0);   // 실패
            pwmSetPercent(0, 1);
            pwmSetPercent(100, 2);
            read_bmp("loser.bmp", &data, &cols, &rows);
            fb_write(data, cols, rows);      
                 sleep(10);
stage=5;
break;
        }
        else {
pthread_create(&thread[8], NULL, thread_object_8, NULL);
            lcdtextWrite("GOLD!", "");
            pwmSetPercent(0, 0);   // 금메달
            pwmSetPercent(85, 1);
            pwmSetPercent(100, 2);
            read_bmp("gold.bmp", &data, &cols, &rows);
            fb_write(data, cols, rows);
            sleep(10);
stage=5;
break;
        }
    }
}

void stage3() {
  touchExit();
          pthread_create(&thread[7], NULL, thread_object_7, NULL);
         system("sudo pkill aplay");
        static int new_x = 0;
        static int new_y = 300;
        fb_init(&screen_width, &screen_height, &bits_per_pixel, &line_length);
        ledalloff();
        lcdtextWrite("stage3", " ");
        
        int stone_x = -500;
        int stone_y = 250; 

        while (stage == 3) {
            read_bmp("skimap.bmp", &data, &cols, &rows);                // 게임 시작 초기 화면3공학관 사진출력
            fb_write(data, cols, rows);

            accel_x = get_accel_x();                                    // 가속도 센서 값 불러옴
            accel_y = get_accel_y();
            printf("\n");

            if (accel_x >= 4000) {                       // !! 사용자가 키트를 쥐고 TFT LCD를 바라보는 시점에서의
                printf("a : %d\n", accel_x);      // x축 = new_y ::: y축 = new_x
                new_y = new_y + 30;
            }
            else if (accel_x <= -4000) {
                printf("a : %d\n", accel_x);
                new_y = new_y - 30;
            }
            else if (accel_y >= 4000) {               // 사용자가 키트를 기울이는 방향대로 aim이 이동
                printf("b : %d\n", accel_y);
                new_x = new_x + 30;
            }
            else if (accel_y <= -4000) {
                printf("b : %d\n", accel_y);
                new_x = new_x - 30;
            }
            if (new_y > 400) { new_y = 400; }// 사이드 맞을시 안넘어감
            if (new_y < 100) { new_y = 100; }//사이드 맞을시 안넘어감


            if( (new_x <= -460 && new_x >= -550) && (new_y >= 230 && new_y <= 350) ){
                    new_x= 0;
                    new_y = 300;
}

            if (new_x <= -870) {
system("sudo pkill apaly");
                finish = 1;
                

    stage = 4;
                  break;
            }
            read_bmp("letgo.bmp", &data, &cols, &rows);
            fb_write_c(data, cols, rows, new_x, new_y);
            usleep(300000);
        }
    }


void stage2() {

        
          system("sudo pkill aplay");
 pthread_create(&thread[2], NULL, thread_object_2, NULL);
 pthread_create(&thread[3], NULL, thread_object_3, NULL);
 pthread_create(&thread[4], NULL, thread_object_4, NULL);
 pthread_create(&thread[5], NULL, thread_object_5, NULL);
 pthread_create(&thread[6], NULL, thread_object_6, NULL);
        fb_init(&screen_width, &screen_height, &bits_per_pixel, &line_length);
read_bmp("stage22.bmp", &data, &cols, &rows);                // 게임 시작 초기 화면3공학관 사진출력
        fb_write(data, cols, rows);

read_bmp("boo2.bmp", &data, &cols, &rows);                // 게임 시작 초기 화면3공학관 사진출력
        fb_write_c(data, cols, rows, boo2_x, boo2_y);
read_bmp("boo1.bmp", &data, &cols, &rows);                // 게임 시작 초기 화면3공학관 사진출력
        fb_write_c(data, cols, rows, boo1_x, boo1_y);

read_bmp("boo5.bmp", &data, &cols, &rows);                // 게임 시작 초기 화면3공학관 사진출력
       fb_write_c(data, cols, rows, boo5_x, boo5_y);
read_bmp("boo3.bmp", &data, &cols, &rows);                // 게임 시작 초기 화면3공학관 사진출력
       fb_write_c(data, cols, rows, boo3_x, boo3_y);
read_bmp("boo4.bmp", &data, &cols, &rows);                // 게임 시작 초기 화면3공학관 사진출력
       fb_write_c(data, cols, rows, boo4_x, boo4_y);
        ledalloff();
        int msgTID = msgget(MESSAGE_TID, IPC_CREAT | 0666);
        TOUCH_MSG_T recvMsg;

        int stage2_led = 0;
        
        lcdtextWrite("stage2", " ");
        while(1){
         msgrcv(msgTID, &recvMsg, sizeof(recvMsg) - sizeof(long int), 0, 0);
    printf("x = %d, y = %d\n", recvMsg.x, recvMsg.y);
read_bmp("stage22.bmp", &data, &cols, &rows);                // 게임 시작 초기 화면3공학관 사진출력
        fb_write(data, cols, rows);
if(gone1==0){
read_bmp("boo1.bmp", &data, &cols, &rows);                // 게임 시작 초기 화면3공학관 사진출력
        fb_write_c(data, cols, rows, boo1_x, boo1_y);
}
if(gone2==0){
read_bmp("boo2.bmp", &data, &cols, &rows);                // 게임 시작 초기 화면3공학관 사진출력
        fb_write_c(data, cols, rows, boo2_x, boo2_y);
}
if(gone3==0){
read_bmp("boo3.bmp", &data, &cols, &rows);                // 게임 시작 초기 화면3공학관 사진출력
       fb_write_c(data, cols, rows, boo3_x, boo3_y);
}
if(gone4==0){
read_bmp("boo4.bmp", &data, &cols, &rows);                // 게임 시작 초기 화면3공학관 사진출력
       fb_write_c(data, cols, rows, boo4_x, boo4_y);
}
if(gone5==0){
read_bmp("boo5.bmp", &data, &cols, &rows);                // 게임 시작 초기 화면3공학관 사진출력
       fb_write_c(data, cols, rows, boo5_x, boo5_y);
}
read_bmp("scope3.bmp", &data, &cols, &rows);
        fb_write_c(data, cols, rows, recvMsg.x-1024, recvMsg.y);

    
               
    if(recvMsg.x <=585  && recvMsg.x >=440 && recvMsg.y >=30 && recvMsg.y <=170){
gone1=1;  shot=1; 
}
      if(recvMsg.x <=930  && recvMsg.x >=810 && recvMsg.y >=160 && recvMsg.y <=280){
gone2 =1; shot=1; 
}
      if(recvMsg.x <=760  && recvMsg.x >=600 && recvMsg.y >=430 && recvMsg.y <=570){
gone3 = 1; shot=1; 
}
      if(recvMsg.x <=250  && recvMsg.x >=120 && recvMsg.y >=280 && recvMsg.y <=420){
gone4 = 1; shot=1; 
}
      if(recvMsg.x <=620  && recvMsg.x >=480 && recvMsg.y >=230 && recvMsg.y <=370){
gone5 = 1; shot=1; 
}

          ledOnOff(gone1 + gone2 + gone3 + gone4 + gone5 -1 ,1); //목표물이 하나씩 없어질 때 마다 led on

if(gone1 && gone2 && gone3 && gone4 && gone5){
          ledalloff();
        stage = 3;
break;
    }
} 

           
}


void stage1() {                 // 스레드 0 :: FND를 활용한 타이머 생성

    mode = MODE_STATIC_DIS;
    fndDisp(3, mode);
    lcdtextWrite("WAIT", "3!");
    pwmSetPercent(0, 0);   // 3초일때 LED :: Red
    pwmSetPercent(0, 1);
    pwmSetPercent(100, 2);
    system("sudo ./buzzertest 1");
    sleep(1);
    system("sudo ./buzzertest 0");
    sleep(1);

    fndDisp(2, mode);
    lcdtextWrite("WAIT", "2!");
    pwmSetPercent(0, 0);   // 2초일때 LED :: Yellow
    pwmSetPercent(100, 1);
    pwmSetPercent(100, 2);
    system("sudo ./buzzertest 1");
    sleep(1);
    system("sudo ./buzzertest 0");
    sleep(1);

    fndDisp(1, mode);
    lcdtextWrite("WAIT", "1!");
    pwmSetPercent(0, 0);   // 1초일때 LED :: Green
    pwmSetPercent(100, 1);
    pwmSetPercent(0, 2);
    system("sudo ./buzzertest 1");
    sleep(1);
    system("sudo ./buzzertest 0");
    sleep(1);
    system("sudo ./buzzertest 8");
    lcdtextWrite("LET's GO!!", "");
    pwmSetPercent(100, 0);   // 시작일때 LED :: Blue
    pwmSetPercent(0, 1);
    pwmSetPercent(0, 2);
    sleep(1);
    system("sudo ./buzzertest 0");
    lcdtextWrite("STAGE1", "");
    pthread_create(&thread[0], NULL, thread_object_0, NULL);
   
   
    

while (1) {        
msgrcv(msgID, &rcv.keyInput, sizeof(rcv.keyInput), 0, 0);
        msgrcv(msgID, &rcv.pressed, sizeof(rcv.pressed), 0, 0);

        if (rcv.keyInput == 2 && rcv.pressed == 2) {
            //  lcdtextWrite("Stage 1", "BEGINING");

            btn++;
            running = running - 10;

        if (btn >= 10 && btn < 22) {
            ledOnOff(0, 1);
        }
        else if (btn >= 23 && btn < 35) {
            ledOnOff(1, 1);
        }
        else if (btn >= 36 && btn < 48) {
            ledOnOff(2, 1);
        }
        else if (btn >= 49 && btn < 61) {
            ledOnOff(3, 1);
        }
        else if (btn >= 62 && btn < 74) {
            ledOnOff(4, 1);
        }
        else if (btn >= 75 && btn < 87) {
            ledOnOff(5, 1);
        }
        else if (btn >= 88 && btn <92) {
            ledOnOff(6, 1);
        }
        else if (btn > 93) {
            ledOnOff(7, 1);
 stage = 2;
break;
        }

            read_bmp("stage3.bmp", &data, &cols, &rows);                // 게임 시작 초기 화면3공학관 사진출력
            fb_write(data, cols, rows);
            read_bmp("letgo.bmp", &data, &cols, &rows);          // 참가자가 버튼 눌러 이동할때마다 새로 업데이트된 좌표로 참가자.bmp 출력 (TFT LCD)
            fb_write_c(data, cols, rows, running, yrunning);

            
        }
    }
}
       
void stage0() {

         pthread_create(&thread[1], NULL, thread_object_1, NULL);
            while (stage1_end != 1) {
                msgrcv(msgID, &rcv.keyInput, sizeof(rcv.keyInput), 0, 0);
                msgrcv(msgID, &rcv.pressed, sizeof(rcv.pressed), 0, 0);

                if (rcv.keyInput == 6 && rcv.pressed == 2) {
                    read_bmp("stage3.bmp", &data, &cols, &rows);                // 게임 시작 초기 화면3공학관 사진출력
                    fb_write(data, cols, rows);
                    stage = 1;
                       break;
                }



            }
        }
    



    int main(void) {
        
system("sudo amixer sset 'Speaker' 80%");
         msgID = msgget(MESSAGE_ID, IPC_CREAT | 0666);        // 버튼 입력 위해 메세지큐 생성
        stage = 0;
        buttonInit();
        pwmLedInit();
        ledLibInit();
        lcdtextInit();
        ledalloff();
        buzzerInit();
                          touchInit();                                                 // 각 기능들 활용을 위한 초기 설정
        fndDisp(000000, 0);                                                              // 게임 시작 시 FND 0으로 초기화

        if (fb_init(&screen_width, &screen_height, &bits_per_pixel, &line_length) < 0)
        {
            printf("FrameBuffer Init Failed\r\n");                                           // TFT LCD 초기화
        }

        lcdtextWrite("GAME START", "");

        read_bmp("realreal.bmp", &data, &cols, &rows);                // 게임 시작 초기 화면3공학관 사진출력
        fb_write(data, cols, rows);


    stage0();
    if (stage == 1) { stage1(); }
    if (stage == 2) { stage2(); }
    if (stage == 3) { stage3(); }
    if (stage == 4) { stage4(); }
  if (stage == 5) { exit(0); }

int returnValue = 0;
        while (1) {
            
            returnValue = msgrcv(msgID, &rcv, sizeof(rcv) - sizeof(long int), 0, IPC_NOWAIT);           // 메세지큐 초기화
            if (returnValue == -1) break;
        }

 pthread_join(thread[0], NULL);
 pthread_join(thread[1], NULL);
 pthread_join(thread[2], NULL);

 pthread_join(thread[3], NULL);
 pthread_join(thread[4], NULL);
 pthread_join(thread[5], NULL);
 pthread_join(thread[6], NULL);
 pthread_join(thread[7], NULL);
 pthread_join(thread[8], NULL);
 pthread_join(thread[9], NULL);


        //------------------------//

         
       
    }
