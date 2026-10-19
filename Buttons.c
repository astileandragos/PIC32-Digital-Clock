#include "Buttons.h"
#include <xc.h>


volatile enum BtnFsmStates BtnFSMState[NumButtons];

volatile enum BtnValues BtnVal[NumButtons];

volatile enum BtnFlagTypes BtnFlag[NumButtons];

volatile unsigned short BtnTimeCount[NumButtons];


void Update_Buttons() {
    int i;
    uint8_t raw_state[NumButtons];
    static uint8_t debounce_cnt[NumButtons] = {0}; 
    
    raw_state[BtnC]  = PORTFbits.RF0;
    raw_state[BtnUp] = PORTBbits.RB1;
    raw_state[BtnDn] = PORTAbits.RA15;
    raw_state[BtnL]  = PORTBbits.RB0; 
    raw_state[BtnR]  = PORTBbits.RB8; 
    
    for (i = 0; i < NumButtons; i++) {
        
        BtnTimeCount[i]++; 
  
        if (raw_state[i] != BtnVal[i]) {
            debounce_cnt[i]++;
            if (debounce_cnt[i] >= STABLE_TIME_TICKS) {
                BtnVal[i] = raw_state[i]; 
                debounce_cnt[i] = 0;
                BtnTimeCount[i] = 0; 
            }
        } else {
            debounce_cnt[i] = 0;
        }
        
        BtnFSM((enum Buttons)i);
    }
}

void BtnFSM(enum Buttons Btn){
    switch (BtnFSMState[Btn]){
    case stBtnIdle: if (BtnVal[Btn] == BtnPressed) 
                        BtnFSMState[Btn] = stBtnPressed;
                    break;
    case stBtnPressed: if (BtnVal[Btn] != BtnPressed){
                            BtnFSMState[Btn] = stBtnClick;
                            BtnFlag[Btn] = Click;
                        }
                      else
                        if(BtnTimeCount[Btn] == LONGPRESS_TICKS){
                               BtnFSMState[Btn] = stBtnLongPress;
                               BtnFlag[Btn] = LPress;
                        }
                    break;
    case stBtnClick: if (BtnTimeCount[Btn] >= DBLCLICK_TIMEOUT_TICKS)
                                BtnFSMState[Btn] = stBtnIdle;
                         else if(BtnVal[Btn] == BtnPressed){
                             BtnFSMState[Btn] = stBtnDblClick;
                             BtnFlag[Btn] = DblClick;
                         }
                         break;
    case stBtnDblClick: if (BtnVal[Btn] == BtnReleased)
                        BtnFSMState[Btn] = stBtnIdle;
                        break;
    case stBtnLongPress: 
            if (BtnVal[Btn] == BtnReleased) {
                BtnFSMState[Btn] = stBtnIdle;
                BtnFlag[Btn] = LpRelease;
            } else if (BtnTimeCount[Btn] % 10 == 0) { 
                BtnFlag[Btn] = LPress;
            }
            break;
    default : break;
    } 
}

void Init_Buttons(){
    enum Buttons Btn;

    for (Btn = BtnC; Btn < NumButtons; Btn++){
        BtnFSMState[Btn]= stBtnIdle;
        BtnVal[Btn]=BtnReleased;
        BtnFlag[Btn]=BtnIdle;
        BtnTimeCount[Btn]=0;
    } 
    
}


