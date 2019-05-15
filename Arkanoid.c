/* game.c - xmain, prntr */
/*
* This is a Arkanoid  Game that programed with a OS XINU we controlled in interrupt 70H and using port 70 and 42 and,
* interrupt 16H to the control of the sound, and we using at ports h4D3 and port h5D3 that control on  video card "found" in the segment B800h.
*
* @author Marshood Ayoub , Firas Gadban , Ameer Amer
* @version 1.0 Nov 2018
*/


#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <bios.h>
#include <stdlib.h>
#include <graphics.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <proc.h>
#include <dos.h>

// ----------------------- DEFINES ----------------------- //
#define RESET_PAD 1
#define RESET_GAME 2 
#define DISPLAY_HEIGHT 25
#define DISPLAY_WIDTH 80
#define STAGE_1 1 
#define STAGE_2 2
#define STAGE_3 3

#define ON (1)
#define OFF (0)
// ----------------------- STRUCTS ----------------------- //
typedef struct GameBlock{
  int HitCounter;
  char  ch;
  int color;
  int firstPos;
  
};


struct GameBlock  display_draft[DISPLAY_HEIGHT][DISPLAY_WIDTH];
struct  GameBlock  GameBarriers[DISPLAY_HEIGHT][DISPLAY_WIDTH];
struct GameBlock   GameBall[DISPLAY_HEIGHT-6][DISPLAY_WIDTH-2];
// ----------------------- DECLARATIONS ----------------------- //
extern struct intmap far *sys_imp;
unsigned char far *b800h ;
void  Int70hHandler(int mdevno);
void set_Int70hHandler();
extern int tod;
int  intmap(int vec,int (*Handler)(), int mdevno);
 volatile int global_delay = 0, global_delay_curr;
int uppid, dispid, recvpid;
int receiver_pid;
int gcycle_length;
int point_in_cycle;
int gameOn = 1 ;
int gno_of_pids;
int stage = STAGE_1;
int purple_flag=0;
int sched_arr_pid[5] = {-1};
int sched_arr_int[5] = {-1};
int targetCount=0;
int player_live=3;
int startPos;
int front = -1;
int rear = -1;
int timeLeftBonus=0;
char ch_arr[1024];
int firstPos=35;
int lastPos=45;
int pad_width =0;
int ballPosX=40;
int ballPosY=22;
int ball_dir=1;
int nextColor=0;
int alpha=0;
char ch=0;
int counter_purple =0;
char ch1=0;
int counter=0;
int counter2 =0 ;
int Colors[8]={15,32,19,80,78,110,64,8};// 19 = blue, 8 = grey , 110 = yellow , 32 = green ,64 = red
int score=0;
int h=5;
 volatile int global_flag=0;
volatile  int global_flag1 =0;
int first_stage[24]={-1,-1,-1,1,-1,-1,-1,-1,-1,2,-1,-1,-1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1,-1};
int second_stage[24]={1,-1,1,-1,1,-1,-1,1,-1,1,-1,1,1,-1,1,-1,1,-1,-1,-1,-1,-1,-1,-1};
int third_stage[24]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
//int third_stage[24]={-1,-1,-1,1,-1,-1,-1,-1,-1,1,-1,-1,-1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1,-1};
int targetGotHit[20][80];// if the ball hit a block
int  ColorTemp1=2,ColorTemp2=2,ColorTemp3=2;
void ball_next_x();
void reset();
int hour, min, sec,desSec=0 ,timeleft=60; // to read the time 
void DesTime();  // draw the time 
void readclk(); // read 
void draw_game_ball();
void handle_controls();
void draw_blocks();
void display_score();
void check_target();
void draw_game_barriers();
void draw_score_barriers();
void draw_game_pad();
void init_game();
void NoSound();
void sound(int hertz);


