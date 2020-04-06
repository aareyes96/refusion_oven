 #include <18F2550.h>
 
 #device ADC=10

 #FUSES MCLR                     //Master Clear 
 #FUSES WDT                      //Watch Dog Timer
 #FUSES WDT2048                  //Watch Dog Timer uses 1:512 Postscale
 #FUSES PUT                      //Power Up Timer
 #FUSES BORV43                   //Brownout reset at 4.3V
 #FUSES NOVREGEN                 //USB voltage regulator disabled
 #FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used FOR I/O
 #FUSES NOPBADEN
 #FUSES PROTECT                  //Code protected from reads
 #FUSES CPUDIV1

 #use delay(INTERNAL=8000000)    //FRECUENCIA RELOJ DEL PIC(INTERNA)

 #define LCD_ENABLE_PIN    PIN_A6
 #define LCD_RS_PIN        PIN_A4
 #define LCD_RW_PIN        PIN_A5
 #define LCD_DATA4         PIN_C4
 #define LCD_DATA5         PIN_C5
 #define LCD_DATA6         PIN_C6
 #define LCD_DATA7         PIN_C7
 
 #define M1_PREC_TEMP_0      0x00
 #define M1_PREC_TEMP_1      0x01
 #define M1_CALENT_TEMP_0    0x02
 #define M1_CALENT_TEMP_1    0x03
 #define M1_REFUS_TEMP_0     0x04
 #define M1_REFUS_TEMP_1     0x05
 #define M1_ENF_TEMP_0       0x06
 #define M1_ENF_TEMP_1       0x07
 
 #define M1_PREC_TIME      0x08
 #define M1_CALENT_TIME    0x09
 #define M1_REFUS_TIME     0x0A
 #define M1_ENF_TIME       0x0B

 #USE SPI (MASTER, CLK=PIN_A0, DI=PIN_A1,ENABLE=PIN_B6,ENABLE_DELAY=2,BITS=16,stream=TP_1,mode=0,baud=9600)
 #USE SPI (MASTER, CLK=PIN_A0, DI=PIN_A1,ENABLE=PIN_B7,ENABLE_DELAY=2,BITS=16,stream=TP_2,mode=0,baud=9600)
 #USE RS232(baud=9600,parity=N,xmit=PIN_A3,rcv=PIN_A2,bits=8,stream=SERIAL_DATA)
 
 #USE PWM (pwm1,FREQUENCY=1000,stream=pwm1)
 #USE PWM (pwm2,FREQUENCY=1000,stream=pwm2)
 
 int8          counter=0,time=0;
 int8          reading=0;
 int8          mode=0;
 int16         prec,calent,refus,enf;     //temperaturas
 int8          tprec,tcalent,trefus,tenf; //iempos
 int1          flag=0;       
 int16         LCK[2];
 float         TMP[2];
 int16         temperature=0; //evaluar si es necesaria la coma flotante
 //variables del PID
 float min=0;                             //inicialización variables 
 float max=0;
 int16 control=0;
 //constantes del PID 
 const float a=0.1243;                              
 const float b=0.0062;
 const float c=0.6215;
 
 #include <lcd.c>
 #include <math.h>
 #include "tones.c"
 #include "funciones.h"
 
 #INT_RTCC
 void  RTCC_isr(void) {
   //hacer funcion para inicializar counter = time
   if(counter==0)flag=1;
   else { counter--; time++; }
 }
 
 void main(void){
   
   pwm_off(pwm1);
   pwm_off(pwm2);
   
   delay_ms(1000);//tiempo de espera
   
   lcd_init();
   
   //////////////////////////////////caracteres personalizados
   
   unsigned int8 grados[8];
   grados [0] = 0b00001110;
   grados [1] = 0b00001010;
   grados [2] = 0b00001110;
   grados [3] = 0b00000000;
   grados [4] = 0b00000000;
   grados [5] = 0b00000000;
   grados [6] = 0b00000000;
   grados [7] = 0b00000000;
   
   lcd_set_cgram_char(2,grados);
   
   //////////////////////////////////pantalla de inicio
   lcd_gotoxy(4,1);
   printf(lcd_putc,"%s","TO2CONTROL");
   lcd_gotoxy(5,2);
   printf(lcd_putc,"%s","FUSION-I");
   delay_ms(2000);
   lcd_putc("\f");
   
   ////////////////////////////////botones activos en LOW
   port_b_pullups(0b00111111); 
   ////////////////////////////////timer overflow 1.0s
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_32);      //1,0 s overflow
   
   disable_interrupts(GLOBAL);
   
   write_eeprom(M1_PREC_TEMP_0,0x00);
   write_eeprom(M1_PREC_TEMP_1,0x0064);
   write_eeprom(M1_CALENT_TEMP_0,0x00);
   write_eeprom(M1_CALENT_TEMP_1,0x0096);
   write_eeprom(M1_REFUS_TEMP_0,0x00);
   write_eeprom(M1_REFUS_TEMP_1,0x009B);
   write_eeprom(M1_ENF_TEMP_0,0x00);
   write_eeprom(M1_ENF_TEMP_1,0x0054);
   
   write_eeprom(M1_PREC_TIME,6);
   write_eeprom(M1_CALENT_TIME,5);
   write_eeprom(M1_REFUS_TIME,4);
   write_eeprom(M1_ENF_TIME,3);
   
      while(1){
      
            mode_insert();
            subroutine_mode();
            delay_ms(100); 
            precalentamiento();
            calentamiento();
            refusion();
            enfriamiento();
            over();
            restart();
            restart_wdt();
      }
 }
 
