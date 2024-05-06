#include <stdio.h> 
#include <stdlib.h> 

#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050

volatile int * KEY_ptr =(volatile int *) KEY_BASE;  
volatile int * SW_ptr  =(volatile int *) SW_BASE;
//Colours
#define BLACK 0x0000 		
#define GREEN 0x07E0 	
#define RED 0xF800 	 	
#define WHITE 0xFFFF	
#define LGRAY 0xC618 	
#define NAVY 0x000F 

const unsigned char segmentPatterns[10] = {
    0b11111100,  // 0
    0b01100000,  // 1
    0b11011010,  // 2
    0b11110010,  // 3
    0b01100110,  // 4
    0b10110110,  // 5
    0b10111110,  // 6
    0b11100000,  // 7
    0b11111110,  // 8
    0b11110110   // 9
};

// const char bitCodes[10] = { 0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01100111 };
int winlvl = 0;
char maingrid[320][240];
char marker='#';
int dir[]={0,0};
int arr_right[50];
int arr_righty[50];
int arr_left[50];
int arr_lefty[50];
int noofrects;
int currcentrex,currcentrey,radius;
int over=0;
int score=0;
int tottime=0;

// GRID SIZE: 320 X 240 

void write_pixel(int x,int y,short colour);
void clear_screen();
void write_char(int x,int y,char ch);
void clear_char();
void draw_line(int x1,int y1,int x2,int y2,short color);
void draw_circle(int centerX,int centerY,int radius,short color);
void checkforbuttonpress();
void move_rects();
void mySleep();
void moveright();
void burst();

struct pair{
    int x;int y;
};

short colors[] = {WHITE,GREEN,RED,LGRAY,NAVY};
int szofcolors = 5;


// ╭╮╭╮╭┳━━━┳╮  ╭━━━┳━━━┳━╮╭━┳━━━╮
// ┃┃┃┃┃┃╭━━┫┃  ┃╭━╮┃╭━╮┃┃╰╯┃┃╭━━╯
// ┃┃┃┃┃┃╰━━┫┃  ┃┃ ╰┫┃ ┃┃╭╮╭╮┃╰━━╮
// ┃╰╯╰╯┃╭━━┫┃ ╭┫┃ ╭┫┃ ┃┃┃┃┃┃┃╭━━╯
// ╰╮╭╮╭┫╰━━┫╰━╯┃╰━╯┃╰━╯┃┃┃┃┃┃╰━━╮
//  ╰╯╰╯╰━━━┻━━━┻━━━┻━━━┻╯╰╯╰┻━━━╯


/* set a single pixel on the screen at x,y
 * x in [0,319], y in [0,239], and colour in [0,65535]
 */
void write_pixel(int x, int y, short colour) {
  volatile short *vga_addr=(volatile short*)(0x08000000 + (y<<10) + (x<<1));
  *vga_addr=colour;
}

int check_pixel(int x,int y,int color){
    volatile short *vga_addr=(volatile short*)(0x08000000 + (y<<10) + (x<<1));
    if(*vga_addr==color)return 1;
    return 0;
}

/* write a single character to the character buffer at x,y
 * x in [0,79], y in [0,59]
 */
void write_char(int x, int y, char c) {
  // VGA character buffer
  volatile char * character_buffer = (char *) (0x09000000 + (y<<7) + x);
  *character_buffer = c;
}

void clear_screen() {
  int x, y;
  for (x = 0; x < 320; x++) {
    for (y = 0; y < 240; y++){
	  write_pixel(x,y,BLACK);
	}
  }
}

void clear_char(){
  char ch='\0';
  for(int x=0;x<80;x++){
    for(int y=0;y<60;y++){
      write_char(x,y,ch);
    }
  }
}