void  Int70hHandler(int mdevno)
{
  int i;
 asm {
   PUSH AX
   PUSH BX
   IN AL,70h   // Read existing port 70h
   MOV BX,AX

   MOV AL,0Ch  // Set up "Read status register C"
   OUT 70h,AL  //
   MOV AL,8Ch  //
   OUT 70h,AL  //
   IN AL,71h
   MOV AX,BX   //  Restore port 70h
   OUT 70h,AL  // 

   MOV AL,20h   // Set up "EOI" value  
   OUT 0A0h,AL  // Notify Secondary PIC
   OUT 020h,AL  // Notify Primary PIC

   POP BX
   POP AX

  } // asm */
  global_delay++;
}
/* this function reset the screen after moving to the next level and draw the blocks for the required level*/
void init_game()
{
  if(counter==0)//to make sure it's only work once
  {
  int i,j;

  if(targetCount == 0 || purple_flag == 1 &&targetCount == counter_purple)//move to the next level only if there 0 blocks or the number of purple blocks
  {
    if(stage == STAGE_1)
  timeleft = 60;
  else if (stage == STAGE_2)
  timeleft = 180;
  else if (stage == STAGE_3)
  timeleft = 240;
  }
 for(i=0;i<DISPLAY_HEIGHT;i++)
  {
    for(j=0;j<DISPLAY_WIDTH;j++)
    {

display_draft[i][j].ch=-1;
display_draft[i][j].color=128;
display_draft[i][j].HitCounter=0;
      display_draft[1][20].ch ='L';
      display_draft[1][20].color=13;
      display_draft[1][21].ch ='e';
      display_draft[1][21].color=13;
      display_draft[1][22].ch ='v';
      display_draft[1][22].color=13;
      display_draft[1][23].ch ='e';
      display_draft[1][23].color=13;
      display_draft[1][24].ch ='l';
      display_draft[1][24].color=13;
      display_draft[1][25].ch =':';
      display_draft[1][25].color=13;
      display_draft[1][26].ch =stage+'0';
      display_draft[1][26].color=13;
GameBarriers[i][j].ch=-1;
GameBarriers[i][j].color=128;
GameBarriers[i][j].HitCounter = 0;
    }
  }
  draw_score_barriers();
  draw_game_barriers();
  if(stage ==STAGE_1)
  draw_blocks(stage,first_stage);
  else if (stage == STAGE_2)
  draw_blocks(STAGE_2,second_stage);
  else if (stage == STAGE_3)
  draw_blocks(STAGE_3,third_stage);
  readclk();
  display_draft[1][35].ch ='B';
      display_draft[1][35].color=13;
      display_draft[1][36].ch ='l';
      display_draft[1][36].color=13;
      display_draft[1][37].ch ='o';
      display_draft[1][37].color=13;
      display_draft[1][38].ch ='c';
      display_draft[1][38].color=13;
      display_draft[1][39].ch ='k';
      display_draft[1][39].color=13;
      display_draft[1][40].ch ='s';
      display_draft[1][40].color=13;
      display_draft[1][41].ch =' ';
      display_draft[1][41].color=13;
      display_draft[1][42].ch ='L';
      display_draft[1][42].color=13;
      display_draft[1][43].ch ='e';
      display_draft[1][43].color=13;
      display_draft[1][44].ch ='f';
      display_draft[1][44].color=13;
      display_draft[1][45].ch ='t';
      display_draft[1][45].color=13;
      display_draft[1][46].ch =':';
      display_draft[1][46].color=13;
      if(targetCount>=10)
      {
      display_draft[1][47].ch=targetCount/10+'0';
      display_draft[1][47].color=13;
      display_draft[1][48].ch=targetCount%10+'0';
      display_draft[1][48].color=13;
      }
      else
      {
      display_draft[1][47].ch=targetCount+'0';
      display_draft[1][47].color=13;
      }
      display_draft[1][56].ch = 'P';
      display_draft[1][56].color=13;
      display_draft[1][57].ch = 'a';
      display_draft[1][57].color=13;
      display_draft[1][58].ch = 'd';
      display_draft[1][58].color=13;
      display_draft[1][59].ch = 's';
      display_draft[1][59].color=13;
      display_draft[1][60].ch = ' ';
      display_draft[1][60].color=13;
      display_draft[1][61].ch = 'L';
      display_draft[1][61].color=13;
      display_draft[1][62].ch = 'e';
      display_draft[1][62].color=13;
      display_draft[1][63].ch = 'f';
      display_draft[1][63].color=13;
      display_draft[1][64].ch = 't';
      display_draft[1][64].color=13;
      display_draft[1][65].ch = ':';
      display_draft[1][65].color=13;
  draw_game_pad();
  draw_game_ball();
  counter++;
  }
  display_draft[3][10].ch ='T';
      display_draft[3][10].color=13;
      display_draft[3][11].ch ='i';
      display_draft[3][11].color=13;
      display_draft[3][12].ch ='m';
      display_draft[3][12].color=13;
      display_draft[3][13].ch ='e';
      display_draft[3][13].color=13;
      display_draft[3][14].ch =' ';
      display_draft[3][14].color=13;
     display_draft[3][15].ch ='N';
      display_draft[3][15].color=13;
      display_draft[3][16].ch ='o';
      display_draft[3][16].color=13;
      display_draft[3][17].ch ='w';
      display_draft[3][17].color=13;
      display_draft[3][18].ch =':';
      display_draft[3][18].color =13;

      display_draft[3][35].ch = 'S';
      display_draft[3][35].color=13;
      display_draft[3][36].ch = 'c';
      display_draft[3][36].color=13;
      display_draft[3][37].ch = 'o';
      display_draft[3][37].color=13;
      display_draft[3][38].ch = 'r';
      display_draft[3][38].color=13;
      display_draft[3][39].ch = 'e';
      display_draft[3][39].color=13;
      display_draft[3][40].ch = ':';
      display_draft[3][40].color=13;


      display_draft[3][56].ch = 'T';
      display_draft[3][56].color=13;
      display_draft[3][57].ch = 'i';
      display_draft[3][57].color=13;
      display_draft[3][58].ch = 'm';
      display_draft[3][58].color=13;
      display_draft[3][59].ch = 'e';
      display_draft[3][59].color=13;
      display_draft[3][60].ch = ' ';
      display_draft[3][60].color=13;
      display_draft[3][61].ch = 'L';
      display_draft[3][61].color=13;
      display_draft[3][62].ch = 'e';
      display_draft[3][62].color=13;
      display_draft[3][63].ch = 'f';
      display_draft[3][63].color=13;
      display_draft[3][64].ch = 't';
      display_draft[3][64].color=13;
      display_draft[3][65].ch = ':';
      display_draft[3][65].color=13;
}
void set_Int70hHandler()
{
  int i;
  struct intmap far *imp;
  asm {
   CLI         // Disable interrupts
   PUSH AX     // Interrupt may occur while updating

   IN AL,0A1h  // Make sure IRQ8 is not masked
   AND AL,0FEh // Set bit 0 of port 0A1 to zero
   OUT 0A1h,AL //

   IN AL,70h   // Set up "Write into status register A"
   MOV AL,0Ah  //
   OUT 70h,AL  //
   MOV AL,8Ah  //
   OUT 70h,AL  //
   IN AL,71h   //
   AND AL,10000000b // Change only Rate
   OR AL,00100110b // Make sure it is Rate =0110 (1Khz)
   OUT 71h,AL  // Write into status register A
   IN AL,71h   // Read to confirm write
   IN AL,70h  // Set up "Write into status register B"
   MOV AL,0Bh //
   OUT 70h,AL //
   MOV AL,8Bh //
   OUT 70h,AL //
   IN AL,71h  //
   OR AL,40h  // Enable periodic interrupts (PI=1) only
   OUT 71h,AL // Write into status register  B
   IN AL,71h  // Read to confirm write
   IN AL,021h  // Make sure IRQ2 is not masked
   AND AL,0FBh // Write 0 to bit 2 of port 21h
   OUT 021h,AL // Write to port 21h
   IN AL,70h  // Set up "Read into status resister C"
   MOV AL,0Ch // Required for for "Write into port 71h"
   OUT 70h,AL
   IN AL,70h
   MOV AL,8Ch // 
   OUT 70h,AL
   IN AL,71h  // Read status register C 
              // (we do nothing with it)

   IN AL,70h  // Set up "Read into status resister C"
   MOV AL,0Dh // Required for for "Write into port 71h"
   OUT 70h,AL
   IN AL,70h
   MOV AL,8Dh
   OUT 70h,AL
   IN AL,71h  // Read status register D 
              // (we do nothing with it)


   STI
   POP AX

  } // asm
mapinit(0x70,Int70hHandler,0x70);
}
void display_char(int y,int x,struct GameBlock matrix[][DISPLAY_WIDTH])
{
b800h[2*(80*y+x)]=matrix[y][x].ch;
b800h[2*(80*y+x)+1]=matrix[y][x].color;
}


