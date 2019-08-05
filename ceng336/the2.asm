; Beyazit Yalcinkaya    2172138
; Orhan Asan            2231264
    
    
#include "p18f8722.inc"
; ##################################################################################################
; CONFIGURATION OF BOARD
; ##################################################################################################
    
; Taken from recitation 3 files
; CONFIG1H
  CONFIG  OSC = HSPLL, FCMEN = OFF, IESO = OFF
; CONFIG2L
  CONFIG  PWRT = OFF, BOREN = OFF, BORV = 3
; CONFIG2H
  CONFIG  WDT = OFF, WDTPS = 32768
; CONFIG3L
  CONFIG  MODE = MC, ADDRBW = ADDR20BIT, DATABW = DATA16BIT, WAIT = OFF
; CONFIG3H
  CONFIG  CCP2MX = PORTC, ECCPMX = PORTE, LPT1OSC = OFF, MCLRE = ON
; CONFIG4L
  CONFIG  STVREN = ON, LVP = OFF, BBSIZ = BB2K, XINST = OFF
; CONFIG5L
  CONFIG  CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF, CP4 = OFF, CP5 = OFF
  CONFIG  CP6 = OFF, CP7 = OFF
; CONFIG5H
  CONFIG  CPB = OFF, CPD = OFF
; CONFIG6L
  CONFIG  WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF, WRT4 = OFF
  CONFIG  WRT5 = OFF, WRT6 = OFF, WRT7 = OFF
; CONFIG6H
  CONFIG  WRTC = OFF, WRTB = OFF, WRTD = OFF
; CONFIG7L
  CONFIG  EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF, EBTR4 = OFF
  CONFIG  EBTR5 = OFF, EBTR6 = OFF, EBTR7 = OFF
; CONFIG7H
  CONFIG  EBTRB = OFF
  
; ##################################################################################################
; DECLARATION OF VARIABLES
; ##################################################################################################
  
arg_1			    udata 0x10
arg_1
      
arg_2			    udata 0x11
arg_2
      
total_number_of_asteroids   udata 0x12
total_number_of_asteroids
      
score_current_digit	    udata 0x13
score_current_digit
      
spaceship_position	    udata 0x14
spaceship_position
  
button_pressed		    udata 0x15 
button_pressed
    
random_seed		    udata 0x16
random_seed
    
number_of_asteroids	    udata 0x17
number_of_asteroids
  
temp			    udata 0x18
temp
      
complement_counter	    udata 0x19
complement_counter
      
tmr0_counter		    udata 0x1A
tmr0_counter
      
number_of_lasers	    udata 0x1B
number_of_lasers
      
temp_2			    udata 0x1C
temp_2
  
asteroids		    udata 0x20
asteroids
  
lasers			    udata 0x30
lasers

score_0			    udata 0x50
score_0

score_1			    udata 0x51
score_1

score_2			    udata 0x52
score_2

score_3			    udata 0x53
score_3

ss_flag			    udata 0x54
ss_flag

score_highest_digit	    udata 0x55
score_highest_digit

highest_asteroid_count	    udata 0x56
highest_asteroid_count
; (First bit is related to start button, Second bit is related to fire button, Third bit is related to
; up button and fourth bit is related to down button)
    
; ##################################################################################################
; DEFINITION OF VECTORS
; ##################################################################################################
  
; Reset vector
org 0x00
goto start
  
; High priority interrupt vector
org 0x08
goto hi_isr

; Low priority interrupt vector
org 0x18
goto lo_isr

; ##################################################################################################
; RESET VECTOR
; ##################################################################################################
  
org 0x30
start
  call init_reset
post_start
  call init
  call main

