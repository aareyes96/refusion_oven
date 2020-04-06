 /////////////////////////////////////////////////////////////////////////////////////
 // serial_connection(void)     ==>> toma los valores                              	//
 //                                  de subrutinas desde el terminal             	//
 // mode_insert(void)           ==>> selecciona el modo                      		//
 //                                  y muestra solicitud en lcd                  	//
 // read_tmp(void)              ==>> lee la temperatura y la almacena en TMP[2]     //
 //                                  y detecta si el termopar esta abierto         	//
 // subroutine_mode(void)       ==>> lee las variables de la eeprom y              	//
 //                                  las pone en la RAM segun el modo            	//
 // tono (int8 who)             ==>> genera un tono:                         		//
 //                                  0:tono error                           		//
 //                                  1:tono afirmativo                        		//
 //                                  2:tono inicio de refusion                  	//
 //                                  3:tono fin de refusion                     	//
 //                                  4:tono boton presionado                  		//
 // PID (int1 zone)             ==>> controla la temperatura por PID             	//   
 //                                  zone=0: zona 1                         		//
 //                                  zone=1: zona 2                           		//
 // precalentamiento(void)      ==>> controla etapa de precalentamiento             //
 //                                                               					//
 // calentamiento(void)         ==>> controla etapa de calentamiento            	//
 //                                                               					//
 // refusion(void)              ==>> controla etapa de refusion                  	//
 //                                                               					//
 // enfriamiento(void)          ==>> controla etapa de enfriamiento                	//
 //                                                               					//
 // over(void)                  ==>> espera a que se enfrie el horno a menos de    	//
 //                                  50 grados celcius                         		//
 // restart(void)               ==>> resetea todas las variables               		//
 //                                                               					//
 /////////////////////////////////////////////////////////////////////////////////////
 
 ////////////////////////////////////tonos y sonidos
 void tono (int8 who){
    
   if( who == 0 ){
      restart_wdt();
      generate_tone(500,125);
      delay_ms(200);
   }
   if( who == 1 ){
      restart_wdt();
      generate_tone(E_NOTE[0],100);
      generate_tone(B_NOTE[0],80);
   }
   if( who == 2 ){
      restart_wdt();
      generate_tone(600,125);
      delay_ms(100);
      generate_tone(600,125);
   }
   if( who == 3 ){
      restart_wdt();
      generate_tone(600,225);
      delay_ms(400);
      generate_tone(600,225);
      delay_ms(400);
      generate_tone(600,225);
      delay_ms(400);
      generate_tone(600,225);
   }
   if(who == 4 ){
      restart_wdt();
      generate_tone(600,125);
   }
   restart_wdt();
 }
 /////////////////////////tomar valores d subrutinas desde el terminal
 /*void serial_connection(void){
    
   restart_wdt();
   
   int8  times=0;
   int16 temp=0;
   int8  rx[2];
   char  tt=0; 
   
   int8 mode1[12];
   int8 mode2[12];
   int8 mode3[12];
   int8 mode4[12];
   int8 mode5[12];
   
   restart_wdt();
   gets(rx,SERIAL_DATA);
   
   if( rx[0] == 0x31 && rx[1] == 0x39 ){
      
      for(int i=0;i<5;i++){
      
         restart_wdt();
         puts("MODE??\n",SERIAL_DATA);
         gets(tt,SERIAL_DATA);
         restart_wdt();
         
         if( tt == '1' )gets(mode1,SERIAL_DATA);
         if( tt == '2' )gets(mode2,SERIAL_DATA);
         if( tt == '3' )gets(mode3,SERIAL_DATA);
         if( tt == '4' )gets(mode4,SERIAL_DATA);
         if( tt == '5' )gets(mode5,SERIAL_DATA);
         restart_wdt();
      }
    }
   
   
   
    restart_wdt();
 }*/
 ////////////////////////////////// SELECION DE MODO
 void mode_insert(void){
   
   restart_wdt();
   
   signed int8 i=16;
   int8 pos=1;
   char selec[] = "SELECCION=>MODO";
   char modo[]  = "     MODO ";
   int1 flag=1;
   
   lcd_putc("\f");
   
   while(1){ 
   
      lcd_gotoxy(i,1);
      printf(lcd_putc,"%s","ELIJA EL MODO DE REFUSION ");
      delay_ms(100);
      if(i<-32)i=16;
      reading=input_b();
      if( ( reading & 0b00011100 ) != 0b00011100 ){tono(4);lcd_putc("\f");break;}
      i--;
      restart_wdt();
      
   }
   
    while( bit_test(reading,3) ){
      
      if(reading == 0b00001000) {
      
         //serial_connection();
         printf(lcd_putc,"%s","   *CONECTADO* ");
         
      }
     
      if( !bit_test(reading,2) && flag ){
         
         tono(4);
         pos += 1;
         if( pos > 6 ) pos=6; 
         if( pos < 1 ) pos=1;
         
         flag=0;
      }
      if( !bit_test(reading,4) && flag ) {
         
         tono(4);
         pos -= 1;
         if( pos > 6 ) pos=6; 
         if( pos < 1 ) pos=1;
         
         flag=0;
      }
      
      lcd_putc("\a");
      lcd_gotoxy(1,1);
      printf(lcd_putc,"%s",selec);
      lcd_gotoxy(1,2);
      printf(lcd_putc,"%s%u",modo,pos);
      
      reading=input_b();
      if( bit_test(reading,2) && bit_test(reading,4) ) flag=1;
      
     restart_wdt();
   }
   
   mode=pos;
   tono(1);
   
   restart_wdt();
 }
