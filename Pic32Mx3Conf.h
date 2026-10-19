/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    config_bits.h

  @Summary
    Brief description of the file.

  @Description
    Bitii de configurare ale microcontrolerului
 */
/* ************************************************************************** */

#ifndef _CONFIG_BITS_H    /* Guard against multiple inclusion */
#define _CONFIG_BITS_H


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

    // 'C' source line config statements


    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif


// PIC32MX370F512L Configuration Bit Settings



#pragma config JTAGEN = OFF     
#pragma config FWDTEN = OFF      

/* ------------------------------------------------------------ */
/*						Configuration Bits		                */
/* ------------------------------------------------------------ */
// Device Config Bits in  DEVCFG1:	
#pragma config FNOSC =	FRCPLL
#pragma config FSOSCEN =	OFF
#pragma config POSCMOD =	XT
#pragma config OSCIOFNC =	ON
#pragma config FPBDIV =     DIV_2

// Device Config Bits in  DEVCFG2:	
#pragma config FPLLIDIV =	DIV_2
#pragma config FPLLMUL =	MUL_20
#pragma config FPLLODIV =	DIV_1

#endif /* _CONFIG_BITS_H */

/* *****************************************************************************
 End of File
 */