; Device on start
org 0x40
init_reset
  ; PORTA-PORTE<0:4> as output PORTA-PORTE<5:7> as input
  movlw B'11000000'
  movwf TRISA
  movwf TRISB
  movwf TRISC
  movwf TRISD
  movwf TRISE
  movwf TRISF
  ; PORTG<0:3> as input PORTG<4:7> as output
  movlw B'00001111'
  movwf TRISG
  ; Seven segment display's initial configuration
  clrf TRISJ
  clrf TRISH
  
  ; Set all inputs to digital
  ; Configuration of Timer0 and Timer1
  movwf ADCON1
  ; Configuration of Timer0 and Timer1
  movlw B'01010110'
  movwf T0CON
  ; Timer0 will be 16-bit, use internal clock (10 MHz instr. cycle) and have 1:128 prescale mode 
  ; (Since 2.5ms/100ns = 25000, 25000/128=195.3125~195. We will load TMR0 with 256 - 195 = 61 which is 0x3D) 
  ; (We can measure 2.5ms with Timer0)
  movlw B'00000001'
  movwf T1CON
  ; Start timer1 without any prescaler
  ; Configuration of interrupts (Enable interrupts and and enable TMR0 and TMR1 overflow interrupts)
  movlw B'11100000'
  movwf INTCON
  bsf PIE1, TMR1IE
  return
  
; After game is finished or device first started
org 0x90
init
  ; Clear all active LEDs
  clrf LATA
  clrf LATB
  clrf LATC
  clrf LATD
  clrf LATE
  clrf LATF
  clrf LATJ
  clrf LATH
  bsf LATH,3
  movlw 0x3F
  movwf LATJ
  ; Clear state symbols
  clrf button_pressed
  clrf number_of_lasers
  clrf number_of_asteroids
  clrf total_number_of_asteroids
  clrf complement_counter
  ; Spaceship created
  bsf LATA, 3
  movlw 0x13
  movwf spaceship_position
  ; Seven segment initialized to 0 and shown
  clrf score_0
  clrf score_1
  clrf score_2
  clrf score_3
  clrf ss_flag
  clrf score_highest_digit
  clrf score_current_digit
  clrf highest_asteroid_count
  clrf arg_1
  bsf PORTH, 3 
  clrf TMR0L
  bcf T0CON, 7
  movlw D'6'
  movwf temp
  movlw asteroids
  movwf FSR0L
  decf FSR0L
l1
  incf FSR0L
  clrf INDF0
  decfsz temp
  bra l1
; there can be 30 lasers max at the same time.
  movlw D'30'
  movwf temp
  movlw lasers
  movwf FSR0L
  decf FSR0L
l2
  incf FSR0L
  clrf INDF0
  decfsz temp
  bra l2
  return

org 0x120
main   
  ; Check if PORTG, 0 is pressed and released
  btfss PORTG, 0
  bra pressed_check_start
  bsf button_pressed, 0
  bra main    
pressed_check_start
  btfss button_pressed, 0
  bra main
  setf ss_flag
  bsf T0CON, 7
  movlw D'61'
  movwf TMR0L
; prepare tmr0
  clrf tmr0_counter
  clrf number_of_asteroids
; clear laser and asteroids array
; there can be 6 asteroids max at the same time.

  
game_loop
  tstfsz tmr0_counter
  bra counter_not_elapsed
  tstfsz ss_flag
  call update_seven_segment_display
  clrf LATA
  movf spaceship_position, 0
  addlw -0x10
  xorlw 0
  bz put_pos_0
  xorlw 0^1
  bz put_pos_1
  xorlw 1^2
  bz put_pos_2
  xorlw 2^3
  bz put_pos_3
  xorlw 3^4
  bz put_pos_4
  xorlw 4^5
  bz put_pos_5
clear_continue
  clrf LATB
  clrf LATC
  clrf LATD
  clrf LATE
  clrf LATF 
  call spaceship_check
  call collision_check
  call determine_highest_digit
  call move_asteroids
  call move_lasers
  call create_astreoid
  tstfsz highest_asteroid_count
  goto set_200ms
  movlw D'10'
  cpfsgt total_number_of_asteroids
  goto set_500ms
  movlw D'30'
  cpfsgt total_number_of_asteroids
  goto set_400ms
  movlw D'50'
  cpfsgt total_number_of_asteroids
  goto set_300ms
  goto set_200ms
finish_game_loop
  clrf temp_2
  clrf button_pressed
  bra game_loop
  
put_pos_0
  bsf LATA, 0
  bra clear_continue