void hit_sound()
{
	sound(4100);   // this func is to take out the sound (hit soud ) 
	delay(15);
	sound(200);
	delay(100);
NoSound();
}
void dying_sound() // this func is to take out the sound(dying  soud)
{
	sound(4100);
	delay(100);
	sound(4200);
	delay(100);
	sound(4300);
	delay(100);
	sound(4200);
	delay(100);
	sound(4100);
	delay(100);
	sound(4400);
	delay(100);
	sound(4300);
	delay(100);
	sound(4200);
	delay(100);
	sound(4100);
	delay(100);
	sound(4300);
	delay(100);
	sound(4200);
	delay(100);
	sound(4100);
	delay(100);
	sound(200);
	delay(100);

NoSound();


}

void mydelay(int n)
{
	asm{
	CLI
	PUSH AX
	MOV AL, 036h
	OUT 43h, AL  //port 43 
	MOV AX, 9700
	OUT 40h, AL  // port 40 
	MOV AL, AH
	OUT 40h, AL
	POP AX
} // asm
  
	global_delay_curr = global_delay;
	while (global_delay - global_delay_curr <= n * 110)
		;

	asm{
		CLI
		PUSH AX
		MOV AL, 036h
		OUT 43h, AL
		MOV AX, 0
		OUT 40h, AL
		MOV AL, AH
		OUT 40h, AL
		POP AX
	} // asm


 

} //mydelay



/*------------------------------------------------
ChangeSpeaker - Turn speaker on or off. */

void ChangeSpeaker(int status)  //To make sounds on the speaker  PC you must actually turn ONand OFF anelectronic "gate" (which is a kind of toggle switch).
{
	int portval;
	//   portval = inportb( 0x61 );

	portval = 0;
	asm{
		PUSH AX
		MOV AL, 61h  //turns our "gate" ON and OFF is: OUT 61H, AL
		MOV byte ptr portval, AL
		POP AX
	}

	if (status == ON)
		portval |= 0x03;
	else
		portval &= ~0x03;
	// outportb( 0x61, portval );
	asm{
		PUSH AX
		MOV AX, portval  // ax have a  value, latc
		OUT 61h, AL  //Copy it to port 61H of the PPI chip
		POP AX    // restore AX
	} // asm

} /*--ChangeSpeaker( )----------*/

void sound(int hertz)
{
  unsigned divisor;
	 divisor = 1193180L / hertz;
	ChangeSpeaker(ON);

	//        outportb( 0x43, 0xB6 );
	// TIMER COMMAND: SELECT CHANNEL 2;
	//                READ / WRITE LSB-MSB 
	//                MODE 3: BINARY
	asm{
		PUSH AX
		MOV AL, 0B6h
		OUT 43h, AL
		POP AX
	} // asm


	//       outportb( 0x42, divisor & 0xFF ) ;
	asm{
		PUSH AX
		MOV AX, divisor
		AND AX, 0FFh    //channel 2.
		OUT 42h, AL
		POP AX
	} // asm


	//        outportb( 0x42, divisor >> 8 ) ;

	asm{
		PUSH AX
		MOV AX, divisor
		MOV AL, AH  // write the divisor 

		OUT 42h, AL
		POP AX
	} // asm

} /*--Sound( )-----*/

void NoSound(void)  //   No sound
{
	ChangeSpeaker(OFF);
} /*--NoSound( )------*/

int  convert_to_binary(int x)
{
 int i;
 int temp, scale, result;
 

  temp =0;
  scale = 1;
  for(i=0; i < 4; i++)
   {
     temp = temp + (x % 2)*scale;
     scale *= 2;
     x = x >> 1;
   } // for

  result = temp;
  temp = 0;

  scale = 1;
  for(i=0; i < 4; i++)
   {
     temp = temp + (x % 2)*scale;
     scale *= 2;
     x = x >> 1;
   } // for

  temp *= 10;
  result = temp + result;
  return result;

} // convert_to_binary