void displayDigit(unsigned char digit) {
    if (digit >= 0 && digit <= 9) {
        unsigned char segmentPattern = segmentPatterns[digit];
        // Code to send segmentPattern to the 7-segment display
        printf("Displaying digit %u on 7-segment display (pattern: 0x%02X)\n", digit, segmentPattern);
    } else {
        printf("Invalid digit: %u\n", digit);
    }
}

// Function to display a score on a 7-segment display
void displayScore() {
    // Extract each digit from the score and display on 7-segment display
    int remainingScore = score;
    while (remainingScore > 0) {
        int digit = remainingScore % 10;
        displayDigit(digit);
        remainingScore /= 10;
    }
}

int checkinterr(int x,int y){
    if(maingrid[x][y]=='D' || (currcentrey+radius>=210 || currcentrey-radius<=30)){
        burst();return 1;
    }
    return 0;
}

void checkinterrect(int x,int y){
    for(int i=x;i<=x+50;++i){
        for(int j=y;j<=15;++j){
            if(maingrid[i][j]=='C')burst;
        }
    }
}

int checkinterrline(int x,int y){
    if(maingrid[x][y]=='C'){
        burst();return 1;
    }
    return 0;
}

void draw_line(int x1, int y1, int x2, int y2,short color){
    if(over)return;
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    char mark='#';
    if(color==WHITE)mark='D';
    while (x1 != x2 || y1 != y2){
        if(!checkinterrline(x1,y1)){
            write_pixel(x1, y1,color);
        }
        maingrid[x1][y1]=mark;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void draw_circle(int centerX, int centerY, int radius,short color){
    if(over)return;
    if(centerX+radius>=319){
        winlvl=1;return;
    }
    char mark = 'C';
    if(color==BLACK)mark='#';
    int r = radius;
    while(r--){
        int x = r;
        int y = 0;
        int err = 0;

        while (x >= y){
            if(checkinterr(centerX+x,centerY+y))return;
            write_pixel(centerX + x, centerY + y, color);
            maingrid[centerX+x][centerY+y]=mark;
            if(checkinterr(centerX+y,centerY+x))return;
            write_pixel(centerX + y, centerY + x, color);
            maingrid[centerX+y][centerY+x]=mark;
            if(checkinterr(centerX-y,centerY+x))return;
            write_pixel(centerX - y, centerY + x, color);
            maingrid[centerX-y][centerY+x]=mark;
            if(checkinterr(centerX-x,centerY+y))return;
            write_pixel(centerX - x, centerY + y, color);
            maingrid[centerX-x][centerY+y]=mark;
            if(checkinterr(centerX-x,centerY-y))return;
            write_pixel(centerX - x, centerY - y, color);
            maingrid[centerX-x][centerY-y]=mark;
            if(checkinterr(centerX-y,centerY-x))return;
            write_pixel(centerX - y, centerY - x, color);
            maingrid[centerX-y][centerY-x]=mark;
            if(checkinterr(centerX+y,centerY-x))return;
            write_pixel(centerX + y, centerY - x, color);
            maingrid[centerX+y][centerY-x]=mark;
            if(checkinterr(centerX+x,centerY-y))return;
            write_pixel(centerX + x, centerY - y, color);
            maingrid[centerX+x][centerY-y]=mark;

            if (err <= 0) {
                y += 1;
                err += 2*y + 1;
            }
            if (err > 0) {
                x -= 1;
                err -= 2*x + 1;
            }
        }
    }
    marker='#';
    return;
}

//draws rectangle given corners in cyclic order
void draw_rect(int c1x,int c1y,int c2x,int c2y,int c3x,int c3y,int c4x,int c4y,short color){
    checkinterrect(c1x,c1y);
    if(color==WHITE)marker = 'D'; //indicates line is drawn
    draw_line(c1x,c1y,c2x,c2y,color);
    draw_line(c2x,c2y,c3x,c3y,color);
    draw_line(c3x,c3y,c4x,c4y,color);
    draw_line(c4x,c4y,c1x,c1y,color);
    marker='#';
}

//draws rectangle given corners in cyclic order
void draw_tri(int c1x,int c1y,int c2x,int c2y,int c3x,int c3y,short color){
    marker = 'D'; // indicates DANGER
    draw_line(c1x,c1y,c2x,c2y,color);
    draw_line(c2x,c2y,c3x,c3y,color);
    draw_line(c3x,c3y,c1x,c1y,color);
    marker='#';
}

void delay_ms(int milliseconds) {
    int iterations = milliseconds * 1000;  
    
    // Perform busy-waiting for the specified number of iterations
    for (int i = 0; i < iterations; i++) {
        asm("nop");  // No-operation (assembly instruction to do nothing)
    }
}

void moveup(){
    //Hex3_0();
    int x = currcentrex,y=currcentrey,r=radius;
    int timr = 45;
    int cntr=0,mod=30;
    // for(int j=0;j<40;++j){
    //     if(!cntr)generate_rects();
    //     move_rects();
    //     int swp = *SW_ptr;
    //     if(swp==1){
    //         movedown();
    //     }
    //     draw_circle(x,y,r,BLACK);
    //     y--;
    //     draw_circle(x,y,r,WHITE);
    //     move_rects();
    //     cntr++;cntr%=30;
    // }
    //delay_ms(timr);
    // for(int j=0;j<40;++j){
    //     move_rects();
    //     //delay_ms(timr);
    //     draw_circle(x,y,r,BLACK);
    //     y++;
    //     //delay_ms(timr);
    //     draw_circle(x,y,r,WHITE);
    //     move_rects();
    // }
    while(1){
        score++;tottime++;
        //displayScore();
        y=currcentrey;
        if(!cntr)generate_rects();
        move_rects();
        int swp = *SW_ptr;
        if(swp==1)movedown();
        else if(swp!=2)break;
        delay_ms(timr);
        draw_circle(x,y,r,BLACK);
        move_rects();
        currcentrey--;y--;
        delay_ms(timr);
        draw_circle(x,y,r,WHITE);
        move_rects();
        if(mod>=5 && tottime%1000000000000==0)mod--;
        cntr++;cntr%=mod;
    }
}

void movedown(){
    score++;
    //displayScore();
    int x = currcentrex,y=currcentrey,r=radius;
    int timr = 45;
    int cntr=0,mod=30;
    // for(int j=0;j<40;++j){
    //     move_rects();
    //     draw_circle(x,y,r,BLACK);
    //     x++;
    //     draw_circle(x,y,r,WHITE);
    //     move_rects();
    // }
    // while(maingrid[x][y+r]!='L'){
    //     draw_circle(x,y,r,BLACK);
    //     move_rects();
    //     y++;
    //     draw_circle(x,y,r,WHITE);
    //     move_rects();
    // }
    // draw_circle(x,y,r,BLACK);
    while(1){
        y=currcentrey;tottime++;score++;
        if(!cntr)generate_rects();
        int swp = *SW_ptr;
        if(swp==2)moveup();
        else if(swp!=1)break;
        delay_ms(timr);
        draw_circle(x,y,r,BLACK);
        move_rects();
        currcentrey++;y++;
        delay_ms(timr);
        draw_circle(x,y,r,WHITE);
        move_rects();
        cntr++;cntr%=mod;
        if(mod>=5 && tottime%1000000000000==0)mod--;
    }
}

void endgame(){
    while(1){
        clear_screen();
        clear_char();

        char * gameover[] = {
            " ######      ###    ##     ## ########   #######  ##     ## ######## ########",  
            "##    ##    ## ##   ###   ### ##        ##     ## ##     ## ##       ##     ##", 
            "##         ##   ##  #### #### ##        ##     ## ##     ## ##       ##     ##", 
            "##   #### ##     ## ## ### ## ######    ##     ## ##     ## ######   ########",  
            "##    ##  ######### ##     ## ##        ##     ##  ##   ##  ##       ##   ##",   
            "##    ##  ##     ## ##     ## ##        ##     ##   ## ##   ##       ##    ##",  
            " ######   ##     ## ##     ## ########   #######     ###    ######## ##     ##",
            "\0"
        };
        //displayScoreOnHex3_0();
        print_arr(gameover,10);
        int sw = *SW_ptr;
        if(sw==3){over=0;start();}
    }
}

void burst(){
    int timr = 100;
    // for(int i=0;i<320;++i){
    //     for(int j=0;j<240;++j){
    //         if(maingrid[i][j]=='C'){
    //             maingrid[i][j]='#';
    //             write_pixel(i,j,BLACK);
    //         }
    //     }
    // }
    int x = currcentrex;
    // while(radius && currcentrey-radius>=0){
    //     delay_ms(timr);
    //     draw_circle(x,currcentrey,radius,BLACK);
    //     currcentrey--; radius--;
    //     delay_ms(timr);
    //     draw_circle(x,currcentrey,radius,WHITE);
    // }
    over=1;endgame();
}

//Choose x as 30 or 249
//Choose y between 30 and 180

void generate_rects(){
    //draw_rect(0,140,39,140,40,239,0,239,WHITE);
    int xchoose = (rand()%2)+1,x,y;
    if(xchoose==1)x=30;
    else x = 248;

    y = rand()%(151)+30;
    int fl=0;
    for(int i=0;i<50;++i){
        int yr = arr_righty[i],yl = arr_lefty[i];
        if((y>=yr && y<=yr+15)||(y<=yr && y+15>=yr)||(y>=yl && y<=yl+15)||(y<=yl && y+15>=yl)){
            fl=1;break;
        }
    }
    if(!fl){
        draw_rect(x,y,x+50,y,x+50,y+15,x,y+15,WHITE);
        if(xchoose==1){
            for(int i=0;i<50;++i){
                if(arr_right[i]==-1){
                    arr_right[i] = x;
                    arr_righty[i] = y;
                    break;
                }
            }
        }
        else{
            for(int i=0;i<50;++i){
                if(arr_left[i]==-1){
                    arr_left[i] = x;
                    arr_lefty[i] = y;
                    break;
                }
            }
        }
    }
}

void generate_triangles(){
    int thornsx=0;
    for(int i=0;i<10;++i){
        draw_tri(thornsx,239,thornsx+16,209,thornsx+32,239,WHITE);
        thornsx+=32;
    }
    int thornsy=24;
    for(int i=0;i<8;++i){
        draw_tri(0,thornsy,20,thornsy+12,0,thornsy+24,WHITE);
        thornsy+=24;
    }
    thornsy=24;
    for(int i=0;i<8;++i){
        draw_tri(319,thornsy,299,thornsy+12,319,thornsy+24,WHITE);
        thornsy+=24;
    }
    thornsx=0;
    for(int i=0;i<10;++i){
        draw_tri(thornsx,0,thornsx+16,30,thornsx+32,0,WHITE);
        thornsx+=32;
    }
}

void move_rects(){
    float timr = 2.5;
    if(timr==2 && tottime>=1000000000000)timr--;
    for(int i=0;i<50;++i){
        delay_ms(timr);
        int xr = arr_right[i],yr = arr_righty[i];
        int xl = arr_left[i],yl = arr_lefty[i];
        if(xr!=-1){
            if(xr+1>248){
                draw_rect(xr,yr,xr+50,yr,xr+50,yr+15,xr,yr+15,BLACK);
                arr_right[i]=-1;arr_righty[i]=-1;
            }
            else{
                draw_rect(xr,yr,xr+50,yr,xr+50,yr+15,xr,yr+15,BLACK);
                arr_right[i]++;
                xr = arr_right[i];
                draw_rect(xr,yr,xr+50,yr,xr+50,yr+15,xr,yr+15,WHITE);
            }
        }
        if(xl!=-1){
            if(xl-1<30){
                draw_rect(xl,yl,xl+50,yl,xl+50,yl+15,xl,yl+15,BLACK);
                arr_left[i]=-1;arr_lefty[i]=-1;
            }
            else{
                draw_rect(xl,yl,xl+50,yl,xl+50,yl+15,xl,yl+15,BLACK);
                arr_left[i]--;
                xl = arr_left[i];
                draw_rect(xl,yl,xl+50,yl,xl+50,yl+15,xl,yl+15,WHITE);
            }
        }
    }
}

void checkforbuttonpress(){
    int buttonpress = *KEY_ptr;
    if(buttonpress==2)moveup();
    else if(buttonpress==1)movedown();
}

void start(){
    clear_screen();
    clear_char();
    over=0;
    score=0;
    tottime=0;

    for(int i=0;i<320;++i){
        for(int j=0;j<240;++j){
            maingrid[i][j]='#';
        }
    }

    //noofrects = 2;
    currcentrex = 160 ,currcentrey =120 ,radius = 10;

    for(int i=0;i<50;++i)arr_left[i]=-1;
    for(int i=0;i<50;++i)arr_right[i]=-1;

    arr_right[0]=40;arr_righty[0]=135;

    int buttonpress = *KEY_ptr;
    int swp = *SW_ptr;

    draw_circle(currcentrex,currcentrey,radius,WHITE);
    int counterforrectgen=0;

    generate_triangles();

    while(1){
        score++;
        tottime++;
        //displayScore();
        if(counterforrectgen==0)generate_rects();
        move_rects();
        draw_circle(currcentrex,currcentrey,radius,WHITE);
        swp = *SW_ptr;
        if(swp==1)movedown();
        else if(swp==2)moveup();
        draw_circle(currcentrex,currcentrey,radius,WHITE);
        counterforrectgen++;
        counterforrectgen%=50;
    }
}

int print_arr(char* arr[],int yval){
    int x,y;
    y=yval;
    char * ch="\0";
    char **ptr = arr;
    while (*ptr != ch) {
        x=2;
        char *line = *ptr;
        while (*line != '\0') {
            write_char(x,y,*line);
            line++;
            x++;
        }
        y++;
        ptr++;
    }
    return y;
}

void print_wlcm(){
    delay_ms(500);
    clear_char();
    clear_screen();
    char *welcome_art[] = {     
          "'##:::::'##:'########:'##::::::::'######:::'#######::'##::::'##:'########:",
          "##:'##: ##: ##.....:: ##:::::::'##... ##:'##.... ##: ###::'###: ##.....::",
          "##: ##: ##: ##::::::: ##::::::: ##:::..:: ##:::: ##: ####'####: ##:::::::",
          "##: ##: ##: ######::: ##::::::: ##::::::: ##:::: ##: ## ### ##: ######:::",
          "##: ##: ##: ##...:::: ##::::::: ##::::::: ##:::: ##: ##. #: ##: ##...::::",
          "##: ##: ##: ##::::::: ##::::::: ##::: ##: ##:::: ##: ##:.:: ##: ##:::::::",
         ". ###. ###:: ########: ########:. ######::. #######:: ##:::: ##: ########:",
         ":...::...:::........::........:::......::::.......:::..:::::..::........::",                                
         "\0" // NULL terminator to indicate end of array
    };

    char *roll[] = {
    
         ":'#######:::'#######::::'#####:::::'##::::'######:::'######:::'#######::::'##:::",
         "'##.... ##:'##.... ##::'##.. ##::'####:::'##... ##:'##... ##:'##.... ##:'####:::",
         "..::::: ##:..::::: ##:'##:::: ##:.. ##::: ##:::..:: ##:::..::..::::: ##:.. ##:::",
         ":'#######:::'#######:: ##:::: ##::: ##::: ##:::::::. ######:::'#######:::: ##:::",
         "'##::::::::'##:::::::: ##:::: ##::: ##::: ##::::::::..... ##::...... ##::: ##:::",
         "'##:::::::: ##::::::::. ##:: ##:::: ##::: ##::: ##:'##::: ##:'##:::: ##::: ##:::",
          "#########: #########::. #####:::'######:. ######::. ######::. #######::'######:",
         ".........::.........::::.....::::......:::......::::......::::.......:::......::",
         "\0" // NULL terminator to indicate end of array
    };    

    char * presstostart[] = {
               "___________________________________     _______",     
               "7     77  _  77     77     77     7     7     7",     
               "|  -  ||    _||  ___!|  ___!|  ___!     |___  !",     
               "|  ___!|  _ `. |  __|_!__   7!__   7      |__  |",     
               "|  7   |  7  ||     77     |7     |     |     |",     
               "!__!   !__!__!!_____!!_____!!_____!     !_____!",     
                                                                   
          "_______________     _____________________________________",
          "7      77     7     7     77      77  _  77  _  77      7",
          "!__  __!|  7  |     |  ___!!__  __!|  _  ||    _|!__  __!",
          "  7  7  |  |  |     !__   7  7  7  |  7  ||  _ `.   7  7",  
          "  |  |  |  !  |     7     |  |  |  |  |  ||  7  |  |  |",  
          "  !__!  !_____!     !_____!  !__!  !__!__!!__!__!  !__!",
            "\0"
        };

    print_arr(presstostart,print_arr(roll,print_arr(welcome_art,10)+7)+2);
}

void instructions(){
    clear_char();
    clear_screen();
    
    char* instrs[] = {
        "FOLLOW THE FOLLOWING INSTRUCTIONS TO PLAY THE GAME:",
        "                                                     ",
        "THE GAME CONSISTS OF BOUNCE THE BALL WHO IS TRAPPED IN THE THORN WORLD",
        "                                                     ",
        "AS HE THINKS OF A WAY TO GET OUT, HELP HIM STAY ALIVE BY PREVENTING HIM",
        "                                                     ",
        "TO BUMP INTO THORNS, OR ONCOMING BRICKS SENT BY HIS ENEMY BOB!",
        "                                                     ",
        "AS TIME INCREASES, THE FREQUENCY OF BRICKS THROWN ALSO INCREASE",
        "                                                     ",
        "PRESS SWITCHES '1' AND '2' RESPECTIVELY TO MAKE HIM MOVE DOWN AND UP",
        "                                                     ",
        "YOUR SCORE INCREASES AS LONG AS BOUNCE LIVES!",
        "                                                     ",
        "PUSH BUTTON 1 TO START",
        "\0"
    };

    char* rules[] = {
        "8888888b.  888     888 888      8888888888 .d8888b.", 
        "888   Y88b 888     888 888      888       d88P  Y88b", 
        "888    888 888     888 888      888       Y88b.",      
        "888   d88P 888     888 888      8888888    Y888b.",   
        "8888888P   888     888 888      888           Y88b.", 
        "888 T88b   888     888 888      888             888", 
        "888  T88b  Y88b. .d88P 888      888       Y88b  d88P", 
        "888   T88b   Y88888P   88888888 8888888888  Y8888P",
         "\0"
    };

    print_arr(instrs,print_arr(rules,10)+5);

}

int main(){
    clear_screen();

    for(int i=0;i<320;++i){
        for(int j=0;j<240;++j){
            maingrid[i][j]=marker;
        }
    }

    int swp = *SW_ptr;
    int keyp = *KEY_ptr;

    while(1){
        swp=*SW_ptr;
        if(swp==3){
            instructions();break;
        }
        print_wlcm();
    }
    while(1){
        keyp=*KEY_ptr;
        if(keyp==1){
            start();break;
        }
    }
    return 0;
}