put_pos_1
  bsf LATA, 1
  bra clear_continue
put_pos_2
  bsf LATA, 2
  bra clear_continue
put_pos_3
  bsf LATA, 3
  bra clear_continue
put_pos_4
  bsf LATA, 4
  bra clear_continue
put_pos_5
  bsf LATA, 5
  bra clear_continue
  

create_astreoid  
; Coppy the lower value of timer1 as random_seed 
  movff TMR1L, random_seed
; Get next asteroids location (the return value in WREG)
  call randomizer
  movwf temp
; Open LED in PORTF
  xorlw 0
  bz case_0
  xorlw 0^1
  bz case_1
  xorlw 1^2
  bz case_2
  xorlw 2^3
  bz case_3
  xorlw 3^4
  bz case_4
  xorlw 4^5
  bz case_5
  bra end_switch_case
case_0
  bsf LATF, 0
  bra end_switch_case
case_1
  bsf LATF, 1
  bra end_switch_case
case_2
  bsf LATF, 2
  bra end_switch_case
case_3
  bsf LATF, 3
  bra end_switch_case
case_4
  bsf LATF, 4
  bra end_switch_case
case_5
  bsf LATF, 5
end_switch_case
; Add astreoid to asteroids array
  movf temp,0
; BCD of place of astreoid
  addlw 0x60
  movwf temp  
  movlw asteroids
  movwf FSR0L
  decf FSR0L
  movlw D'6'
loop_add_astreoid
  INCF FSR0L
  addlw -D'1'
  bn add_astreoid
  tstfsz INDF0
  bra loop_add_astreoid
  bra add_astreoid
add_astreoid
  incf number_of_asteroids
  incf total_number_of_asteroids
  movff temp, INDF0
return
  
  
counter_not_elapsed
; check RGs.
; TODO: 7 segment
  tstfsz ss_flag
  call update_seven_segment_display
wait_rg1_press
  btfsc button_pressed, 1
  bra wait_rg1_release
  btfss PORTG, 1
  bra wait_rg2_press
  bsf button_pressed, 1
wait_rg1_release
  btfsc PORTG, 1
  bra wait_rg2_press
  bcf button_pressed, 1
  call fire_laser
wait_rg2_press
  btfsc button_pressed, 2
  bra wait_rg2_release
  btfss PORTG, 2
  bra wait_rg3_press
  bsf button_pressed, 2
wait_rg2_release
  btfsc PORTG, 2
  bra wait_rg3_press
  bcf button_pressed, 2
  call move_down
wait_rg3_press
  btfsc button_pressed, 3
  bra wait_rg3_release
  btfss PORTG, 3
  bra game_loop
  bsf button_pressed, 3
wait_rg3_release
  btfsc PORTG, 3
  bra game_loop
  bcf button_pressed, 3
  call move_up
  bra game_loop
    
  
; temp_2[0] = RB0, temp_2[1] = RB1, ..., temp_2[5] = RB5
fire_laser
  movf spaceship_position, 0
  addlw -0x10
  xorlw 0
  bz check_pos_0
  xorlw 0^1
  bz check_pos_1
  xorlw 1^2
  bz check_pos_2
  xorlw 2^3
  bz check_pos_3
  xorlw 3^4
  bz check_pos_4
  xorlw 4^5
  bz check_pos_5
fire_laser_continue
  movf spaceship_position, 0
  addlw 0x10
  movwf temp
  movlw lasers
  movwf FSR0L
  decf FSR0L
loop_add_laser
  incf FSR0L
  tstfsz INDF0
  bra loop_add_laser
add_laser
  incf number_of_lasers
  movff temp, INDF0
fire_laser_return
  return
  
check_pos_0
  btfsc temp_2, 0
  bra fire_laser_return
  bsf LATB, 0
  bsf temp_2, 0
  bra fire_laser_continue
check_pos_1
  btfsc temp_2, 1
  bra fire_laser_return
  bsf LATB, 1
  bsf temp_2, 1
  bra fire_laser_continue
check_pos_2
  btfsc temp_2, 2
  bra fire_laser_return
  bsf LATB, 2
  bsf temp_2, 2
  bra fire_laser_continue