void readclk()  // this func to get the real time frome port 70h onley read 
{
  int i;
 

  hour = min = sec = 0;

  asm {
   PUSH AX
   MOV AL,0
   OUT 70h,AL // to get the sec
   IN AL,71h
   MOV BYTE PTR sec,AL
;
   MOV AL,2
   OUT 70h,AL  // to get the min 
   IN AL,71h
   MOV BYTE PTR min,AL
;
   MOV AL,4
   OUT 70h,AL  // to get the hour 
   IN AL,71h
   MOV BYTE PTR hour,AL
;
   POP AX
  } // asm

  sec = convert_to_binary(sec);
  min = convert_to_binary(min);
  hour = convert_to_binary(hour);
  if(desSec==0){desSec=sec;}
 else if (sec!=desSec){  // this desSec to get the time left for the game 
  timeleft--;
  desSec=sec;
}
} // readclk
void draw_score_barriers()    // this func to draw the barriers  
{
  int i,j;   // we draw  with chat 201 and color 13 frome position 
  //HORIZINTAL BARRIERS
  GameBarriers[0][0].ch=201;
  GameBarriers[0][0].color=13;
  display_char(0,0,GameBarriers);
  GameBarriers[0][DISPLAY_WIDTH-1].ch=187;
  GameBarriers[0][DISPLAY_WIDTH-1].color=13;
  display_char(0,DISPLAY_WIDTH-1,GameBarriers);
  GameBarriers[5][0].ch=200;
  GameBarriers[5][0].color=13;
  display_char(5,0,GameBarriers);
  GameBarriers[5][79].ch=188;
  GameBarriers[5][79].color=13;
  display_char(5,DISPLAY_WIDTH-1,GameBarriers);
//VERTICAL BARRIERS
  for(i=1;i<=4;i++)
  {
    GameBarriers[i][0].ch = 186;
    GameBarriers[i][0].color = 13;
    display_char(i,0,GameBarriers);
    GameBarriers[i][79].ch = 186;
    GameBarriers[i][79].color = 13;
    display_char(i,79,GameBarriers);
  }
  for(i=1;i<79;i++)
  {
    GameBarriers[0][i].ch=205;
    GameBarriers[0][i].color=13;
    display_char(0,i,GameBarriers);
    GameBarriers[5][i].ch=205;
    GameBarriers[5][i].color=13;
    display_char(5,i,GameBarriers);
  }
   
}
void draw_blocks(int stage,int block[24])
{
  int x=5,y=7;
  int i,j,k,m=0;
  int color=0;
  int z=0,p=0;
  int firstPos = 0;
    for(k=7;k<=13;k=k+2)  // row =7   draw from row number 7 
    {
      x=5;
      for(j=0;j<6;j++)   // to get 6 columns of blocks and draw them
      {
              color = rand()%8;  // to get the random color frome the array colors we have 8 colors
              //color =3;
          firstPos=x;
        if(block[m] == 1)
        {
               if(Colors[color] ==80)  // we have atarget counter if the player  erase all blocks thic counter be 0 and the  up the stage but the 
               {                // purple color the player can't delete them  this counet we use to minus thetarget counter 
                 counter_purple++;
              purple_flag=1;
               }
          for(i=x;i<x+10;i++) // draw the blocks with the char 177 and the random color 
          {
             display_draft[k][i].ch =177;
            display_draft[k][i].color = Colors[color];
            display_draft[k][i].firstPos= firstPos;  // first position is the first position in the block this will be deferent  with other blocks 
            display_draft[k][i].HitCounter = 1;  // the hit counter is the ball hit with the block if this be 0 then the block well be hide  define 1 
          if(Colors[color]!= 80 )  // if the color is not purple 
          {
            if(ColorTemp2>0)  // to get the random hits to all the blocks
            {
            display_draft[k][i].HitCounter = 2;
          
            ColorTemp2--;
             }
             if(ColorTemp3>0&&stage !=STAGE_1)
             {
            display_draft[k][i].HitCounter = 3;
            ColorTemp3--;
             }
          }
          else
          display_draft[k][i].HitCounter = 10000; // if the color ic purple we cant delete the so we have a hight number 
          }
          if(Colors[color]!=80) // the to count the target count the number of the block in the screen 
          targetCount++;
        }
        else  if (block[m] == 2)   // this to draw the bonus block
        {
        for(i=x;i<x+10;i++)
          {
             display_draft[k][i].ch =179;
            display_draft[k][i].color = Colors[color];
            display_draft[k][i].firstPos= firstPos;
            display_draft[k][i].HitCounter=0;
          
        }
        targetCount++;
      }
      else if  (block[m] == -1)// dont draw the blocks
      {
         display_draft[i][j].HitCounter = 100;
         display_draft[i][j].ch = -1 ;
      }
          m++;
        x=x+11;
    }
}
}
/* barriers for the game screen*/
void draw_game_barriers()
{
  int i,j;
    //HORIZINTAL BARRIERS
   GameBarriers[24][0].ch=200;
   GameBarriers[24][0].color=13;
   display_char(24,0,GameBarriers);
  GameBarriers[24][79].ch=188;
  GameBarriers[24][79].color=13;
   display_char(24,79,GameBarriers);
    //VERTICAL BARRIERS
   for(i=3;i<DISPLAY_HEIGHT-1;i++)
   {
      GameBarriers[i][0].ch=186;
      GameBarriers[i][0].color=13;
      display_char(i,0,GameBarriers);
      GameBarriers[i][79].ch=186;
      GameBarriers[i][79].color=13;
      display_char(i,79,GameBarriers); 
   }
    for(i=1;i<DISPLAY_WIDTH-1;i++)
         {
           GameBarriers[24][i].ch=205;
           GameBarriers[24][i].color=13;
           display_char(24,i,GameBarriers);
         }
}
void draw_game_pad( )  // this func to draw game pad 
{
int i,temp,number=0;
if(gameOn == 0) // if the game is on 
{
if(ch1=='r')   // if go rigth we delete the first char in the pad  
{
display_draft[23][firstPos-1].ch=' ';
display_draft[23][firstPos-1].color=13;
}
if(ch1=='l')  // if go lift we delete the last pad char 
{
  
  display_draft[23][lastPos+1].ch=' ';
display_draft[23][lastPos+1].color=13;
}
pad_width=lastPos-firstPos;   // this is the pad size 
for(i=firstPos;i<firstPos+pad_width;i++)  // draw the new pad  
{
  display_draft[23][i].ch=205;
  display_draft[23][i].color = 13;
}
}
}
void get_bonus()  // this func to get a bouns when we hide the bouns block 
{
  int number ;

      number =rand()%3;  // random number to get a random bouns 

  if(number == 0)
  { 
pad_width +=10;  // we get   pad_width +10
lastPos+=10;
  }
  else if (number == 1)
  {
pad_width -=5;   //    small the   pad_width 
lastPos-=5;
  }
  else if (number == 2 )  // add to time left for the game 30 sec 
  {
timeleft+=30;
  }
}
void check_target()
{
    int i,j,k,p;
    int temp;
    int number;
    int color;
   for(i=7;i<=13;i=i+2)//check the targets in the three rows 7 9 13
    {
     for(j=5;j<DISPLAY_WIDTH;j++)
     {
       if(display_draft[i][j].ch==-103||display_draft[i][j-1].ch==-103)
       {
         if(ballPosX>=display_draft[i][j].firstPos&&ballPosX<=display_draft[i][j].firstPos+pad_width
         &&display_draft[i][j].firstPos!=-1&&display_draft[i][j].HitCounter==0&&i==ballPosY&&display_draft[i][j].color!=128)
     {
         hit_sound();
         temp = display_draft[i][j].firstPos;

  if(display_draft[i][j-1].ch == -79||display_draft[i][j].ch==-79)
  {
         if(nextColor==15)// 15 = white 
         score += 60;
         else if (nextColor == 32) // 32 = green
        score +=80;
        else if (nextColor == 19) // 19 == blue 
        score+= 70;
        else if (nextColor == 78) // 78 = orange 
        score+=60;
        else if (nextColor == 110)// 110 = yellow 
        score+=120;
        else if (nextColor == 64) // 64 = red
        score+=90;
        else if (nextColor == 8) // 8 = grey
        score+=50;
  }
  else if(display_draft[i][j].ch ==-77 )
  { 
     score +=0;
     get_bonus();
  }
//printf("%d",display_draft[i][j].ch);
         for(k=temp;k<temp+10;k++)
         {
             display_draft[i][k].ch =-1;
             display_draft[i][k].color = 128;
         //    printf("%d ",display_draft[i][k].firstPos);
               display_draft[i][k].firstPos=-1;  
               
         } 
		//}	
         if( ball_dir==0){ball_dir=1;ballPosY--;ball_next_x();}		
        if( ball_dir==1){ball_dir=0;}	
         targetCount--;
         if(targetCount>=10)
         {
          
         display_draft[1][47].ch=targetCount/10+'0';
         display_draft[1][48].ch=targetCount%10+'0';
         }
         else 
         {
            display_draft[1][47].ch=targetCount+'0';
         }  
         if(targetCount==0||targetCount==counter_purple&&purple_flag ==1)
         {
           counter2 =0;
           gameOn=1;
       if(stage==STAGE_1)
       {
       stage = STAGE_2;
       timeleft =180;
       }
       else if (stage == STAGE_2) 
       {
         timeleft = 240;
       stage =STAGE_3;
       }
       counter=0;
       //init_game();
       reset();
       
         }
     }
          else if(ballPosX>=display_draft[i][j].firstPos&&ballPosX<=display_draft[i][j].firstPos+pad_width&&display_draft[i][j].firstPos!=-1&&display_draft[i][j].HitCounter>0 )
     {
        display_draft[i][j].HitCounter--;
        hit_sound();
        if(display_draft[i][j].color==80)
        purple_flag =1 ;
       // printf("%d", display_draft[i][j].HitCounter);
        ball_dir=0;
        
     
     }
       }
     }
    }
}
void draw_game_ball()//function to draw the ball
{
int i;
if(gameOn==0)//game is on
{
  if(ballPosY!=23) //if the ball is not in the 23 row of the screen whiche where the pad is 
  {
  if(ballPosY>h && ball_dir ==1 )//ball_dir ==1 the ball is going up
  {
    display_draft[ballPosY][ballPosX].ch =' ';
    display_draft[ballPosY][ballPosX].color =128;//deleting the ball
    ballPosY--;//going up in the rows
	ball_next_x();//function to calculate the next x 
    nextColor = display_draft[ballPosY][ballPosX+2].color;
    display_draft[ballPosY][ballPosX].ch = 153;
    display_draft[ballPosY][ballPosX].color =13;// displaying the ball in the upper row and the next x
	//check_target();//function to check if the ball hitted a target
  }
  else if (ballPosY == h && ball_dir == 1)//if the ball hits the upper border go down
  {

       display_draft[ballPosY][ballPosX].ch=' ';
       display_draft[ballPosY][ballPosX].color=128;
	     ballPosY++;//going one row down
	    ball_next_x();
		ball_dir=0;// the ball direction is down
       display_draft[ballPosY][ballPosX].ch=153;
       display_draft[ballPosY][ballPosX].color = 13;
      // 	check_target();
	    
  }
  else if (ballPosY < 23&&ball_dir == 0)//the ball is going down
  {
       if(ballPosY!=22)// if the ball isn't in the 22 row
       {
		 
       display_draft[ballPosY][ballPosX].ch=' ';
       display_draft[ballPosY][ballPosX].color=128;
       ballPosY++;
	    ball_next_x();
       display_draft[ballPosY][ballPosX].ch=153;
       display_draft[ballPosY][ballPosX].color = 13;
       }
       if(ballPosY==22)
       {
        display_draft[ballPosY][ballPosX].ch=' ';
        display_draft[ballPosY][ballPosX].color=128;
                ballPosY++;
				 ball_next_x();
        display_draft[ballPosY][ballPosX].ch=153;
       display_draft[ballPosY][ballPosX].color = 13;
       }

  }
  }
  else if (ballPosY==23)
  {

    if(ballPosX>=firstPos&&ballPosX<=lastPos)
    {
    ball_dir = 1;//the ball got caught with the pad and now the direction is up
	display_draft[ballPosY][ballPosX].ch=205;
    display_draft[ballPosY][ballPosX].color=13;
    ballPosY--;
	
          if(alpha == 0)        // change ball's angle
          { 
            alpha= -((firstPos+5) - ballPosX);
          }
	ball_next_x();
    }
    else //the ball got caught with the pad and now the direction is up
    {
      display_draft[24][ballPosX].ch = ' ';
      display_draft[24][ballPosX].color = 128;
      player_live--; //the player losing life
 dying_sound();
      score=0;
      if(player_live==0)//going back to stage one
      {
      stage = STAGE_1;
      player_live=3;
      }
         reset();
    }
    ball_next_x();
  }
}
}
void ball_next_x()
{  
 
  if (alpha<0 )         // ball is going leftwise
  {
    if(ballPosX <=2 )
    {
      alpha = -alpha; 
    }  
  }
  else                  // ball is going rightwise
  {
    if(ballPosX >= 76)
    {
      alpha = -alpha; 
    }
  }
  
  ballPosX += alpha;  // advance the ball
} // ball_next_x
void WriteStr2VidRAMAttr( struct GameBlock acsii[DISPLAY_HEIGHT][DISPLAY_WIDTH])
{
	int row, col;
	for (row = 0; row<DISPLAY_HEIGHT; row++)
	{
		for (col = 0; col<DISPLAY_WIDTH; col++)
		{
      if(row!=24&&row!=5&&row!=0&&col!=0&&col!=79)
		display_char(row,col,display_draft);
		}

	}
} /*--WriteStr2VidRAM( )------------*/
void displayer()
{
  int i;
  while(1)
{
  receive();
  WriteStr2VidRAMAttr(display_draft);
}
}
void display_score()
{
  if(score>=0&&score<=9)
  {
  display_draft[3][41].ch=score+'0';
  display_draft[3][41].color=13;
  }
  else if (score>=10&& score <=99)
  {
  display_draft[3][41].ch=score/10+'0';
  display_draft[3][41].color=13;
  display_draft[3][42].ch=score%10+'0';
  display_draft[3][42].color=13;
  }
  else if(score >=100 && score <= 999)
  {
  display_draft[3][41].ch=score/100+'0';
  display_draft[3][41].color=13;
  display_draft[3][42].ch=(score/10)%10+'0';
  display_draft[3][42].color=13;
  display_draft[3][43].ch=score%10+'0';
  display_draft[3][43].color=13;
  }
}
/* this function handle all controls */
void handle_controls()
{
  while(front != -1)
  {
    ch = ch_arr[front];
    if(front != rear)
      front++;
    else
      front = rear = -1;

    // respond to controls in stack
    if ( (ch == 'r') || (ch == 'R') )
    {
      if(firstPos<=68&&lastPos<=78)
    {  
    firstPos++;
    lastPos++;
    draw_game_pad();
    ch1='r';
    }
    }
    else if ((ch == 'l') || (ch == 'L') )
    { 
       if(firstPos>=2)
    { 
      lastPos--;
      firstPos--;
          draw_game_pad();
      ch1='l';
    }
   } // while(front != -1)
	 else if ((ch == 'u') || (ch == 'U') )
    { 
      if(counter2==0)
      {
      gameOn--;
      if(gameOn==0)
      init_game();
      ch1='u';
      counter2++;
      }
   }
   ch=0;// void handle_controls()
  }
}
void receiver()
{
  char temp;
while(1)
{
  temp=receive();
  rear++;
  ch_arr[rear] = temp;
  if(front == -1)
  front = 0;

}
}

