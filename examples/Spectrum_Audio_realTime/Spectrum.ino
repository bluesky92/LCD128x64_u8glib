
// include the library header
#include "U8glib.h"
#include <fix_fft.h>

U8GLIB_ST7920_128X64_4X u8g(18, 16, 17);  // SPI Com: SCK = en = 18, MOSI = rw = 16, CS = di = 17
#define L_IN 1 // Audio input A0 Arduino
#define R_IN 0 // Audio input A1 Arduino

 
const int SIZE =64; // Have to in formula: 2^(x) = 64, 128,256... where x =1,2,3,4,5,6 ... 
 
char im[SIZE], data[SIZE];
char Rim[SIZE], Rdata[SIZE];

double LData[SIZE];
double RData[SIZE];
double data_Up[SIZE];
double data_Down[SIZE];
int debugLoop;
int i;
int load; 

int Base = 30;
int offset = 10;


void u8g_prepare(void) {
  // установить начальные параметры 
  u8g.setFont(u8g_font_6x10);
  u8g.setFontRefHeightExtendedText();
  u8g.setDefaultForegroundColor();
  u8g.setFontPosTop();
}

void u8g_line() {
  //рисуем оси 
    u8g.drawStr( 0, 0, "pi");
    u8g.drawStr( 0, 55, "-pi");
    u8g.drawStr( 0, 25, "0");
    //u8g.drawStr( 25, 56, "f,Hz");
    u8g.drawLine(offset, 10, offset, 55);  //Ось У
    u8g.drawLine(offset, Base, 120, Base); // ось Х
}


uint8_t draw_state = 0;
void draw(void) {
  u8g_prepare();
}

void setup() {
  analogReference (EXTERNAL); 
  }

void loop() {
  int test_up,test_down,test_up1,test_down1;
  
  //***************************Processing**********************
  for (int i = 0; i < SIZE; i++) {    // 64 bins = 32 bins of usable spectrum data
    data[i]  = ((analogRead(L_IN) / 4 ) );  // chose how to interpret the data from analog in
    im[i]  = 0;   // imaginary component
    Rdata[i] = ((analogRead(R_IN) / 4 ) );  // chose how to interpret the data from analog in
    Rim[i] = 0;   // imaginary component
     }
    fix_fft(data, im, log(SIZE)/log(2), 0);   // Send Left channel normalized analog values through fft
    fix_fft(Rdata, Rim, log(SIZE)/log(2), 0); // Send Right channel normalized analog values through fft
  for (int i = 0; i < SIZE; i++) {
    LData[i] = atan2((double)data[i],(double)im[i]);// Phase of  Left- chanel   
    RData[i] = atan2((double)Rdata[i],(double)Rim[i]); //phase of Right -chanel 
    // COPY the Right low-band (0-15) into the Left high-band (16-31) for display ease
   
   if (i<SIZE/4){
    // Update for Left-chanel
     if (LData[i] >=0) {
         data_Up[i] = LData[i];
         data_Down[i] = 0;
      }
      else {
          data_Up[i] = 0;
          data_Down[i] = LData[i]*(-1);
       }
       }
       else {
        // Update for Right-chanel
           if (RData[i-SIZE/4] >=0){
               data_Up[i] = RData[i-SIZE/4];
               data_Down[i] = 0;
            }
           else{
              data_Up[i] = 0;
              data_Down[i] = RData[i-SIZE/4]*(-1);
           }
        }
   
    // Remap values to physical display constraints...  
    data_Down[i] = constrain(data_Down[i], 0, 3.14 );     //data samples * range (0 до Пи)  
    data_Down[i] = map(data_Down[i], 0, 3.14, 0, 25);  // remap averaged values ( 0 до 25)
    data_Up[i] = constrain(data_Up[i], 0, 3.14 );     //data samples * range (0 до Пи)  
    data_Up[i] = map(data_Up[i], 0, 3.14, 0, 25);  // remap averaged values ( 0 до 25)
   }
  //*****************************************************
u8g.firstPage();  
  do {
  u8g_prepare();
  for(int i=0;i<SIZE;i++)
  {
  // for up-land
  test_up=(Base-data_Up[i+1]) - (Base-data_Up[i]); 
  test_down=(Base-data_Up[i]) - (Base-data_Up[i+1]);
  // for down-land
  test_up1=(Base+data_Down[i+1]) - (Base+data_Down[i]); 
  test_down1=(Base+data_Down[i]) - (Base+data_Down[i+1]);
  
    if(test_up>0 && i!=(SIZE-1)){
       u8g.drawLine( i+offset, Base-data_Up[i], i+offset, Base-data_Up[i+1]);
     }
   else if(test_down>0 && i!=(SIZE-1)){
           u8g.drawLine( i+offset, Base-data_Up[i+1], i+offset, Base-data_Up[i]);
           
          }
   else
   u8g.drawPixel(i+offset, Base-data_Up[i]);
   // for other land 
     if(test_up1>0 && i!=(SIZE-1)){
         u8g.drawLine( i+offset, Base+data_Down[i], i+offset, Base+data_Down[i+1]);
     }
   else if(test_down1>0 && i!=(SIZE-1)){
          u8g.drawLine( i+offset, Base+data_Down[i+1], i+offset, Base+data_Down[i]);
         }
   else
   u8g.drawPixel(i+offset, Base+data_Down[i]);
  }
              
            u8g_line();
     }while( u8g.nextPage() );
}