check_pos_3
  btfsc temp_2, 3
  bra fire_laser_return
  bsf LATB, 3
  bsf temp_2, 3
  bra fire_laser_continue
check_pos_4
  btfsc temp_2, 4
  bra fire_laser_return
  bsf LATB, 4
  bsf temp_2, 4
  bra fire_laser_continue
check_pos_5
  btfsc temp_2, 5
  bra fire_laser_return
  bsf LATB, 5
  bsf temp_2, 5
  bra fire_laser_continue

 
move_down
  clrf LATA
  movf spaceship_position, 0
  addlw -0x10
  xorlw 0
  bz move_down_pos_0
  xorlw 0^1
  bz move_down_pos_1
  xorlw 1^2
  bz move_down_pos_2
  xorlw 2^3
  bz move_down_pos_3
  xorlw 3^4
  bz move_down_pos_4
  xorlw 4^5
  bz move_down_pos_5
move_down_return
  return
  
move_down_pos_0
  bsf LATA, 1
  movlw 0x11
  movwf spaceship_position
  bra move_down_return
move_down_pos_1
  bsf LATA, 2
  movlw 0x12
  movwf spaceship_position
  bra move_down_return
move_down_pos_2
  bsf LATA, 3
  movlw 0x13
  movwf spaceship_position
  bra move_down_return
move_down_pos_3
  bsf LATA, 4
  movlw 0x14
  movwf spaceship_position
  bra move_down_return
move_down_pos_4
  bsf LATA, 5
  movlw 0x15
  movwf spaceship_position
  bra move_down_return
move_down_pos_5
  bsf LATA, 5
  movlw 0x15
  movwf spaceship_position
  bra move_down_return
  
move_up
  clrf LATA
  movf spaceship_position, 0
  addlw -0x10
  xorlw 0
  bz move_up_pos_0
  xorlw 0^1
  bz move_up_pos_1
  xorlw 1^2
  bz move_up_pos_2
  xorlw 2^3
  bz move_up_pos_3
  xorlw 3^4
  bz move_up_pos_4
  xorlw 4^5
  bz move_up_pos_5
move_up_return
  return
  
move_up_pos_0
  bsf LATA, 0
  movlw 0x10
  movwf spaceship_position
  bra move_up_return
move_up_pos_1
  bsf LATA, 0
  movlw 0x10
  movwf spaceship_position
  bra move_up_return
move_up_pos_2
  bsf LATA, 1
  movlw 0x11
  movwf spaceship_position
  bra move_up_return
move_up_pos_3
  bsf LATA, 2
  movlw 0x12
  movwf spaceship_position
  bra move_up_return
move_up_pos_4
  bsf LATA, 3
  movlw 0x13
  movwf spaceship_position
  bra move_up_return
move_up_pos_5
  bsf LATA, 4
  movlw 0x14
  movwf spaceship_position
  bra move_up_return
  
  


; ##################################################################################################
; INTERRUPT SERVICE ROUTINES
; ##################################################################################################
  
org 0x800
hi_isr
  btfsc INTCON, TMR0IF
  bra tmr0_isr
  btfsc PIR1, TMR1IF
  bra tmr1_isr
  retfie

tmr0_isr
  bcf INTCON, TMR0IF
  decf tmr0_counter
  movlw D'61'
  movwf TMR0L
  setf ss_flag
; decrement tmr0_counter and set flag for 7 segment display
  retfie
  
tmr1_isr
  bcf PIR1, TMR1IF
  retfie  
  
org 0x900
lo_isr
  retfie  
  
; ##################################################################################################
; HELPER FUNCTIONS
; ##################################################################################################
  
; Returns seven segment bits of corresponding number (make sure arg_1 is loaded with any of 0..9)  
org 0xA00
seven_segment
  movf arg_1, 0
  xorlw 0
  bz ss_zero
  xorlw 0^1
  bz ss_one
  xorlw 1^2
  bz ss_two
  xorlw 2^3
  bz ss_three
  xorlw 3^4
  bz ss_four
  xorlw 4^5
  bz ss_five
  xorlw 5^6
  bz ss_six
  xorlw 6^7
  bz ss_seven
  xorlw 7^8
  bz ss_eight
  xorlw 8^9
  bz ss_nine
  bra ss_default
  