void updater()
{
  int i;
while(1)
{
  receive();
            handle_controls();
  if(gameOn == 0)
  {
   DesTime();
  display_score();
  for(i=0;i<player_live;i++)
      {
        display_draft[1][66+i].ch =3;
        display_draft[1][66+i].color=13;
      }
  if(timeleft==0)
 {
   stage = STAGE_1;
   player_live = 3;
   timeleft = 60;
   score = 0;
   reset();
  }
     draw_game_ball();
       check_target();
        draw_game_pad();
}
}
}
void DesTime(){


// desplay time now //
     readclk();
       display_draft[3][20].ch= hour/10+'0';
      display_draft[3][20].color=13;
      display_draft[3][21].ch=hour%10+'0';
      display_draft[3][21].color=13;
      display_draft[3][22].ch=':';
      display_draft[3][22].color=13;
      display_draft[3][23].ch=min/10+'0';
      display_draft[3][23].color=13;
      display_draft[3][24].ch=min%10+'0';
      display_draft[3][24].color=13;
      display_draft[3][25].ch= ':';
      display_draft[3][25].color=13;
      display_draft[3][26].ch=sec/10+'0';
      display_draft[3][26].color=13;
      display_draft[3][27].ch=sec%10+'0';
      display_draft[3][27].color=13; 
 
 
      //desplay the time lift  // 
      /*if(STAGE_1) {}
      if(STAGE_2) {timeleft=120;}
      if(STAGE_3) {timeleft=260;}*/
    //printf("   %d",timeleft);
 

      //display_draft[3][66].ch= timeleft/100+'0';
     // display_draft[3][66].color=13;
       if(timeleft>=0&&timeleft<=99)
       {
      display_draft[3][67].ch=timeleft/10+'0';
      display_draft[3][67].color=13;
      display_draft[3][68].ch=timeleft%10+'0';
      display_draft[3][68].color=13;
      display_draft[3][69].ch = ' ';
      display_draft[3][69].color = 128;
       }
       else if (timeleft>=100 &&timeleft<=999)
       {

      display_draft[3][67].ch=(timeleft/100)+'0';
      display_draft[3][67].color=13;
      display_draft[3][68].ch=(timeleft/10)%10+'0';
      display_draft[3][68].color=13;
      display_draft[3][69].ch=timeleft%10+'0';
      display_draft[3][69].color=13;
       }
     }
  

