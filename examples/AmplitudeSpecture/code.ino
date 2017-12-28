

// include the library header
#include "U8glib.h"
#include <fix_fft.h>

U8GLIB_ST7920_128X64_4X u8g(18, 16, 17);	// SPI Com: SCK = en = 18, MOSI = rw = 16, CS = di = 17
#define L_IN 1 // Audio input A0 Arduino
#define R_IN 0 // Audio input A1 Arduino

const int Yres = 8;
const int gain = 3;
float peaks[64];
char im[64], data[64];
char Rim[64], Rdata[64];
char data_avgs[64];
int debugLoop;
int i;
int load; 


//int sig_v[113];
void u8g_prepare(void) {
  u8g.setFont(u8g_font_6x10);
  u8g.setFontRefHeightExtendedText();
  u8g.setDefaultForegroundColor();
  u8g.setFontPosTop();
}

void u8g_line() {
  u8g.drawStr( 0, 0, "pi");
  u8g.drawStr( 0, 55, "-pi");
  u8g.drawStr( 0, 25, "0");
  u8g.drawStr( 25, 56, "f,Hz");
  u8g.drawLine(7, 10, 7, 55);  //45
  u8g.drawLine(7, 30, 120,30); // oci x
}


uint8_t draw_state = 0;
void draw(void) {
  u8g_prepare();
 
}
int sig_v[113];
int moctoado = 55;
void setup() {
	 analogReference (EXTERNAL);
	 Serial.begin(9600);
  }

void loop() {
  int test_up,test_down; 
  int time_div;
  int min_amp=53, max_amp=0;
  
  for(int i=0;i<113;i++){
       //sig= analogRead(0);
       //delayMicroseconds(10);
       sig_v[i] = map(analogRead(0), 0, 1023, 0, 45);
       time_div=analogRead(2)*2;
       delayMicroseconds(time_div); 
    }
	//***************************Pros**********************
	
	for (int i = 0; i < 64; i++) {    // 64 bins = 32 bins of usable spectrum data
		data[i]  = ((analogRead(L_IN) / 4 ) );  // chose how to interpret the data from analog in
		im[i]  = 0;   // imaginary component
		Rdata[i] = ((analogRead(R_IN) / 4 ) );  // chose how to interpret the data from analog in
		Rim[i] = 0;   // imaginary component
     }
	 //fix_fft(data, im, 6, 0);   // Send Left channel normalized analog values through fft
	 fix_fft(Rdata, Rim, 6, 0); // Send Right channel normalized analog values through fft
	for (int i = 0; i < 32; i++) {
		data[i] = sqrt(data[i]  *  data[i] +  im[i] *  im[i]);
		Rdata[i] = sqrt(Rdata[i] * Rdata[i] + Rim[i] * Rim[i]);

		// COPY the Right low-band (0-15) into the Left high-band (16-31) for display ease
		if (i < 16) {
		  data_avgs[i] = data[i];
		}
		else {
		  data_avgs[i] = Rdata[i - 16];
		}

		// Remap values to physical display constraints... that is, 8 display custom character indexes + "_"
		data_avgs[i] = constrain(data_avgs[i], 0, 9 - gain);     //data samples * range (0-9) = 9
		data_avgs[i] = map(data_avgs[i], 0, 9 - gain, 0, Yres);  // remap averaged values
		Serial.println(data_avgs[i]);
   }

	//*****************************************************
u8g.firstPage();  
  do {
  u8g_prepare();
  for(int i=0;i<113;i++)
  {
  test_up=(moctoado-sig_v[i+1]) - (moctoado-sig_v[i]); 
  test_down=(moctoado-sig_v[i]) - (moctoado-sig_v[i+1]);
  if(test_up>0 && i!=112)
  {
    u8g.drawLine( i+7, moctoado-sig_v[i], i+7, moctoado-sig_v[i+1]);
  }
  else if(test_down>0 && i!=112)
  u8g.drawLine( i+7, moctoado-sig_v[i+1], i+7, moctoado-sig_v[i]);
  else
   u8g.drawPixel(i+7, moctoado-sig_v[i]);
  //Serial.println(sig_v[i]);
  min_amp=min(min_amp,sig_v[i]);
  max_amp=max(max_amp,sig_v[i]);
  }
	 
           //draw();
           
            u8g_line();
            
            //u8g.drawStr( 40, 57, String(time_div));
         //   u8g.drawStr(50, 57, "uS");
           // u8g.drawStr( 94, 57, String((max_amp-min_amp)/10.6));
          //  u8g.drawStr(110, 57, "V");
     }while( u8g.nextPage() );
  
 // int sig = analogRead(0);
  /* int test_up,test_down; 
  int time_div;
  int min_amp=53, max_amp=0;
   
  for(int i=0;i<113;i++)
{
 analogWrite(7, i+2);
 //sig= analogRead(0);
 //delayMicroseconds(10);
 sig_v[i] = map(analogRead(0), 0, 1023, 0, 53);
 time_div=analogRead(2)*2;
 delayMicroseconds(time_div); 
}
//GLCD.ClearScreen()   ;


GLCD.FillRect( 15, 0, 112, 53, WHITE);
GLCD.DrawVLine(70, 0, 53, BLACK);
GLCD.DrawHLine(14, 26, 113, BLACK);
  for(int i=0;i<113;i++)
  {
  test_up=(53-sig_v[i+1]) - (53-sig_v[i]);
  test_down=(53-sig_v[i]) - (53-sig_v[i+1]);
  if(test_up>0 && i!=112)
  {
    GLCD.DrawLine( i+15, 53-sig_v[i], i+15, 53-sig_v[i+1], BLACK);
  }
  else if(test_down>0 && i!=112)
  GLCD.DrawLine( i+15, 53-sig_v[i+1], i+15, 53-sig_v[i], BLACK);
  else
   GLCD.SetDot(i+15, 53-sig_v[i], BLACK);
  //Serial.println(sig_v[i]);
  min_amp=min(min_amp,sig_v[i]);
  max_amp=max(max_amp,sig_v[i]);
  
  }
*/
  
    
  
 
  }
