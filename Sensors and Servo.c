//Connections
//P2.1 ECHO
//P1.6 TRIGGER yellow

//GREEN  P1.0 BUZZER ------> P1.7
//YELLOW P1.1 PIR SENSOR ---> P1.5
//ORANGE P1.2  SERVO--------> P2.6

#include <msp430.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


volatile int temp[2];
volatile float diff;
volatile unsigned int i=0;
int dst_int;
int close = 0;
//test
int active =0;
int active2 =0;
int movement =0;

int dst_flt;
float tmp_flt;
char dst_char[5];
char dst_flt_char[5];
volatile float distance;
void ser_output(char *str);


//Accelerometer
//Blue   -  X axis   P1.2
//Green  -  Y axis   P1.1
//Yellow -  Z axis   P1.0
#include <msp430.h>
unsigned int xaccel =  0, xaccelroom =0;
unsigned int yaccel =  0, yaccelroom =0;
unsigned int zaccel =  0, zaccelroom =0;
unsigned int ADCReading [3];
unsigned int tempaccel = 0;
// Function Prototypes
void ConfigureAdc(void);
void getanalogvalues();
//End Accelerometer


void main(void)
{

    //PIR Sensor
    WDTCTL = WDTPW + WDTHOLD;       // stop watch dog timer (WDT)

    //PIR SENSOR
    P1OUT = 0;                      // set default values to all zeros for port 1
    P1DIR = 0;                      // set port direction to all inputs for port 1

    P1REN = 0;                      // disable pull up/down resistors for port 1
    P2REN = 0;                      // disable pull up/down resistors for port 2

    P1DIR |= (BIT7 | BIT6 );        //Set BIT7(led/buzzer)     BIT1(PIR/Motion Sensor)
    //P1OUT &= ~BIT0;               //turn output LED & BUZZER OFF

    //Servo Motor
    BCSCTL1= CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    //

    //Ultrasonic Sensor
    BCSCTL1= CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    // P1DIR = BIT6;
    //
    P2OUT = 0;                                    // set default values to all zeros for port 1
    P2DIR = ( BIT0 | BIT2 | BIT3 | BIT6 | BIT4);  //Set BIT0(YellowlED/AUDIO)  BIT2 (GreenLED)   BIT3(RedLED)
                                                  //BIT6 (Servo)  BIT4 (Buzzer Accelerometer)
    P2OUT &~ BIT0;                                //turn output LED OFF

    P2SEL = BIT1;
    P1SEL = BIT6;
    //P1SEL2 = BIT1|BIT2;
    UCA0CTL1 |= UCSWRST+UCSSEL_2;
    UCA0BR0 = 52;
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS_0;
    UCA0CTL1 &= ~UCSWRST;
    TA0CTL = TASSEL_2|MC_1 ;
    TA0CCR0 = 0xFFFF;
    TA0CCR1 = 0x000A;
    TA0CCTL1 = OUTMOD_7;
    TA1CTL = TASSEL_2|MC_2 ;
    TA1CCTL1 = CAP | CCIE | CCIS_0 | CM_3 | SCS ;
    _enable_interrupts();


    //Accelerometer
    ConfigureAdc();
    // reading the initial position values,xaccelroom, yaccelroom, zaccelroom
    __delay_cycles(250);
    getanalogvalues();
    xaccelroom = xaccel; yaccelroom = yaccel; zaccelroom = zaccel;
    __delay_cycles(250);


    while(1){

        //ACTIVE SIGNAL FROM OTHER MSP
        if( (P1IN & BIT4) == 0){
            active = 0; //OFF

         }else{
            active = 1; //ON
         }


        //DOOR//
        if(active == 0){

            //OPEN DOOR
            P2SEL   |= BIT6;             // P2.6 = TA1 output Enabling PWM Signal

            TACCR0 = 20000;              //PWM period
            TACCR1 = 700;                //CCR1 PWM Duty Cycle  !min 350 max 2600 angle 190,
                                         //350 2350-180 degrees
            TACCTL1 = OUTMOD_7;          //CCR1 selection reset-set
            TACTL = TASSEL_2|MC_1;       //SMCLK submain clock,upmode
            __delay_cycles(1500000);

            P2SEL   &= ~BIT6;            // Disabling PWM Signal
            //OPEN DOOR ENDED

            //LED
            P2OUT |= BIT2; //Green LED on
            P2OUT &= ~BIT3; //Red LED off

        }else{
            //CLOSE DOOR
            P2SEL   |= BIT6;    // P2.6 = TA1 output Enabling PWM Signal

            TACCR0 = 20000;  //PWM period
            TACCR1 = 2600;
            TACCTL1 = OUTMOD_7;  //CCR1 selection reset-set
            TACTL = TASSEL_2|MC_1;
            __delay_cycles(1500000);

             P2SEL   &= ~BIT6;  // Disabling PWM Signal
            //CLOSE Door Ended


             //LED
             P2OUT &= ~BIT2; //Green LED off
             P2OUT |= BIT3; //Red LED on

        }




        //Ultrasonic Sensor
        distance = diff/58;
        if(distance < 10)
        {
           close = 1;
           P2OUT  |= BIT0;
        }  else{
            close = 0;
            P2OUT &=    ~BIT0;
        }


        //PIR SENSOR
        __delay_cycles(1000);
        //PIR Sensor
        if((P1IN & BIT5) == 0){
            P1OUT &= ~BIT7;

        }
        else{
            P1OUT |= BIT7;
        }




        //Accelerometer
        getanalogvalues();

        if( (xaccel > (xaccelroom + 9)) | (xaccel < (xaccelroom - 9)) |
                (yaccel > (yaccelroom + 9)) | (yaccel < (yaccelroom - 9)) |
                        (zaccel > (zaccelroom + 9)) | (zaccel < (zaccelroom - 9)) ){
            tempaccel =1;

        } else{
            tempaccel=0;
        }


        //Accelerometer
        if(tempaccel && active){
            P2OUT |= BIT4;
        }else if( active == 0 ){
            P2OUT &= ~BIT4;
        }


    }   //End of while loop
}


//Accelerometer
void ConfigureAdc(void)
{
   ADC10CTL1 = INCH_2 | CONSEQ_1;                // A2 + A1 + A0, single sequence 
   ADC10CTL0 = ADC10SHT_2 | MSC | ADC10ON;
   while (ADC10CTL1 & BUSY);
   ADC10DTC1 = 0x03;                             // 3 conversions
   ADC10AE0 |= (BIT0 | BIT1 | BIT2);             // ADC10 option select
}


//Acelerometer
void getanalogvalues()
{
    ADC10CTL0 &= ~ENC;
    while (ADC10CTL1 & BUSY);                     //Wait while ADC is busy
    ADC10SA = (unsigned)&ADCReading;              //RAM Address of ADC Data, must be reset every conversion
    ADC10CTL0 |= (ENC | ADC10SC);                 //Start ADC Conversion
    while (ADC10CTL1 & BUSY);                     //Wait while ADC is busy
    xaccel = ADCReading[0];                       // sum  all 5 reading for the three variables
    yaccel = ADCReading[1];
    zaccel = ADCReading[2];
}

//Accelerometer
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    __bic_SR_register_on_exit(CPUOFF);
}



#pragma vector = TIMER1_A1_VECTOR
__interrupt void Timer_A(void){
        temp[i] = TA1CCR1;
        i += 1;
        TA1CCTL1 &= ~CCIFG ;
        if (i==2) {
            diff=temp[i-1]-temp[i-2];
            i=0;
        }

}