ss_zero
  retlw 0x3F
ss_one
  retlw 0x06
ss_two
  retlw 0x5B
ss_three
  retlw 0x4F
ss_four
  retlw 0x66
ss_five
  retlw 0x6D
ss_six
  retlw 0x7C
ss_seven
  retlw 0x07
ss_eight
  retlw 0x7F
ss_nine
  retlw 0x6F
ss_default
  retlw 0x0

; For taking random seed
randomizer
; If complement counter is greater than 9, clear it and complement random_seed
  movlw 0x09
  cpfsgt complement_counter
  bra randomizer_masking
  clrf complement_counter
  comf random_seed  
randomizer_masking
; Mask random_seed with 0x07 to get last 3 bits
  movlw 0x07
  andwf random_seed, 0
  movwf temp
  movlw 0x05
  cpfsgt temp
  bra randomizer_less_or_equal
  bra randomizer_greater

randomizer_greater
  movf temp, 0
  addlw -0x06
  bra randomizer_rest
randomizer_less_or_equal  
  movf temp, 0 
randomizer_rest
  rrncf random_seed
  incf complement_counter
  return

increment_score
  incf score_0
  movlw D'10'
  cpfseq score_0
  return
  clrf score_0
  incf score_1
  cpfseq score_1
  return
  clrf score_1
  incf score_2
  cpfseq score_2
  return
  clrf score_2
  incf score_3
  cpfseq score_3
  return
  clrf score_3
  return 
  
determine_highest_digit
  clrf WREG
  cpfseq score_3
  bra pre_return_set_high_as_3
  cpfseq score_2
  bra pre_return_set_high_as_2
  cpfseq score_1
  bra pre_return_set_high_as_1
  clrf score_highest_digit
  return
pre_return_set_high_as_3
  movlw D'3'
  movwf score_highest_digit
  return
pre_return_set_high_as_2
  movlw D'2'
  movwf score_highest_digit
  return
pre_return_set_high_as_1
  movlw D'1'
  movwf score_highest_digit
  return
  
; ##################################################################################################
; SUBSTEPS
; ##################################################################################################
move_asteroids
  movlw asteroids
  movwf FSR0L  
  movlw D'6'
loop_move_asteroids
  addlw -D'1'
  bn finish_move_asteroids
  movwf temp_2
  movf INDF0,0
  xorlw 0x0
  bz end_switch_case_asteroids
  movlw PORTJ
  movwf FSR1L
  movlw 0x0F
  movwf FSR1H
  movlw -D'16'
  addwf INDF0
  movlw 0x0F
  cpfsgt INDF0
; here check if it is out of bounds
  bra skip_astreoid
  movf INDF0, 0
; get least significant decimal point
  andlw 0x0F
  movwf temp
  movf INDF0, 0
; get most significant decimal point
  andlw 0xF0
; rotate left 4 times (i.e. divide by 16)
  rrncf WREG
  rrncf WREG
  rrncf WREG
  rrncf WREG
; add remaining size to LATA (if 0 it yields LATA, if 5 it yields LATF)
  addwf FSR1L
; move least significant decimal point to WREG and open LED
  movf temp,0
  xorlw 0
  bz case_0_ast
  xorlw 0^1
  bz case_1_ast
  xorlw 1^2
  bz case_2_ast
  xorlw 2^3
  bz case_3_ast
  xorlw 3^4
  bz case_4_ast
  xorlw 4^5
  bz case_5_ast
  bra end_switch_case_asteroids
skip_astreoid
  clrf INDF0
  incf FSR0L
  decf number_of_asteroids
  movf temp_2, 0
  bra loop_move_asteroids
case_0_ast
  bsf INDF1, 0
  bra end_switch_case_asteroids
case_1_ast
  bsf INDF1, 1
  bra end_switch_case_asteroids
case_2_ast
  bsf INDF1, 2
  bra end_switch_case_asteroids
case_3_ast
  bsf INDF1, 3
  bra end_switch_case_asteroids
case_4_ast
  bsf INDF1, 4
  bra end_switch_case_asteroids
case_5_ast
  bsf INDF1, 5
end_switch_case_asteroids
; Restore WREG value  
  incf FSR0L
  movf temp_2, 0
  bra loop_move_asteroids
finish_move_asteroids
  return
  
move_lasers 
  movlw lasers
  movwf FSR0L 
  movlw D'30'
loop_move_lasers
  addlw -D'1'
  bn finish_move_lasers
  movwf temp_2
  movf INDF0,0
  xorlw 0x0
  bz end_switch_case_lasers
  movlw PORTJ
  movwf FSR1L
  movlw 0x0F
  movwf FSR1H
  movlw D'16'
  addwf INDF0
; check if it is out of bounds
  movlw 0x66
  cpfslt INDF0
  bra skip_laser
  movf INDF0, 0
; get least significant decimal point
  andlw 0x0F
  movwf temp
  movf INDF0, 0
; get most significant decimal point
  andlw 0xF0
; rotate left 4 times (i.e. divide by 16)
  rrncf WREG
  rrncf WREG
  rrncf WREG
  rrncf WREG
; add remaining size to LATA (if 0 it yields LATA, if 5 it yields LATF)
  addwf FSR1L
; move least significant decimal point to WREG and open LED
  movf temp,0
  xorlw 0
  bz case_0_laz
  xorlw 0^1
  bz case_1_laz
  xorlw 1^2
  bz case_2_laz
  xorlw 2^3
  bz case_3_laz
  xorlw 3^4
  bz case_4_laz
  xorlw 4^5
  bz case_5_laz
  bra end_switch_case_lasers
skip_laser
  clrf INDF0
  incf FSR0L
  decf number_of_lasers
  movf temp_2, 0
  bra loop_move_lasers  
case_0_laz
  bsf INDF1, 0
  bra end_switch_case_lasers
case_1_laz
  bsf INDF1, 1
  bra end_switch_case_lasers
case_2_laz
  bsf INDF1, 2
  bra end_switch_case_lasers
case_3_laz
  bsf INDF1, 3
  bra end_switch_case_lasers
case_4_laz
  bsf INDF1, 4
  bra end_switch_case_lasers
case_5_laz
  bsf INDF1, 5
end_switch_case_lasers
; Restore WREG value  
  incf FSR0L
  movf temp_2, 0
  bra loop_move_lasers
finish_move_lasers
  return

collision_check
  movf number_of_asteroids, 0
  xorlw 0
  bz collision_check_return
  movf number_of_lasers, 0
  xorlw 0
  bz collision_check_return
  movlw D'6'
  movwf temp
  movlw asteroids
  movwf FSR0L
  decf FSR0L
loop_asteroids
  incf FSR0L
  tstfsz INDF0
  bra cont_loop_asteroids
  bra dec_temp
cont_loop_asteroids
  movlw D'30'
  movwf temp_2
  movlw lasers
  movwf FSR2L
  decf FSR2L
loop_lasers
  incf FSR2L
  tstfsz INDF2
  bra cont_loop_lasers
  bra dec_temp_2
cont_loop_lasers
  movf INDF2, 0
  cpfseq INDF0
  bra dec_temp_2
inc_score
  call increment_score
  clrf INDF0
  clrf INDF2
  decf number_of_lasers
  decf number_of_asteroids
dec_temp_2
  decfsz temp_2
  bra loop_lasers
dec_temp
  decfsz temp
  bra loop_asteroids
  bra collision_check2
collision_check_return
  return
  

collision_check2
  movf number_of_asteroids, 0
  xorlw 0
  bz collision_check_return
  movf number_of_lasers, 0
  xorlw 0
  bz collision_check_return
  movlw D'6'
  movwf temp
  movlw asteroids
  movwf FSR0L
  decf FSR0L
loop_asteroids2
  incf FSR0L
  tstfsz INDF0
  bra cont_loop_asteroids2
  bra dec_temp2
cont_loop_asteroids2
  movlw D'30'
  movwf temp_2
  movlw lasers
  movwf FSR2L
  decf FSR2L
loop_lasers2
  incf FSR2L
  tstfsz INDF2
  bra cont_loop_lasers2
  bra dec_temp_22
cont_loop_lasers2
  movf INDF2, 0
  addlw 0x10
  cpfseq INDF0
  bra dec_temp_22
inc_score2
  call increment_score
  clrf INDF0
  clrf INDF2
  decf number_of_lasers
  decf number_of_asteroids
dec_temp_22
  decfsz temp_2
  bra loop_lasers2
dec_temp2
  decfsz temp
  bra loop_asteroids2
  bra collision_check_return
 
  
spaceship_check
  movf number_of_asteroids, 0
  xorlw 0
  bz spaceship_check_return
  movlw D'6'
  movwf temp
  movlw asteroids
  movwf FSR0L
  decf FSR0L
loop
  incf FSR0L
  tstfsz INDF0
  bra cont_loop
  bra dec_tempp
cont_loop
  movf INDF0, 0
  cpfseq spaceship_position
  bra dec_tempp
  goto post_start
dec_tempp
  decfsz temp
  bra loop
spaceship_check_return
  return
  
  
  
update_seven_segment_display
  clrf ss_flag
  movf score_highest_digit, 0
  clrf LATH
  xorlw 0
  bz score_highest_0
  xorlw 0^1
  bz score_highest_1
  xorlw 1^2
  bz score_highest_2
  xorlw 2^3
  bz score_highest_3
  return
score_highest_0
  movff score_0, arg_1
  call seven_segment
  movwf LATJ
  bsf LATH,3
  return
score_highest_1
  tstfsz score_current_digit
  bra score_highest_1_0
  movff score_0, arg_1
  call seven_segment
  movwf LATJ
  bsf LATH,3 
  incf score_current_digit
  return
score_highest_1_0
  movff score_1, arg_1
  call seven_segment
  movwf LATJ
  bsf LATH,2
  clrf score_current_digit
  return
score_highest_2
  tstfsz score_current_digit
  bra score_highest_2_higher
  movff score_0, arg_1
  call seven_segment
  movwf LATJ
  bsf LATH,3 
  incf score_current_digit  
  return
score_highest_2_higher
  movf score_current_digit, 0
  addlw -D'1'
  bz score_highest_2_1
  bra score_highest_2_2  
score_highest_2_1
  movff score_1, arg_1
  call seven_segment
  movwf LATJ
  bsf LATH,2
  incf score_current_digit  
  return
score_highest_2_2
  movff score_2, arg_1
  call seven_segment
  movwf LATJ
  bsf LATH,1
  clrf score_current_digit  
  return
score_highest_3
  tstfsz score_current_digit
  bra score_highest_3_higher
  movff score_0, arg_1
  call seven_segment
  movwf LATJ
  bsf LATH,3 
  incf score_current_digit  
  return
score_highest_3_higher
  movf score_current_digit, 0
  addlw -D'1'
  bz score_highest_3_1
  addlw -D'1'
  bz score_highest_3_2
  bra score_highest_3_3  
score_highest_3_1
  movff score_1, arg_1
  call seven_segment
  movwf LATJ
  bsf LATH,2
  incf score_current_digit  
  return
score_highest_3_2
  movff score_2, arg_1
  call seven_segment
  movwf LATJ
  bsf LATH,1
  incf score_current_digit  
  return
score_highest_3_3
  movff score_3, arg_1
  call seven_segment
  movwf LATJ
  bsf LATH,0
  clrf score_current_digit  
  return
  
set_500ms
  movlw D'200'
  movwf tmr0_counter
  goto finish_game_loop
set_400ms
  movlw D'160'
  movwf tmr0_counter
  goto finish_game_loop
set_300ms
  movlw D'120'
  movwf tmr0_counter
  goto finish_game_loop
set_200ms
  movlw D'80'
  movwf tmr0_counter
  setf highest_asteroid_count
  goto finish_game_loop


end

  
  
  
  
  