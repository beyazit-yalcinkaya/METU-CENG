LIST P=18F8722

#INCLUDE <p18f8722.inc> 
    
CONFIG OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF


delay_counter1 udata 0x20
delay_counter1

delay_counter2 udata 0x21
delay_counter2
 
delay_counter3 udata 0x22
delay_counter3
 
pressed udata 0x23 ; pressed[0] := ra4, pressed[3] := re3, pressed[4] := re4 
pressed


org 0x00
goto main

org 0x08
retfie

org 0x18
retfie


init
    clrf pressed
    clrf LATA
    clrf LATB
    clrf LATC
    clrf LATD
    movlw b'11111111'
    movwf ADCON1
    movlw b'11110000'
    movwf TRISA
    movwf TRISB
    movwf TRISC
    movwf TRISD
    movlw b'00011000'
    movwf TRISE
    movlw b'00001111'
    movwf LATA
    movwf LATB
    movwf LATC
    movwf LATD
    call delay_500
    call delay_500
    call delay_500
    call delay_500
    clrf LATA
    clrf LATB
    clrf LATC
    clrf LATD
    call delay_500
    call delay_500
    return


delay_500 ; 500 ms delay
    movlw b'00011011'
    movwf delay_counter1
    movlw b'01011111'
    movwf delay_counter2
    movlw b'00000011'
    movwf delay_counter3
    nop
for1:
    decfsz delay_counter1
    goto for2
    return
for2:
    decfsz delay_counter2
    goto for3
    goto for1
for3:
    decfsz delay_counter3
    goto for3
    goto for2


wait_and_check_ra4 ; if there is no interupt, 500 ms delay
    movlw b'00001010'
    movwf delay_counter1
    movlw b'10000100'
    movwf delay_counter2
    movlw b'00011010'
    movwf delay_counter3
    nop
f1:
    decfsz delay_counter1
    goto f2
    return
f2:
    decfsz delay_counter2
    goto f3
    goto f1
f3:
    decfsz delay_counter3
    goto check_if_pressed
    nop
    goto f2
check_if_pressed:
    btfsc pressed, 0
    goto check_if_released
    btfss PORTA, 4
    goto f3
    bsf pressed, 0
check_if_released:
    btfsc PORTA, 4
    goto f3
    bcf pressed, 0
    clrf STKPTR ; To avoid stack overflow
    goto state2
    
    
set_ra0
    bsf LATA, 0
    call wait_and_check_ra4
    return
set_ra1
    bsf LATA, 1
    call wait_and_check_ra4
    return
set_ra2
    bsf LATA, 2
    call wait_and_check_ra4
    return
set_ra3
    bsf LATA, 3
    call wait_and_check_ra4
    return
set_rb0
    bsf LATB, 0
    call wait_and_check_ra4
    return
set_rb1
    bsf LATB, 1
    call wait_and_check_ra4
    return
set_rb2
    bsf LATB, 2
    call wait_and_check_ra4
    return
set_rb3
    bsf LATB, 3
    call wait_and_check_ra4
    return
set_rc0
    bsf LATC, 0
    call wait_and_check_ra4
    return
set_rc1
    bsf LATC, 1
    call wait_and_check_ra4
    return
set_rc2
    bsf LATC, 2
    call wait_and_check_ra4
    return
set_rc3
    bsf LATC, 3
    call wait_and_check_ra4
    return
set_rd0
    bsf LATD, 0
    call wait_and_check_ra4
    return
set_rd1
    bsf LATD, 1
    call wait_and_check_ra4
    return
set_rd2
    bsf LATD, 2
    call wait_and_check_ra4
    return
set_rd3
    bsf LATD, 3
    call wait_and_check_ra4
    return
    
    
clear_ra0
    bcf LATA, 0
    call wait_and_check_ra4
    return
clear_ra1
    bcf LATA, 1
    call wait_and_check_ra4
    return
