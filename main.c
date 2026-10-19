#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "Pic32Mx3Conf.h"
#include "lcd.h"
#include "time_utils.h"
#include "utils.h"
#include "Buttons.h"
#include <sys/attribs.h>

int mod= 0;
volatile int tick = 0;
volatile int blink = 0;

void Init_Timer1(void) {
    T1CON = 0x0;             
    T1CONbits.TCKPS = 3;     
    PR1 = 1562;              
    
    IPC1bits.T1IP = 3;       
    IFS0bits.T1IF = 0;      
    IEC0bits.T1IE = 1;  
    
    T1CONbits.ON = 1;        
}

void __ISR(_TIMER_1_VECTOR, ipl3soft) Timer1Handler(void) {
    tick++;
    if (tick % 5 == 0) {
        blink = !blink;
    }
    IFS0bits.T1IF = 0;       
}


void Init_Buzzer(void) {

    TRISBCLR = 0x4000;  
    ANSELBCLR = 0x4000; 

    RPB14R = 0x0C; 

    T2CON = 0x0;           
    T2CONbits.TCKPS = 0;   
    PR2 = 39999;           

    OC1CON = 0x0;          
    OC1CONbits.OCTSEL = 0;
    OC1CONbits.OCM = 6;    
    OC1RS = 19999;      
    OC1R = 19999;
    
    T2CONbits.ON = 1;    
}

