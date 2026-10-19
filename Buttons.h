
/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef BUTTONS_H
#define	BUTTONS_H

 #define STABLE_TIME_TICKS 3 

 #define LONGPRESS_TICKS 150

 #define DBLCLICK_TIMEOUT_TICKS 30


enum Buttons {BtnC=0, BtnUp, BtnDn, BtnL, BtnR, NumButtons};

enum BtnFsmStates{stBtnIdle, stBtnPressed, stBtnClick, stBtnDblClick, stBtnLongPress};

enum BtnValues {BtnReleased=0, BtnPressed};

enum BtnFlagTypes {BtnIdle = 0, Click, DblClick, LPress, LpRelease};

extern volatile enum BtnFlagTypes BtnFlag[NumButtons];
extern volatile enum BtnFsmStates BtnFSMState[NumButtons];

void BtnFSM(enum Buttons Btn);
void Init_Buttons();
void Update_Buttons();




#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */
