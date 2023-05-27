#include <msp430.h>

int pass1[10]={0,0,0,0,0,0,0,0,0,0};  //Store password
int pass2[10]={0,0,0,0,0,0,0,0,0,0};  //input password
int count1=0;  //number of characters in password 1 (max 10)
int count2=0;  //number of characters in password 2 (max 10)
int EnterNewPass=0;   //1 if system has password stored
int EnterPass=0;  // 1 if system has password stored
int control=0;  //0 alarm disabled, 1 password saved, 2 alarm enabled, 3 password entered,
int flag=0;  //passwords don't match, 1 passwords match

#define DR P2OUT = P2OUT | BIT4 // define RS high
#define CWR P2OUT = P2OUT & (~BIT4) // define RS low]5
#define ENABLE_HIGH P2OUT = P2OUT | BIT6 // define Enable high signal
#define ENABLE_LOW P2OUT = P2OUT & (~BIT6) // define Enable Low signal
unsigned int i;
unsigned int j;
void delay(unsigned int k)
{
    for(j=0;j<=k;j++)
    {
        for(i=0;i<100;i++);
    }
}
void data_write(void)
{
    ENABLE_HIGH;
    delay(2);
    ENABLE_LOW;
}

void data_read(void)
{
    ENABLE_LOW;
    delay(2);
    ENABLE_HIGH;
}

void check_busy(void)
{
    P2DIR &= ~(BIT3); // make P2.3 as input
    while((P2IN&BIT3)==1)
    {
        data_read();
    }
    P2DIR |= BIT3; // make P2.3 as output
}

void send_command(unsigned char cmd)
{
    check_busy();
    //WRITE;
    CWR;
    P2OUT = (P2OUT & 0xF0)|((cmd>>4) & 0x0F); // send higher nibble
    data_write(); // give enable trigger
    P2OUT = (P2OUT & 0xF0)|(cmd & 0x0F); // send lower nibble
    data_write(); // give enable trigger
}

void send_data(unsigned char data)
{
    check_busy();
    //WRITE;
    DR;
    P2OUT = (P2OUT & 0xF0)|((data>>4) & 0x0F); // send higher nibble
    data_write(); // give enable trigger
    P2OUT = (P2OUT & 0xF0)|(data & 0x0F); // send lower nibble
    data_write(); // give enable trigger
}

void send_string(char *s)
{
    while(*s)
        {
            send_data(*s);
            s++;
        }
}

void lcd_init(void)
{
    P2OUT &= 0x00;
    send_command(0x33);
    send_command(0x32);
    send_command(0x28); // 4 bit mode
    send_command(0x0E); // clear the screen
    send_command(0x01); // display on cursor on
    send_command(0x06); // increment cursor
    send_command(0x80); // row 1 column 1
}


int counter=0;
unsigned int i;