int main(int argc, char** argv) {
    
    LCD_Init();
    
    TRISASET = 0x8000;
    TRISFSET = 0x0001;
    TRISBSET = 0x0103;
    ANSELBCLR = 0x0103;
    
    tmelements_t ceas;
    RtcDateTime(&ceas, __DATE__, __TIME__);
    
    char textCeas[20];
    char textData[20];
    
    int alarm_h = 5, alarm_m = 30, alarm_on = 0, is_ringing = 0;
    
    Init_Buttons();
    Init_Buzzer();
    INTCONbits.MVEC = 1;          
    __builtin_enable_interrupts(); 
    Init_Timer1();                
    
    while(1){
        
        DelayAprox10Us(1000);
        Update_Buttons();
        
        // oprire alaram
        if (is_ringing == 1) {
            if (BtnFlag[BtnC] == Click || BtnFlag[BtnUp] == Click || BtnFlag[BtnDn] == Click) {
                is_ringing = 0; 
                BtnFlag[BtnC] = BtnIdle;
                BtnFlag[BtnUp] = BtnIdle;
                BtnFlag[BtnDn] = BtnIdle;
            }
        }
         
        //afisaj
            if (mod >= 7) {
            // afisaj alarma
            char textAlm[20];
            sprintf(textAlm, "ALARM      [%s] ", alarm_on ? "ON " : "OFF");
            LCD_WriteStringAtPos(textAlm, 0, 0);

            if (mod == 7 && blink == 1 && !(BtnFSMState[BtnUp] == stBtnLongPress || BtnFSMState[BtnDn] == stBtnLongPress)) {
                sprintf(textCeas, "  :%02d            ", alarm_m);
            } else if (mod == 8 && blink == 1 && !(BtnFSMState[BtnUp] == stBtnLongPress || BtnFSMState[BtnDn] == stBtnLongPress)) {
                sprintf(textCeas, "%02d:              ", alarm_h);
            } else if (mod == 9 && blink == 1) {
                sprintf(textCeas, "%02d:%02d      [   ] ", alarm_h, alarm_m); 
            } else {
                sprintf(textCeas, "%02d:%02d      [%s] ", alarm_h, alarm_m, alarm_on ? "ON " : "OFF");
            }
            LCD_WriteStringAtPos(textCeas, 1, 0);
            
        } else {
            // afisaj data
            if (mod == 4 && blink == 1 && !(BtnFSMState[BtnUp] == stBtnLongPress || BtnFSMState[BtnDn] == stBtnLongPress)) {
                sprintf(textData, "  /%02d/20%02d      ", ceas.Month, ceas.Year); 
            }
            else if (mod == 5 && blink == 1 && !(BtnFSMState[BtnUp] == stBtnLongPress || BtnFSMState[BtnDn] == stBtnLongPress)) {
                sprintf(textData, "%02d/  /20%02d      ", ceas.Day, ceas.Year); 
            }
            else if (mod == 6 && blink == 1 && !(BtnFSMState[BtnUp] == stBtnLongPress || BtnFSMState[BtnDn] == stBtnLongPress)) {
                sprintf(textData, "%02d/%02d/          ", ceas.Day, ceas.Month); 
            }
            else {
                sprintf(textData, "%02d/%02d/20%02d      ", ceas.Day, ceas.Month, ceas.Year); 
            }
            LCD_WriteStringAtPos(textData, 0, 0);
            
            //afisaj ora
            if (mod == 1 && blink == 1 && !(BtnFSMState[BtnUp] == stBtnLongPress || BtnFSMState[BtnDn] == stBtnLongPress)){
                sprintf(textCeas, "  :%02d:%02d        ", ceas.Minute, ceas.Second);
            }
            else if (mod == 2 && blink == 1 && !(BtnFSMState[BtnUp] == stBtnLongPress || BtnFSMState[BtnDn] == stBtnLongPress) ){
                sprintf(textCeas, "%02d:  :%02d        ", ceas.Hour, ceas.Second);
            }
            else if (mod == 3 && blink == 1 && !(BtnFSMState[BtnUp] == stBtnLongPress || BtnFSMState[BtnDn] == stBtnLongPress)){
                sprintf(textCeas, "%02d:%02d:          ", ceas.Hour, ceas.Minute);
            }
            else {
                sprintf(textCeas, "%02d:%02d:%02d        ", ceas.Hour, ceas.Minute, ceas.Second);
            }
            LCD_WriteStringAtPos(textCeas, 1, 0);
        }
        
        if(tick >= 100){
            tick = 0;
            uint32_t timp_total = makeTime(&ceas); 
            timp_total++;
            breakTime(timp_total, &ceas);

            if (alarm_on == 1 && ceas.Hour == alarm_h && ceas.Minute == alarm_m && ceas.Second == 0) {
                is_ringing = 1;
            }
        }
        
        // setari timp
        if (BtnFlag[BtnC] == Click) {
            if (mod == 0) {
                mod = 1; 
            } else {
                mod = 0; 
            }
            BtnFlag[BtnC] = BtnIdle;
        }
        
        if (mod > 0) {
            if (BtnFlag[BtnR] == Click) {
                mod++;
                if (mod > 9) mod = 1; 
                BtnFlag[BtnR] = BtnIdle;
            }
            if (BtnFlag[BtnL] == Click) {
                if (mod == 1) mod = 9; 
                else mod--;
                BtnFlag[BtnL] = BtnIdle;
            }
        }
        
        //incrementare ore
        if (mod == 1 && BtnFlag[BtnUp] == Click) { 
            ceas.Hour++;
            if (ceas.Hour >= 24) {
                ceas.Hour = 0;
            }
            BtnFlag[BtnUp] = BtnIdle; 
        }
        else if (mod == 1 && BtnFlag[BtnUp] == LPress) {
            ceas.Hour = ceas.Hour + 1;
            if (ceas.Hour >= 24) {
                ceas.Hour = ceas.Hour - 24;  
            }
            BtnFlag[BtnUp] = BtnIdle; 
        }
        
        //incrementare minute
        if (mod == 2 && BtnFlag[BtnUp] == Click) { 
            ceas.Minute++;
            if (ceas.Minute >= 60) {
                ceas.Minute = 0;
            }
            BtnFlag[BtnUp] = BtnIdle; 
        }
        else if (mod == 2 && BtnFlag[BtnUp] == LPress) {
            ceas.Minute = ceas.Minute + 1;
            if (ceas.Minute >= 60) {
                ceas.Minute = ceas.Minute - 60; 
            }
            BtnFlag[BtnUp] = BtnIdle; 
        }
        
        //incrementare secunde
        if (mod == 3 && BtnFlag[BtnUp] == Click) { 
            ceas.Second++;
            if (ceas.Second >= 60) {
                ceas.Second = 0;
            }
            BtnFlag[BtnUp] = BtnIdle; 
        }
        else if (mod == 3 && BtnFlag[BtnUp] == LPress) {
            ceas.Second = ceas.Second + 1;
            if (ceas.Second >= 60) {
                ceas.Second = ceas.Second - 60; 
            }
            BtnFlag[BtnUp] = BtnIdle; 
        }
        
        //decrementare ore
        if (mod == 1 && BtnFlag[BtnDn] == Click) {
            if (ceas.Hour == 0) {
                ceas.Hour = 23;
            } else {
                ceas.Hour--;
            }
            BtnFlag[BtnDn] = BtnIdle;
        }
        else if (mod == 1 && BtnFlag[BtnDn] == LPress) {
            if (ceas.Hour < 5) {
                ceas.Hour = ceas.Hour + 24 - 1;
            } else {
                ceas.Hour = ceas.Hour - 1;
            }
            BtnFlag[BtnDn] = BtnIdle;
        }
        
        //decrementare minute
        if (mod == 2 && BtnFlag[BtnDn] == Click) {
            if (ceas.Minute == 0) {
                ceas.Minute = 59;
            } else {
                ceas.Minute--;
            }
            BtnFlag[BtnDn] = BtnIdle;
        }
        else if (mod == 2 && BtnFlag[BtnDn] == LPress) {
            if (ceas.Minute < 10) {
                ceas.Minute = ceas.Minute + 60 - 1;
            } else {
                ceas.Minute = ceas.Minute - 1;
            }
            BtnFlag[BtnDn] = BtnIdle;
        }
        
        //decrementare secunde
        if (mod == 3 && BtnFlag[BtnDn] == Click) {
            if (ceas.Second == 0) {
                ceas.Second = 59;
            } else {
                ceas.Second--;
            }
            BtnFlag[BtnDn] = BtnIdle;
        }
        else if (mod == 3 && BtnFlag[BtnDn] == LPress) {
            if (ceas.Second < 10) {
                ceas.Second = ceas.Second + 60 - 1;
            } else {
                ceas.Second = ceas.Second - 1;
            }
            BtnFlag[BtnDn] = BtnIdle;
        }  

        //incrementare zi
        if (mod == 4 && BtnFlag[BtnUp] == Click) { 
            ceas.Day++; if (ceas.Day > 31) ceas.Day = 1; 
            BtnFlag[BtnUp] = BtnIdle; 
        } else if (mod == 4 && BtnFlag[BtnDn] == Click) {
            ceas.Day--; if (ceas.Day < 1) ceas.Day = 31;
            BtnFlag[BtnDn] = BtnIdle;
        }

        // incrementare luna
        if (mod == 5 && BtnFlag[BtnUp] == Click) { 
            ceas.Month++; if (ceas.Month > 12) ceas.Month = 1; 
            BtnFlag[BtnUp] = BtnIdle; 
        } else if (mod == 5 && BtnFlag[BtnDn] == Click) {
            ceas.Month--; if (ceas.Month < 1) ceas.Month = 12;
            BtnFlag[BtnDn] = BtnIdle;
        }

        // incrementare an
        if (mod == 6 && BtnFlag[BtnUp] == Click) { 
            ceas.Year++; if (ceas.Year > 99) ceas.Year = 0; 
            BtnFlag[BtnUp] = BtnIdle; 
        } else if (mod == 6 && BtnFlag[BtnDn] == Click) {
            if (ceas.Year == 0) ceas.Year = 99; else ceas.Year--;
            BtnFlag[BtnDn] = BtnIdle;
        }
        
        // ora alarma
        if (mod == 7 && BtnFlag[BtnUp] == Click) { 
            alarm_h++; if (alarm_h >= 24) alarm_h = 0; 
            BtnFlag[BtnUp] = BtnIdle; 
        } 
        else if (mod == 7 && BtnFlag[BtnUp] == LPress) {
            alarm_h = alarm_h + 1;
            if (alarm_h >= 24) alarm_h = alarm_h - 24;  
            BtnFlag[BtnUp] = BtnIdle; 
        }
        else if (mod == 7 && BtnFlag[BtnDn] == Click) {
            if (alarm_h == 0) alarm_h = 23; else alarm_h--;
            BtnFlag[BtnDn] = BtnIdle;
        }
        else if (mod == 7 && BtnFlag[BtnDn] == LPress) {
            if (alarm_h < 5) alarm_h = alarm_h + 24 - 1; 
            else alarm_h = alarm_h - 1;
            BtnFlag[BtnDn] = BtnIdle;
        }

        // minute alarma
        if (mod == 8 && BtnFlag[BtnUp] == Click) { 
            alarm_m++; if (alarm_m >= 60) alarm_m = 0; 
            BtnFlag[BtnUp] = BtnIdle; 
        }
        else if (mod == 8 && BtnFlag[BtnUp] == LPress) {
            alarm_m = alarm_m + 1;
            if (alarm_m >= 60) alarm_m = alarm_m - 60; 
            BtnFlag[BtnUp] = BtnIdle; 
        }
        else if (mod == 8 && BtnFlag[BtnDn] == Click) {
            if (alarm_m == 0) alarm_m = 59; else alarm_m--;
            BtnFlag[BtnDn] = BtnIdle;
        }
        else if (mod == 8 && BtnFlag[BtnDn] == LPress) {
            if (alarm_m < 10) alarm_m = alarm_m + 60 - 1; 
            else alarm_m = alarm_m - 1;
            BtnFlag[BtnDn] = BtnIdle;
        }

        // on/off alarma
        if (mod == 9 && (BtnFlag[BtnUp] == Click || BtnFlag[BtnDn] == Click)) { 
            alarm_on = !alarm_on;
            BtnFlag[BtnUp] = BtnIdle; 
            BtnFlag[BtnDn] = BtnIdle;
        }
        
        
        // control buzzer
        if (is_ringing == 1) {
            OC1CONbits.ON = 1; 
        } else {
            OC1CONbits.ON = 0; 
        }
    }

    return (EXIT_SUCCESS);
}