//////////////////////////////////LEER TEMPERATURA
 void read_tmp(void){
    
   for(int i=0;i<3;i++){
   
      LCK[0]=spi_xfer(TP_1,0);
      delay_us(1);
     
      LCK[1]=spi_xfer(TP_2,0);
     delay_us(1);
   }
   
  if(bit_test(LCK[0],2)){printf(lcd_putc,"%s","\nTCK 1 ABIERTO!!!");while(1){tono(0);delay_ms(500);};}
  if(bit_test(LCK[1],2)){printf(lcd_putc,"%s","\nTCK 2 ABIERTO!!!");while(1){tono(0);delay_ms(500);};}
   
   TMP[0]=(LCK[0]>>3)/4.000;
   TMP[1]=(LCK[1]>>3)/4.000;
   
   restart_wdt();
 }
 //////////////////////////leer variables de subrutinas en la eeprom 
  void subroutine_mode(){
   restart_wdt();
    int8 a=0,b=0;
   if(mode==1){
      a     =  read_eeprom(0x0000);
      b     =  read_eeprom(0x0001);
      prec=make16(a,b);
      a     =  read_eeprom(0x0002);
      b     =  read_eeprom(0x0003);
      calent=make16(a,b);
      a     =  read_eeprom(0x0004);
      b     =  read_eeprom(0x0005);
      refus=make16(a,b);
      a     =  read_eeprom(0x0006);
      b     =  read_eeprom(0x0007);
      enf=make16(a,b);
      tprec=   read_eeprom(0x08);
      tcalent= read_eeprom(0x09);
      trefus=  read_eeprom(0x0A);
      tenf=    read_eeprom(0x0B);
    }
   if(mode==2){
      a     =  read_eeprom(0x0010);
      b     =  read_eeprom(0x0011);
      prec=make16(a,b);
      a     =  read_eeprom(0x0012);
      b     =  read_eeprom(0x0013);
      calent=make16(a,b);
      a     =  read_eeprom(0x0014);
      b     =  read_eeprom(0x0015);
      refus=make16(a,b);
      a     =  read_eeprom(0x0016);
      b     =  read_eeprom(0x0017);
      enf=make16(a,b);
      tprec=   read_eeprom(0x18);
      tcalent= read_eeprom(0x19);
      trefus=  read_eeprom(0x1A);
      tenf=    read_eeprom(0x1B);
    }
   if(mode==3){
      a     =  read_eeprom(0x0020);
      b     =  read_eeprom(0x0021);
      prec=make16(a,b);
      a     =  read_eeprom(0x0022);
      b     =  read_eeprom(0x0023);
      calent=make16(a,b);
      a     =  read_eeprom(0x0024);
      b     =  read_eeprom(0x0025);
      refus=make16(a,b);
      a     =  read_eeprom(0x0026);
      b     =  read_eeprom(0x0027);
      enf=make16(a,b);
      tprec=   read_eeprom(0x28);
      tcalent= read_eeprom(0x29);
      trefus=  read_eeprom(0x2A);
      tenf=    read_eeprom(0x2B);
    }
   if(mode==4){
      a     =  read_eeprom(0x0030);
      b     =  read_eeprom(0x0031);
      prec=make16(a,b);
      a     =  read_eeprom(0x0032);
      b     =  read_eeprom(0x0033);
      calent=make16(a,b);
      a     =  read_eeprom(0x0034);
      b     =  read_eeprom(0x0035);
      refus=make16(a,b);
      a     =  read_eeprom(0x0036);
      b     =  read_eeprom(0x0037);
      enf=make16(a,b);
      tprec=   read_eeprom(0x38);
      tcalent= read_eeprom(0x39);
      trefus=  read_eeprom(0x3A);
      tenf=    read_eeprom(0x3B);
    }
   if(mode==5){
      a     =  read_eeprom(0x0040);
      b     =  read_eeprom(0x0041);
      prec=make16(a,b);
      a     =  read_eeprom(0x0042);
      b     =  read_eeprom(0x0043);
      calent=make16(a,b);
      a     =  read_eeprom(0x0044);
      b     =  read_eeprom(0x0045);
      refus=make16(a,b);
      a     =  read_eeprom(0x0046);
      b     =  read_eeprom(0x0047);
      enf=make16(a,b);
      tprec=   read_eeprom(0x48);
      tcalent= read_eeprom(0x49);
      trefus=  read_eeprom(0x4A);
      tenf=    read_eeprom(0x4B);
    }
   if(mode==6){
      a     =  read_eeprom(0x0050);
      b     =  read_eeprom(0x0051);
      prec=make16(a,b);
      a     =  read_eeprom(0x0052);
      b     =  read_eeprom(0x0053);
      calent=make16(a,b);
      a     =  read_eeprom(0x0054);
      b     =  read_eeprom(0x0055);
      refus=make16(a,b);
      a     =  read_eeprom(0x0056);
      b     =  read_eeprom(0x0057);
      enf=make16(a,b);
      tprec=   read_eeprom(0x58);
      tcalent= read_eeprom(0x59);
      trefus=  read_eeprom(0x5A);
      tenf=    read_eeprom(0x5B);
    }
   
     lcd_putc("\f");
     lcd_gotoxy(5,1);
     printf(lcd_putc,"%s%u","MODO: ",mode);
     delay_ms(1000);
     restart_wdt();
 }
 //////////////////////////////////////control de la temperatura en tiempo real
 void PID (int1 zone){
    
     float rT,eT,iT,dT,yT,uT,iT0,eT0;
    
     min=0;
     max=1000;
    
     read_tmp();                  		//leer temperatura
     yT=TMP[zone];         
  
     rT=temperature;                    
     eT=rT-yT;                          //Cálculo error        
     iT=b*eT+iT0;                       //Cálculo del término integral
     dT=c*(eT-eT0);                     //Cálculo del término derivativo 
     uT=iT+a*eT+dT;                     //Cálculo de la salida PID
  
     if ( uT > max ) uT=max;            //Salida PID si es mayor que el MAX                    
     if ( uT < min ) uT=min;            //Salida PID si es menor que el MIN 
                                                                
     control=uT;                        //Transferencia de salida PID a señal PWM
    
     if(!zone)set_pwm1_duty(control);
     if(zone)set_pwm2_duty(control);
    
     iT0=iT;                            //Guardar variables
     eT0=eT;
    
     restart_wdt();                                                            
     delay_ms(100);  
 }
 ///////////////////////////////////generar la curva de temperatura
 //////////////////precalentamiento
 void precalentamiento(void){
    restart_wdt();
    float temp_actual=0;
    int16 temp_max=prec;
    float temp_0=0;
    signed int8 i=16;
   
    time=0;
   
    counter=tprec;
    delay_ms(100);
    read_tmp();
    temp_0=(TMP[0]+TMP[1])/2;
    
    if(temp_0 >= 50 ){
        lcd_putc("\f");
        while(1){   
         lcd_gotoxy(i,1);
         tono(0);
         printf(lcd_putc,"%s","LA TEMPERATURA ES DEMASIADO ALTA PARA INICIAR ");
         delay_ms(100);
         if(i<-44)i=16;
       
         restart_wdt();
         
         read_tmp();
         temp_0=(TMP[0]+TMP[1])/2;
       
         if(temp_0 <= 50)break;
         i--;
         restart_wdt();
        }
    }
     
    pwm_on(pwm1);
    pwm_on(pwm2);
    
     tono(2);
     lcd_putc("\f");
     lcd_gotoxy(1,1);
     printf(lcd_putc,"%s","PRECALENTAMIENTO");
     enable_interrupts(INT_RTCC);
     enable_interrupts(GLOBAL);
     while(!flag){
      temperature = ((temp_max - temp_0)/tprec) * time;
      PID(0);
      PID(1);
      
      temp_actual=(TMP[0]+TMP[1])/2;
      lcd_gotoxy(1,2);
      printf(lcd_putc,"%f%c%c",temp_actual,2,'C');
	  lcd_gotoxy(11,2);
      printf(lcd_putc,"%s%u%s","T:",counter,"s");
      restart_wdt();
     }
     disable_interrupts(INT_RTCC);
     disable_interrupts(GLOBAL);
     flag=0;
     restart_wdt();
 }
 ///////calentamiento
 void calentamiento(void){
     restart_wdt();
     float temp_actual=0;
     int16 temp_max=calent;
     counter=tcalent;
     time=0;
    
     read_tmp();
     float temp_0=(TMP[0]+TMP[1])/2;
    
     lcd_putc("\f");
     lcd_gotoxy(1,1);
     printf(lcd_putc,"%s"," CALENTAMIENTO");
     enable_interrupts(INT_RTCC);
     enable_interrupts(GLOBAL);
     while(!flag){
      temperature = ((temp_max - temp_0)/tcalent) * time;
      PID(0);
      PID(1);
      
      temp_actual=(TMP[0]+TMP[1])/2;
      lcd_gotoxy(1,2);
      printf(lcd_putc,"%f%c%c",temp_actual,2,'C');
	  lcd_gotoxy(11,2);
      printf(lcd_putc,"%s%u%s","T:",counter,"s");
      restart_wdt();
     }
     disable_interrupts(INT_RTCC);
     disable_interrupts(GLOBAL);
     flag=0;
     restart_wdt(); 
 }
 ///////////////////////////refusion
 void refusion(void){
     restart_wdt(); 
     float temp_actual=0;
     int16 temp_max=refus;
     counter=trefus;
     time=0;
   
     read_tmp();
     float temp_0=(TMP[0]+TMP[1])/2;
    
     lcd_putc("\f");
     lcd_gotoxy(1,1);
     printf(lcd_putc,"%s","    REFUSION");
     enable_interrupts(INT_RTCC);
     enable_interrupts(GLOBAL);
     while(!flag){
      temperature = ((temp_max - temp_0)/trefus) * time;
      PID(0);
      PID(1);
      
      temp_actual=(TMP[0]+TMP[1])/2;
      lcd_gotoxy(1,2);
      printf(lcd_putc,"%f%c%c",temp_actual,2,'C');
	  lcd_gotoxy(11,2);
      printf(lcd_putc,"%s%u%s","T:",counter,"s");
      restart_wdt();
     }
     disable_interrupts(INT_RTCC);
     disable_interrupts(GLOBAL);
     flag=0;
     restart_wdt();
 }
 ///////////////////////////enfriamiento
 void enfriamiento(void){
     restart_wdt();
    
     float temp_actual=0;
     int16 temp_min=enf;
     counter=tenf;
     time=0;
     
    output_high(PIN_B5);
    
     read_tmp();
     float temp_0=(TMP[0]+TMP[1])/2;
    
     lcd_putc("\f");
     lcd_gotoxy(1,1);
     printf(lcd_putc,"%s","  ENFRIAMIENTO");
     enable_interrupts(INT_RTCC);
     enable_interrupts(GLOBAL);
     while(!flag){
      temperature = (( temp_0 - temp_min )/tenf) * time;
      PID(0);
      PID(1);
    
      temp_actual=(TMP[0]+TMP[1])/2;
      lcd_gotoxy(1,2);
      printf(lcd_putc,"%f%c%c",temp_actual,2,'C');
	  lcd_gotoxy(11,2);
      printf(lcd_putc,"%s%u%s","T:",counter,"s");
      restart_wdt();
     }
     disable_interrupts(INT_RTCC);
     disable_interrupts(GLOBAL);
     flag=0;
     restart_wdt();
 }
 ///////////////////////////resetear todas las variables
 void restart(void){
 
   counter=0;
   time=0;
   reading=0;
   mode=0;
   prec=0; calent=0; refus=0; enf=0;     
   tprec=0;tcalent=0;trefus=0;tenf=0; 
   flag=0;       
   LCK[2]=(0,0);
   TMP[2]=(0,0);
   temperature=0;
   //variables del PID
   min=0;                           
   max=0;
   control=0;
   
   restart_wdt();
 }
 ////////////////////////normaliza a temperatura ambiente y termina el proceso
 void over(void){
 
     restart_wdt();
    
     pwm_off(pwm1);
     pwm_off(pwm2);    

     float temp_0=0;
     read_tmp();
     temp_0=(TMP[0]+TMP[1])/2;
     lcd_putc("\f");
    
     while(1){
    
      read_tmp();
        temp_0=(TMP[0]+TMP[1])/2;
      
      if( temp_0 <= 50 ) break;
      
      lcd_gotoxy(1,1);
      printf(lcd_putc,"%s","NORMALIZANDO... ");
      lcd_gotoxy(1,2);
      printf(lcd_putc,"%f",temp_0);
    }
    
    output_low(PIN_B5);
   
     lcd_putc("\f");
     lcd_gotoxy(1,1);
     printf(lcd_putc,"%s","FIN DE REFUSION ");
     tono(3);
     delay_ms(2000);
     restart_wdt();
 }