int main(void)
{

    WDTCTL = WDTPW + WDTHOLD; // stop watchdog timer
    TACCTL1 = OUTMOD_7;            // TACCR1 reset/set
    TACTL   = TASSEL_2 + MC_1;     // SMCLK, upmode


    P2DIR |= 0x5F;
    P2SEL=0x00;
    P2SEL2=0x00;
    P2REN=0xA0;
    P2OUT &= 0x00;

    P1DIR=0x3F;
    P1REN=0xC0;
    P1OUT =0x00;

    lcd_init();
    send_string("Security By");
    send_command(0xC0);
    send_string("Team 25");

    while(1){
        //debounce delay
       __delay_cycles(150000);

       //*******1st Row
       //1
       P1OUT =P1OUT | 0x02;

       if((P1IN & 0x40)==0x40)
       {
           P1OUT ^=0x01; //0000 0001
           __delay_cycles(1000000);
           P1OUT ^=0x01;

           if(EnterNewPass==1 && count1<10)
             {
                 pass1[count1]=1;
                 count1++;
             }
           else if(EnterPass==1 && count2<10)
               {
                   pass2[count2]=1;
                   count2++;
               }

      }
       //2
       else if((P1IN & 0x80)==0x80)
           {
               P1OUT ^=0x01; //0000 0001
               __delay_cycles(1000000);
              P1OUT ^=0x01;

              if(EnterNewPass==1 && count1<10)
                  {
                       pass1[count1]=2;
                       count1++;
                  }

             else if(EnterPass==1 && count2<10)
                   {
                       pass2[count2]=2;
                       count2++;
                   }

            }



          P1OUT &= (~0x02);

         //2nd row*******************
         //4
         P1OUT =P1OUT | 0x08;

         if((P1IN & 0x40)==0x40)
         {
             P1OUT ^=0x01; //0000 0001
             __delay_cycles(1000000);
             P1OUT ^=0x01;

             if(EnterNewPass==1 && count1<10)
                 {
                     pass1[count1]=4;
                     count1++;
                 }
             else if(EnterPass==1 && count2<10)
               {
                   pass2[count2]=4;
                   count2++;
               }

        }
         //5
         else if((P1IN & 0x80)==0x80)
            {
                 P1OUT ^=0x01; //0000 0001
                  __delay_cycles(1000000);
                  P1OUT ^=0x01;

                  if(EnterNewPass==1 && count1<10)
                    {
                        pass1[count1]=5;
                        count1++;
                    }
                   else if(EnterPass==1 && count2<10)
                      {
                          pass2[count2]=5;
                          count2++;
                      }

                }
          P1OUT &= (~0x08);

          //*******3st Row
          //7
          P1OUT =P1OUT | 0x10;

          if((P1IN & 0x40)==0x40)
          {
              P1OUT ^=0x01; //0000 0001
              __delay_cycles(1000000);
              P1OUT ^=0x01;

              if(EnterNewPass==1 && count1<10)
                 {
                     pass1[count1]=7;
                     count1++;
                 }
             else if(EnterPass==1 && count2<10)
               {
                   pass2[count2]=7;
                   count2++;
               }

         }
          //8
          else if((P1IN & 0x80)==0x80)
             {
                  P1OUT ^=0x01; //0000 0001
                  __delay_cycles(1000000);
                  P1OUT ^=0x01;
                  if(EnterNewPass==1 && count1<10)
                      {
                          pass1[count1]=8;
                          count1++;
                      }
                  else if(EnterPass==1 && count2<10)
                    {
                        pass2[count2]=8;
                        count2++;
                    }

             }

           P1OUT &= (~0x10);
           //*******4th Row
           ///*
           P1OUT =P1OUT | 0x20;

           if((P1IN & 0x40)==0x40 && control==0)
             {
               P1OUT ^=0x01; //0000 0001
               __delay_cycles(1000000);
               P1OUT ^=0x01;

               control=2;
               EnterNewPass=0;
               lcd_init();
               send_string("Alarm Enabled");
               send_command(0xC0);
               send_string("C to Disable");

               P1OUT |=  BIT2;  //send signal alarm enabled

             }
            //0
           else if((P1IN & 0x80)==0x80)
           {
                   P1OUT ^=0x01; //0000 0001
                  __delay_cycles(1000000);
                  P1OUT ^=0x01;

                  if(EnterNewPass==1 && count1<10)
                  {
                        pass1[count1]=0;
                        count1++;
                  }
                  else if(EnterPass==1 && count2<10)
                  {
                      pass2[count2]=0;
                      count2++;
                  }
             }

         P1OUT &= (~0x20);

    //*********************************************************************************

    //*********************************************************************************
         //debounce delay
        __delay_cycles(150000);
        //*******1st Row
        //3
        P1OUT =P1OUT | 0x02;

        if((P2IN & 0x20)==0x20)
        {
            P1OUT ^=0x01; //0000 0001
            __delay_cycles(1000000);
            P1OUT ^=0x01;

            if(EnterNewPass==1 && count1<10)
                {
                  pass1[count1]=3;
                  count1++;
                }
            else if(EnterPass==1 && count2<10)
              {
                  pass2[count2]=3;
                  count2++;
              }
         }
        //A
         else if((P2IN & 0x80)==0x80 && control==0)
            {
                 P1OUT ^=0x01; //0000 0001
                 __delay_cycles(1000000);
                 P1OUT ^=0x01;

                 lcd_init();
                 send_string("Enter New Pass-");
                 send_command(0xC0);
                 send_string("Maximum [10]-");

                 control=1;
                 EnterNewPass=1;
                 EnterPass=0;
                 count1=0;

                  //cleaning the password array to 0
                  int num=0;
                  for(num;num<10;num++)
                  {
                      pass1[num]=0;
                  }

           }

         P1OUT &= (~0x02);
         //*******1st Row
         //6
         P1OUT =P1OUT | 0x08;

         if((P2IN & 0x20)==0x20)
         {
             P1OUT ^=0x01; //0000 0001
             __delay_cycles(1000000);
             P1OUT ^=0x01;

             if(EnterNewPass==1 && count1<10)
                 {
                     pass1[count1]=6;
                     count1++;
                 }
             else if(EnterPass==1 && count2<10)
              {
                  pass2[count2]=6;
                  count2++;
              }

         }
          //B
          else if((P2IN & 0x80)==0x80 && control==1)
             {
                 P1OUT ^=0x01; //0000 0001
                 __delay_cycles(1000000);
                 P1OUT ^=0x01;

                 lcd_init();
                 send_string("New Password Set");
                 __delay_cycles(1000000);

                 control=2;
                 EnterNewPass=0;



                 lcd_init();
                 send_string("Alarm Enabled");
                 send_command(0xC0);
                 send_string("C to Disable");

                 P1OUT |=  BIT2;  //send signal alarm enabled


                }

          P1OUT &= (~0x08);

          //*******1st Row
          //9
          P1OUT =P1OUT | 0x10;

           if((P2IN & 0x20)==0x20)
               {
                   P1OUT ^=0x01; //0000 0001
                   __delay_cycles(1000000);
                   P1OUT ^=0x01;

                   if(EnterNewPass==1 && count1<10)
                     {
                         pass1[count1]=9;
                         count1++;
                     }
                   else if(EnterPass==1 && count2<10)
                     {
                       pass2[count2]=9;
                       count2++;
                     }

                  }
           //C
                   else if((P2IN & 0x80)==0x80 && control==2)
                     {
                         P1OUT ^=0x01; //0000 0001
                         __delay_cycles(1000000);
                         P1OUT ^=0x01;

                         lcd_init();
                         send_string("Enter Password");
                         send_command(0xC0);
                         send_string("& Press D");

                         control=3;
                         EnterNewPass=0;
                         EnterPass=1;
                         count2=0;

                         //cleaning the password array to 0
                         int num=0;

                         for(num;num<10;num++)
                             {
                                 pass2[num]=0;
                             }
                      }

           P1OUT &= (~0x10);

           //*******4st Row
           //#
           P1OUT =P1OUT | 0x20;

           if((P2IN & 0x20)==0x20)
           {
               P1OUT ^=0x01; //0000 0001
               __delay_cycles(1000000);
               P1OUT ^=0x01;
          }
           //D
           else if((P2IN & 0x80)==0x80 && control==3)
              {
                   P1OUT ^=0x01; //0000 0001
                   __delay_cycles(1000000);
                   P1OUT ^=0x01;

                   if(count1 !=count2)
                     {
                         flag=0;
                     }
                     //checking all values
                   else
                     {
                         int chk=0;
                         for (chk;chk<10;chk++)
                         {
                             if(pass1[chk]!=pass2[chk])
                               {
                                     flag=0;
                                     break;
                               }
                             else
                              {
                                 flag=1;
                              }
                         }
                     }

                     if(flag==1)
                        {
                             control=0;

                             lcd_init();
                             send_string("Correct Password");

                             P1OUT &= ~BIT2;  //send signal alarm disabled
                             __delay_cycles(1000000);

                             lcd_init();
                             send_string("Alarm Disabled");
                             send_command(0xC0);
                             send_string("*-ENBL A-NEWPASS");


                          }
                     else
                     {
                         control=2;

                         lcd_init();
                         send_string("Wrong Password!");
                         send_command(0xC0);
                         send_string("C To Try Again");
                         //light red led 5 times
                         int b=5;
                         while(b>0)
                         {
                               P1OUT^=0x01;
                               __delay_cycles(1000000);
                               b--;
                         }
                         P1OUT &= (~0x01);
                     }
              }

                  P1OUT &= (~0x20);

    }
    return 0;
}