clear_ra2
    bcf LATA, 2
    call wait_and_check_ra4
    return
clear_ra3
    bcf LATA, 3
    call wait_and_check_ra4
    return
clear_rb0
    bcf LATB, 0
    call wait_and_check_ra4
    return
clear_rb1
    bcf LATB, 1
    call wait_and_check_ra4
    return
clear_rb2
    bcf LATB, 2
    call wait_and_check_ra4
    return
clear_rb3
    bcf LATB, 3
    call wait_and_check_ra4
    return
clear_rc0
    bcf LATC, 0
    call wait_and_check_ra4
    return
clear_rc1
    bcf LATC, 1
    call wait_and_check_ra4
    return
clear_rc2
    bcf LATC, 2
    call wait_and_check_ra4
    return
clear_rc3
    bcf LATC, 3
    call wait_and_check_ra4
    return
clear_rd0
    bcf LATD, 0
    call wait_and_check_ra4
    return
clear_rd1
    bcf LATD, 1
    call wait_and_check_ra4
    return
clear_rd2
    bcf LATD, 2
    call wait_and_check_ra4
    return
clear_rd3
    bcf LATD, 3
    call wait_and_check_ra4
    return
    

wait_ra4_press:
    btfsc pressed, 0
    goto wait_ra4_release
    btfss PORTA, 4
    goto wait_ra4_press
    bsf pressed, 0
wait_ra4_release:
    btfsc PORTA, 4
    goto wait_ra4_release
    bcf pressed, 0
    goto state2


wait_re3_press:
    btfsc pressed, 3
    goto wait_re3_release
    btfss PORTE, 3
    goto wait_re4_press
    bsf pressed, 3
wait_re3_release:
    btfsc PORTE, 3
    goto wait_re4_press
    bcf pressed, 3
    goto state1
wait_re4_press:
    btfsc pressed, 4
    goto wait_re4_release
    btfss PORTE, 4
    goto wait_re3_press
    bsf pressed, 4
wait_re4_release:
    btfsc PORTE, 4
    goto wait_re3_press
    bcf pressed, 4
    goto state3

    
state1:
    btfss LATA, 0
    call set_ra0
    btfss LATA, 1
    call set_ra1
    btfss LATA, 2
    call set_ra2
    btfss LATA, 3
    call set_ra3   
    btfss LATB, 0
    call set_rb0
    btfss LATB, 1
    call set_rb1
    btfss LATB, 2
    call set_rb2
    btfss LATB, 3
    call set_rb3   
    btfss LATC, 0
    call set_rc0
    btfss LATC, 1
    call set_rc1
    btfss LATC, 2
    call set_rc2
    btfss LATC, 3
    call set_rc3    
    btfss LATD, 0
    call set_rd0
    btfss LATD, 1
    call set_rd1
    btfss LATD, 2
    call set_rd2
    btfss LATD, 3
    call set_rd3
    goto wait_ra4_press
    

state2:
    goto wait_re3_press
    

state3:
    btfsc LATD, 3
    call clear_rd3
    btfsc LATD, 2
    call clear_rd2
    btfsc LATD, 1
    call clear_rd1
    btfsc LATD, 0
    call clear_rd0    
    btfsc LATC, 3
    call clear_rc3
    btfsc LATC, 2
    call clear_rc2
    btfsc LATC, 1
    call clear_rc1
    btfsc LATC, 0
    call clear_rc0
    btfsc LATB, 3
    call clear_rb3
    btfsc LATB, 2
    call clear_rb2
    btfsc LATB, 1
    call clear_rb1
    btfsc LATB, 0
    call clear_rb0
    btfsc LATA, 3
    call clear_ra3
    btfsc LATA, 2
    call clear_ra2
    btfsc LATA, 1
    call clear_ra1
    btfsc LATA, 0
    call clear_ra0
    goto wait_ra4_press


main:
    call init
    goto state1
end