INTPROC new_int9(int mdevno)
{
 char result = 0;
 int scan = 0;
  int ascii = 0;
  int ch;
mdevno++;mdevno--;
asm {
    MOV AH,1
    INT 16h
    JZ  Skip1
    MOV AH,0
    INT 16h
    MOV BYTE PTR scan,AH
    MOV BYTE PTR ascii,AL
  } //asm
 if (scan == 77)
   result = 'r';
    else if (scan == 75)
     result = 'l';
 else if (scan == 72)
     result = 'u';
 
else  if ((scan == 46)&& (ascii == 3)) // Ctrl-C?
   asm{
     INT 27
   }

   send(receiver_pid, result); 

Skip1:
return 0;
} // new_int9
void set_new_int9_newisr()
{
  int i;
  for(i=0; i < 32; i++)
    if (sys_imp[i].ivec == 9)
    {
     sys_imp[i].newisr = new_int9;
     return;
    }

} // set_new_int9_newisr
void reset()
{
  counter=0;
  firstPos=35;
  lastPos=45;
  pad_width = 10;
  ballPosX=(firstPos+lastPos)/2;
  alpha=0;
  ballPosY=23;
  ball_dir=0;
  targetCount=0;
  counter2=0;
  h=5;
   display_draft[10][30].ch = 'P';
  display_draft[10][30].color = 13;
  display_draft[10][31].ch = 'l';
  display_draft[10][31].color = 13;
  display_draft[10][32].ch = 'e';
  display_draft[10][32].color = 13;
  display_draft[10][33].ch = 'a';
  display_draft[10][33].color = 13;
  display_draft[10][34].ch = 's';
  display_draft[10][34].color = 13;
 display_draft[10][35].ch = 'e';
  display_draft[10][35].color = 13;
  display_draft[10][37].ch = 'p';
  display_draft[10][37].color = 13;
  display_draft[10][38].ch = 'r';
  display_draft[10][38].color = 13;
  display_draft[10][39].ch = 'e';
  display_draft[10][39].color = 13;
  display_draft[10][40].ch = 's';
  display_draft[10][40].color = 13;
  display_draft[10][41].ch = 's';
  display_draft[10][41].color = 13;
   display_draft[10][43].ch = 'U';
  display_draft[10][43].color = 13;
  display_draft[10][44].ch = 'P';
  display_draft[10][44].color = 13;
  display_draft[10][46].ch = 't';
  display_draft[10][46].color = 13;
  display_draft[10][47].ch = 'o';
  display_draft[10][47].color = 13;
  display_draft[12][30].ch = 's';
  display_draft[12][30].color = 13;
   display_draft[12][31].ch = 't';
  display_draft[12][31].color = 13;
  display_draft[12][32].ch = 'a';
  display_draft[12][32].color = 13;
  display_draft[12][33].ch = 'r';
  display_draft[12][33].color = 13;
  display_draft[12][34].ch = 't';
  display_draft[12][34].color = 13;
  display_draft[12][40].ch = 'l';
  display_draft[12][40].color = 13;
  display_draft[12][41].ch = 'e';
  display_draft[12][41].color = 13;
  display_draft[12][42].ch = 'v';
  display_draft[12][42].color = 13;
  display_draft[12][43].ch = 'e';
  display_draft[12][43].color = 13;
  display_draft[12][44].ch = 'l';
  display_draft[12][44].color = 13;
  display_draft[12][45].ch = ' ';
  display_draft[12][45].color = 13;
  display_draft[12][46].ch = stage+'0';
  display_draft[12][46].color = 13;
  display_draft[14][30].ch ='Y';
  display_draft[14][30].color = 13; 
  display_draft[14][31].ch ='o';
  display_draft[14][31].color = 13; 
  display_draft[14][32].ch ='u';
  display_draft[14][32].color = 13; 
  display_draft[14][33].ch ='r';
  display_draft[14][33].color = 13; 
  display_draft[14][34].ch =' ';
  display_draft[14][34].color = 13; 
  display_draft[14][35].ch ='s';
  display_draft[14][35].color = 13; 
  display_draft[14][36].ch ='c';
  display_draft[14][36].color = 13; 
  display_draft[14][37].ch ='o';
  display_draft[14][37].color = 13; 
  display_draft[14][38].ch ='r';
  display_draft[14][38].color = 13; 
  display_draft[14][39].ch ='e';
  display_draft[14][39].color = 13; 
  display_draft[14][41].ch ='i';
  display_draft[14][41].color = 13; 
  display_draft[14][42].ch ='s';
  display_draft[14][42].color = 13; 
  display_draft[14][43].ch =':';
  display_draft[14][43].color = 13;
  if(score>=0&&score<=9)
  {
  display_draft[14][44].ch=score+'0';
  display_draft[3][44].color=13;
  }
  else if (score>=10&& score <=99)
  {
  display_draft[14][44].ch=score/10+'0';
  display_draft[14][44].color=13;
  display_draft[14][45].ch=score%10+'0';
  display_draft[14][45].color=13;
  }
  else if(score >=100 && score <= 999)
  {
  display_draft[14][44].ch=score/100+'0';
  display_draft[14][44].color=13;
  display_draft[14][45].ch=(score/10)%10+'0';
  display_draft[14][45].color=13;
  display_draft[14][46].ch=score%10+'0';
  display_draft[14][46].color=13;
  }
score =0;
  draw_game_barriers();
  draw_score_barriers();
  if(gameOn==0)
  {

  init_game();
    //  printf("ameer");
  }
}
SYSCALL schedule(int no_of_pids, int cycle_length, int pid1, ...)
{
  int i;
  int ps;
  int *iptr;
  disable(ps);

  gcycle_length = cycle_length;
  point_in_cycle = 0;
  gno_of_pids = no_of_pids;

  iptr = &pid1;
  for(i=0; i < no_of_pids; i++)
  {
    sched_arr_pid[i] = *iptr;
    iptr++;
    sched_arr_int[i] = *iptr;
    iptr++;
  } // for
  restore(ps);

} // schedule 
xmain()
{
 	srand(time(NULL));
  b800h = (unsigned char far *)0xB8000000;
  asm{
		mov AH,0
		mov AL,03
		INT 10H
  }
  //
  display_draft[10][30].ch = 'P';
  display_draft[10][30].color = 13;
  display_draft[10][31].ch = 'l';
  display_draft[10][31].color = 13;
  display_draft[10][32].ch = 'e';
  display_draft[10][32].color = 13;
  display_draft[10][33].ch = 'a';
  display_draft[10][33].color = 13;
  display_draft[10][34].ch = 's';
  display_draft[10][34].color = 13;
 display_draft[10][35].ch = 'e';
  display_draft[10][35].color = 13;
  display_draft[10][37].ch = 'p';
  display_draft[10][37].color = 13;
  display_draft[10][38].ch = 'r';
  display_draft[10][38].color = 13;
  display_draft[10][39].ch = 'e';
  display_draft[10][39].color = 13;
  display_draft[10][40].ch = 's';
  display_draft[10][40].color = 13;
  display_draft[10][41].ch = 's';
  display_draft[10][41].color = 13;
   display_draft[10][43].ch = 'U';
  display_draft[10][43].color = 13;
  display_draft[10][44].ch = 'P';
  display_draft[10][44].color = 13;
  display_draft[10][46].ch = 't';
  display_draft[10][46].color = 13;
  display_draft[10][47].ch = 'o';
  display_draft[10][47].color = 13;
  display_draft[12][30].ch = 's';
  display_draft[12][30].color = 13;
   display_draft[12][31].ch = 't';
  display_draft[12][31].color = 13;
  display_draft[12][32].ch = 'a';
  display_draft[12][32].color = 13;
  display_draft[12][33].ch = 'r';
  display_draft[12][33].color = 13;
  display_draft[12][34].ch = 't';
  display_draft[12][34].color = 13;
  display_draft[12][40].ch = 'l';
  display_draft[12][40].color = 13;
  display_draft[12][41].ch = 'e';
  display_draft[12][41].color = 13;
  display_draft[12][42].ch = 'v';
  display_draft[12][42].color = 13;
  display_draft[12][43].ch = 'e';
  display_draft[12][43].color = 13;
  display_draft[12][44].ch = 'l';
  display_draft[12][44].color = 13;
  display_draft[12][45].ch = ' ';
  display_draft[12][45].color = 13;
  display_draft[12][46].ch = stage+'0';
  display_draft[12][46].color = 13;
  draw_game_barriers();
  draw_score_barriers();
resume( dispid = create(displayer, INITSTK, INITPRIO, "DISPLAYER", 0) );
resume( recvpid = create(receiver, INITSTK, INITPRIO+3, "RECIVEVER", 0) );
resume( uppid = create(updater, INITSTK, INITPRIO, "UPDATER", 0) );
receiver_pid =recvpid;
set_new_int9_newisr();
set_Int70hHandler();
schedule(2,2, dispid, 0,  uppid, 1);
}

