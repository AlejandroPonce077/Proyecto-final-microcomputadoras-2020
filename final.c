#include <16f877a.h>
#fuses HS, NOPROTECT, NOLVP
#device ADC=10
#use delay(clock=20M)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
//LCD Module Connections
#define LCD_RS_PIN PIN_D7
#define LCD_RW_PIN PIN_D0
#define LCD_ENABLE_PIN PIN_D6
#define LCD_DATA4 PIN_D5
#define LCD_DATA5 PIN_D4
#define LCD_DATA6 PIN_D3
#define LCD_DATA7 PIN_D2
//End LCD Module Connections
#include <lcd.c>

#define nada   0
#define up     1
#define down   2
#define enter  3
#define back   4

char boton=0, estado=0, menu=1;

#INT_RB
void isr_rb(void)
{
   switch(input_b()&0xF0){
   case 0b01110000:
      boton=up;
      break;
   case 0b10110000:
      boton=down;
      break;
   case 0b11010000:
      boton=enter;
      break;
   case 0b11100000:
      boton=back;
      break;
   }
   delay_ms(20);
}


void estado0(void)   //Menu principal
{
   boton=nada;
   if(menu==1)
   {
      lcd_putc('\f');
      lcd_putc("* Temperatura   ");
      lcd_gotoxy(1,2);
      lcd_putc("  Voltimetro    ");
   }
   else if(menu==2)
   {
      lcd_putc('\f');
      lcd_putc("  Temperatura   ");
      lcd_gotoxy(1,2);
      lcd_putc("* Voltimetro    ");
   }
   else if(menu==3)
   {
      lcd_putc('\f');
      lcd_putc("  Voltimetro     ");
      lcd_gotoxy(1,2);
      lcd_putc("* Amperimetro   "); 
   }
   while(!boton);
   switch(boton){
   case up:
      menu--;
      if(menu<1) menu=1;
      break;
   case down:
      menu++;
      if(menu>3) menu=3;
      break;
   case enter:
      estado=menu;
      break;
   }
}


//Muestra la temperatura en grados Celsius, mediante el sensor LM35, que tiene
//una resolución de 10mV/ºC, sabemos que el CAD del pic entrega la conversión
//a 10 bits, además como tenemos como voltajes de referencia positivo y nega-
//tivo de 5 y 0 volts respectivamente, la temperatura se obtiene a partir de
//la lectura del CAD de la siguiente forma:
//
//    T[ºC]=(1 ºC / 10 mV)(5000 mV / 1023)(lectura del CAD)
//    
//    T[ºC]= 0.48876 * (lectura del CAD)
//
void estado1(void)
{
   float temperatura;
   boton=nada;
   set_adc_channel(0);
   delay_us(50);
   lcd_putc('\f');
   lcd_putc("  Temperatura:  ");
   do{
      temperatura= 0.48876 * (float) read_adc();
      lcd_gotoxy(1,2);
      printf(lcd_putc,"  %2.1f Celsius  ",temperatura);
      delay_ms(500);
      switch(boton){
      case up:
         break;
      case down:
         break;
      case enter:
         break;
      case back:
         menu=1;
         estado=0;
         break;
      }
   }while(boton!=back);
}

//Muestra el voltaje medido desde 0 hasta 20 volts. Debido a el pic tiene como
//límite de entrada hasta 5 volts se requiere de un divisor de voltaje como el 
//que se muestra:
//                        ------  0 - 20 V 
//                       |
//                       Z  6.8k
//  (PIC)   CADx --------|                 (Voltaje a medir)
//                       Z  2.2K
//                       |
//           GND ---------------  0 V
//
// Mediante la siguiente experesio se obtine el voltaje a partir de la lectura
// del CAD:
// 
//    Voltaje[V]=((6.8+2.2)/2.2)(5 V / 1023)(Lectura del CAD)
//
//    Voltaje[V]= 0.01999 (Lectura del CAD)
//
void estado2(void)
{
   float voltaje;
   boton=nada;
   set_adc_channel(1);
   delay_us(50);
   lcd_putc('\f');
   lcd_putc("    Voltaje:    ");
   do{
      voltaje= 0.01999 * (float) read_adc();
      lcd_gotoxy(1,2);
      printf(lcd_putc,"  %2.1f volts   ",voltaje);
      delay_ms(500);
      switch(boton){
      case up:
         break;
      case down:
         break;
      case enter:
         break;
      case back:
         menu=1;
         estado=0;
         break;
      }
   }while(boton!=back);
}

//Muestra la corriente en mA medida a través de una restencia de 1 ohm
//
//            CADx --------------- <-- ----   +V  -------
//                      |           i                    |
//       (PIC)          Z 1 ohm                       (FUENTE)
//                      |                                |
//             GND -----------|CARGA|------  GND  -------
//
// Asi la corriente medida sera directamente proporcional con un factor de uno
// al voltaje medido en la resistencia:
//
//    I[mA] = 4.8876(Lectura del CAD)
//
void estado3(void)
{
   float corriente;
   boton=nada;
   set_adc_channel(2);
   delay_us(50);
   lcd_putc('\f');
   lcd_putc("   Corriente:    ");
   do{
      corriente= 4.8876 * (float) read_adc();
      lcd_gotoxy(1,2);
      printf(lcd_putc,"%3.1f miliamperes  ",corriente);
      delay_ms(500);
      switch(boton){
      case up:
         break;
      case down:
         break;
      case enter:
         break;
      case back:
         menu=1;
         estado=0;
         break;
      }
   }while(boton!=back);
}

void main(void)
{
   lcd_init();
   delay_ms(100);
   lcd_putc('\f');
   lcd_putc(" Laboratorio de ");
   lcd_gotoxy(1,2);
   lcd_putc("Microcomputadora");
   delay_ms(1000);
   setup_adc_ports(AN0_AN1_AN2_AN3_AN4_AN5);
   setup_adc(ADC_CLOCK_INTERNAL);
   output_b(0);
   set_tris_b(0xF0);
   enable_interrupts(INT_RB);
   enable_interrupts(GLOBAL);
   while(TRUE)
   {
   switch(estado){
      case 0:
         estado0();  //Menu principal
         break;
      case 1:
         estado1();  //Muestra la temperatura
         break;
      case 2:
         estado2();  //Voltimetro
         break;
      case 3:
         estado3();  //Amperimetro
         break;
      }
   }
}

