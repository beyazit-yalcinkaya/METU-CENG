/*
 *
 * Beyazıt Yalçınkaya   2172138
 * Orhan Asan           2231264
 * 
 */

#include <xc.h>
#pragma config OSC = HSPLL, FCMEN = OFF, IESO = OFF, PWRT = OFF, BOREN = OFF, WDT = OFF, MCLRE = ON, LPT1OSC = OFF, LVP = OFF, XINST = OFF, DEBUG = OFF

#define _XTAL_FREQ   40000000

#include "Includes.h"
#include "lcd.h"

/* -----------------------------------------------------------------------------
   --------------------------- GENERAL STRUCTURE OF PROGRAM --------------------
   -----------------------------------------------------------------------------
   
   Here we applied round robin approach with interrupts that consists of 5 states.
   These steps are given as functions such as state1() etc. 
   STATES:
   State 1: It waits for RE1 by using polling and when it is triggered it waits for
   3 seconds by using Timer1 ISR after printing entrance string to the LCD. Then, 
   our program is went to state 2.
   State 2: Here we wait for number of tens digit of potential voters. Control is 
   done through RB Port on Change interrupt. Similar to first step we wait for
   appropriate input to continue with. RB6 increases tens digit. After RB7 is pressed,
   we proceed to step 3.
   Step 3: Similar to step 2 but here we control units digit of potential voters. After
   user presses RB7, we proceed to state 4. Before proceeding, writing to LCD is done as
   a preparation.
   Step 4: At very beginning of step 4 and step 5, we update seven segments to show the 
   candidate with highest vote. State 4 corresponds to the voting phase. Here we control
   and change blink status (which is set through Timer0 ISR), ADC status (to show current
   candidate and avoid rewritting same candidate over and over), buttons (to increase the 
   vote of corresponding candidate) and also LCD (it shows current candidate with respect 
   to current candidate index changed through ADC ISR). This phase persists for exactly 90
   seconds (again controlled by Timer0 ISR) and when 90 seconds are elapsed we proceed to
   step 5.
   Step 5: Here, voting procedure is stopped but we can look for each candidates votes by
   setting the potentiometer. Candidates and their votes are printed onto LCD screen with
   the help of ADC ISR where we look for if candidate is changed after last reading, and 
   if it is not, we simply ignore the remaining tasks. If so, we print current candidate's
   name and vote by using its index set through ADC ISR.

   ISRs:
   Timer0 ISR: We have used Timer0 with 8-bit mode and 1:256 prescaler and to count 5 ms, 
   we preload it with 61 ((256 - 61) * 256 * 0.1 us = 4.992 ms). Rest is standard interrupt
   configuration for timers. It also counts for 100 ms and 250 ms by using two extra counters.
   Here we also trigger ADC by setting GO after each 100 ms.
   Timer1 ISR: We have used Timer1 with 16 bit mode and 1:8 prescaler and to count 50 ms 
   (actually we can count 1 ms without using any arbitrary counter but to be exact 50ms is
   needed), we loaded Timer0 with 3036 ((65536 - 3036) * 8 * 0.1 us = 50 ms). Rest is pretty
   trivial, we count 20 times to elapse 1 seconds.
   PortB Interrupt on Change ISR: Here we set pressed and toggled status of RB6 and RB7 by
   using a 2 byte array for each. 
   ADC ISR: When conversion is done, we read the value of potentiometer and change the current
   candidate index with respect to it. Here, we also check whether candidate is changed or not
   after last reading to minimize writings into LCD.

   Utility functions:
   updateSevenSegment(): It updates seven segment when it is called and it does so in a circular
   fashion. After update is done to 4 digits, it returns.
   get7SegmentValues(unsigned char): It returns hexadecimal values for corresponding value.
 */

unsigned char tmr1_cnt_50ms = 0;
unsigned char tmr1_cnt_1s = 0;
unsigned char state = 1;
unsigned char seven_segment[4] = {'-', '-', '-', '-'};
unsigned char potential_voters = 0;
unsigned char tmr0_cnt_blink = 0;
unsigned char tmr0_cnt_adc = 0;
unsigned char blink = 1; 
unsigned char rb7_pressed = 0;
unsigned char temp = 0;
unsigned char potential_voters_char[3] = "00";
unsigned int i; 
unsigned int pot_value = 0;
unsigned char pressed_and_toggledB[2][2] = {{0, 0},
                                            {0, 0}};
unsigned char candidates[8][11] = {"18 Poe    ",
                                   "23 Nesimi ",
                                   "33 Hatayi ",
                                   "34 Selimi ",
                                   "63 Nabi   ",
                                   "64 Galib  ",
                                   "67 Zweig  ",
                                   "99 Austen "};
unsigned int candidate_pot_ranges[8]= {127, 
                                       255, 
                                       383, 
                                       511, 
                                       639, 
                                       767, 
                                       895,    
                                       1023};

unsigned char voting_time[4] = "90 ";
unsigned char current_candidate_index = 0;// Adjust this in isr for ADC.
unsigned char candidate_votes[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char candidate_votes_char[8][3] = {"00",
                                            "00",
                                            "00",
                                            "00",
                                            "00",
                                            "00",
                                            "00",
                                            "00"};
unsigned char given_votes = 0;
unsigned char current_winner_index = 0;
unsigned char blink_changed = 0;
unsigned char adc_changed = 0;

void updateLCD();
void setup();
void state1();
void updateSevenSegment();
void clearSevenSegment();
void resetTimer1();
void reset();

void __interrupt() high_isr () {
    if (TMR1IE && TMR1IF) {
        tmr1_cnt_50ms++;
        if (tmr1_cnt_50ms == 20) {
            tmr1_cnt_50ms = 0;
            tmr1_cnt_1s++;
        }
        TMR1L = 0xDC;
        TMR1H = 0xB;
        TMR1IF = 0;
    }
    if (TMR0IE && TMR0IF) {
        tmr0_cnt_blink++;
        tmr0_cnt_adc++;
        if (tmr0_cnt_adc == 20) {
            // Start AD conversion
            tmr0_cnt_adc = 0;
            GO = 1;
        }
        if (tmr0_cnt_blink == 50) {
            blink_changed = 1;
            tmr0_cnt_blink = 0;
            blink = !blink;            
        } 
        TMR0L = 61; 
        TMR0IF = 0;
    }
    if (RBIE && RBIF) {
        // Do not forget to clear pressed status !!!!
        RBIF = 0;        
        // if pressed first make toggled true
        if (RB6 && !pressed_and_toggledB[0][1])
            pressed_and_toggledB[0][1] = 1;
        if (RB7 && !pressed_and_toggledB[1][1])
            pressed_and_toggledB[1][1] = 1;
        // if pressed status is not abundant, then check if toggled before. If so, set pressed to 1
        if (!RB6 && pressed_and_toggledB[0][1]) {            
            pressed_and_toggledB[0][0] = 1;            
            pressed_and_toggledB[0][1] = 0;
        }
        if (!RB7 && pressed_and_toggledB[1][1]) {           
            pressed_and_toggledB[1][0] = 1;      
            pressed_and_toggledB[1][1] = 0;
        }
    }    
    if (ADIF && ADIE) {
        // set current_candidate_index here;
        ADIF = 0;
        pot_value = (ADRESH << 8) | ADRESL;
        if (pot_value <= candidate_pot_ranges[0]) {
            if (current_candidate_index != 0) {
                adc_changed = 1;
                current_candidate_index = 0;
            }
        } else if (pot_value <= candidate_pot_ranges[1]) {
            if (current_candidate_index != 1) {
                adc_changed = 1;
                current_candidate_index = 1;
            }
        } else if (pot_value <= candidate_pot_ranges[2]) {
            if (current_candidate_index != 2) {
                adc_changed = 1;
                current_candidate_index = 2;
            }
        } else if (pot_value <= candidate_pot_ranges[3]) {
            if (current_candidate_index != 3) {
                adc_changed = 1;
                current_candidate_index = 3;
            }
        } else if (pot_value <= candidate_pot_ranges[4]) {
            if (current_candidate_index != 4) {
                adc_changed = 1;
                current_candidate_index = 4;
            }
        } else if (pot_value <= candidate_pot_ranges[5]) {
            if (current_candidate_index != 5) {
                adc_changed = 1;
                current_candidate_index = 5;
            }
        } else if (pot_value <= candidate_pot_ranges[6]) {
            if (current_candidate_index != 6) {
                adc_changed = 1;
                current_candidate_index = 6;
            }
        } else if (pot_value <= candidate_pot_ranges[7]) {
            if (current_candidate_index != 7) {
                adc_changed = 1;
                current_candidate_index = 7;
            }
        }
    }
}

void resetTimer0() {
    tmr0_cnt_adc = 0;
    tmr0_cnt_blink = 0;
    TMR0ON = 0;
    TMR0L = 61;
}

void resetTimer1() {
    tmr1_cnt_50ms = 0;
    tmr1_cnt_1s = 0;
    TMR1ON = 0;
    TMR1L = 0xDC;
    TMR1H = 0xB;
}

unsigned char get7SegmentValues (unsigned char val) {
    // Source to hex values https://forum.hosteng.com/wndm/HTMLHelp1/Instruction_Set/SEG_Hex_BCD_to_7_Segment_Display.htm
    switch (val) {
        case '0': return 0x3F;
        case '1': return 0x06;
        case '2': return 0x5B;
        case '3': return 0x4F;
        case '4': return 0x66;
        case '5': return 0x6D;
        case '6': return 0x7D;
        case '7': return 0x07;
        case '8': return 0x7F;
        case '9': return 0x67;
        case '-': return 0x40;
    }    
}

void clearSevenSegment() {
    for(i = 0; i < 4; i++) {
        seven_segment[i] = '-';
    }
}

void updateSevenSegment() {
    LATJ = get7SegmentValues(seven_segment[0]);
    LATH = 0x0F & 1; // Open appropriate position of 7Seg
    LATJ = get7SegmentValues(seven_segment[1]);
    LATH = 0x0F & 2; // Open appropriate position of 7Seg
    LATJ = get7SegmentValues(seven_segment[2]);
    LATH = 0x0F & 4; // Open appropriate position of 7Seg
    LATJ = get7SegmentValues(seven_segment[3]);
    LATH = 0x0F & 8; // Open appropriate position of 7Seg
}

// Setup (i.e config) is here
void setup() {
    //Disable interrupts while configuring.
    GIE = 0; // Disable interrupts   
    PEIE = 0; // Disable peripheral interrupt
    
    // LCD Configuration
    InitLCD();
	// A just in cause reset function	
    reset();
    // Configuring I/O
    ADCON1 = 0x0F;  // Declare all as digital
    TRISB = 0;
    TRISBbits.TRISB6 = 1;
    TRISBbits.TRISB7 = 1;  // All output except 6th and 7th bits
    TRISE = 0;
    TRISEbits.TRISE1 = 1;  // All output except 1st bit
    RBIE = 1; // Enable port b trigger on change interrupt
    RBIF = 0; // Clear RB interrupt flag
    // Configuring Timer0
    T0CON = 0;
    T08BIT = 1;
    T0PS0 = T0PS1 = T0PS2 = 1; // Prescaler 1:256
    TMR0L = 61;  // Start
    TMR0IE = 1; // Enable Timer0 interrupt
    TMR0ON = 1; // Enable Timer0 to start ADC acquisition
    // Configuring Timer1
    T1CON = 0;
    T1CKPS0 = T1CKPS1 = 1; // Prescaler 1:8
    TMR1L = 0xDC;  // Start
    TMR1H = 0xB;
    TMR1IE = 1; // Enable Timer1 interrupt
    // Configuring ADC
    CHS3 = CHS2 = 1; // Channel 12
    ADIE = 1; // Enable ADC interrupt   
    ACQT2 = ACQT0 = 1;
    ACQT1 = 0; // Acquisition time 12 T_AD (look for page 277, part 21.3) 
    ADIF = 0; // Clear flag
    ADFM = 1; // Right justified
    ADON = 1;
    // Configuring 7-Segment
    TRISH = 0; // PORTH<3:0> is used as output
    TRISJ = 0; // All of PORTJ is used as output
    
    //Enable interrupts after configuration.
    GIE = 1; // Enable interrupts   
    PEIE = 1; // Enable peripheral interrupt
}

void reset () {
    ClearLCDScreen();
    // Reset other vectors
    clearSevenSegment(); // Clear seven segment just as a precaution
    tmr1_cnt_50ms = tmr1_cnt_1s = potential_voters = tmr0_cnt_blink = tmr0_cnt_adc = rb7_pressed = 0;
	state = blink = 1;
	potential_voters_char[0] = potential_voters_char[1] = '0';
	pressed_and_toggledB[0][0] = pressed_and_toggledB[0][1] = pressed_and_toggledB[1][0] = pressed_and_toggledB[1][1] = 0;
}


// States here
void state1() {
    while(!RE1); // Check if RE1 is pressed
    while(RE1); // Check if RE1 is released 
    TMR1ON = 1;
    WriteCommandToLCD(0x80);   // Goto to the beginning of the first line
    WriteStringToLCD(" #Electro Vote# ");	// Write first line
    WriteCommandToLCD(0xC0); // Goto to the beginning of the second line
    WriteStringToLCD(" ############## ");
    while(tmr1_cnt_1s != 3); // Wait 3 seconds
    resetTimer1();
    state = 2;
    ClearLCDScreen();
    WriteCommandToLCD(0x80);
    WriteStringToLCD(" #Enter Voters# ");
    WriteCommandToLCD(0xC1);
    WriteStringToLCD(potential_voters_char); 	// Write first line
}

void state2() {
    // state2 increments tens digit by RB6.
    // If RB7 is pressed go to state3.
    rb7_pressed = pressed_and_toggledB[1][0];
    if (!rb7_pressed) {
        if (pressed_and_toggledB[0][0]) {
            pressed_and_toggledB[0][0] = 0;
            if (potential_voters / 10 == 9) {                
                potential_voters -= 90;
                potential_voters_char[0] -= 9;
                WriteCommandToLCD(0xC1);
                WriteStringToLCD(potential_voters_char);     
            }
            else {
                potential_voters += 10; 
                potential_voters_char[0] += 1;    
                WriteCommandToLCD(0xC1);
                WriteStringToLCD(potential_voters_char);           
            }
        }
    }
    else {
        pressed_and_toggledB[1][0] = 0;
        state = 3;
    }
}

void state3() {
    // state2 increments units digit by RB6.
    // If RB7 is pressed go to state4.
    rb7_pressed = pressed_and_toggledB[1][0];
    if (!rb7_pressed) {
        if (pressed_and_toggledB[0][0]) {
            pressed_and_toggledB[0][0] = 0;
            if (potential_voters % 10 == 9) {                
                potential_voters -= 9;
                potential_voters_char[1] -= 9;
                WriteCommandToLCD(0xC1);
                WriteStringToLCD(potential_voters_char);
            }
            else {
                potential_voters += 1; 
                potential_voters_char[1] += 1;  
                WriteCommandToLCD(0xC1);
                WriteStringToLCD(potential_voters_char);             
            }
        }
    }
    else {
        pressed_and_toggledB[1][0] = 0;
        state = 4;
        WriteCommandToLCD(0x80);
        WriteStringToLCD("  Time left :");
        WriteCommandToLCD(0x8D);
        WriteStringToLCD(voting_time);
        WriteCommandToLCD(0xCC);
        WriteStringToLCD(":");
        WriteCommandToLCD(0xC0);
        WriteStringToLCD(" >");
        WriteCommandToLCD(0xC2);
        WriteStringToLCD(candidates[current_candidate_index]);
        WriteCommandToLCD(0xCD);
        WriteStringToLCD(candidate_votes_char[current_candidate_index]);
        blink_changed = 0;
        blink = 1;
        adc_changed = 0;
        resetTimer0();
        TMR0ON = 1;
        resetTimer1();
        if (potential_voters) TMR1ON = 1;
        else state = 5;
    }
}

void state4() {
    if (blink_changed) {
        blink_changed = 0;
        WriteCommandToLCD(0xC1);
        if (blink) WriteStringToLCD(">");
        else WriteStringToLCD(" ");
    }
    if (adc_changed) {
        adc_changed = 0;
        WriteCommandToLCD(0xC2);
        WriteStringToLCD(candidates[current_candidate_index]);
        WriteCommandToLCD(0xCD);
        WriteStringToLCD(candidate_votes_char[current_candidate_index]);
    }
    rb7_pressed = pressed_and_toggledB[1][0];
    if (rb7_pressed) {
        pressed_and_toggledB[1][0] = 0;
        given_votes += 1;
        if (potential_voters == given_votes) {
            resetTimer1();
            state = 5;
        }
        candidate_votes[current_candidate_index] += 1;
        if (candidate_votes_char[current_candidate_index][1] == '9') {
            candidate_votes_char[current_candidate_index][0] += 1;
            candidate_votes_char[current_candidate_index][1] -= 9;
        } else {
            candidate_votes_char[current_candidate_index][1] += 1;
        }
        for (i = 0; i < 8; i++) {
            if (candidate_votes[i] > candidate_votes[current_winner_index]) {
                current_winner_index = i;
            }
        }
        seven_segment[0] = candidates[current_winner_index][0];
        seven_segment[1] = candidates[current_winner_index][1];
        seven_segment[2] = candidate_votes_char[current_winner_index][0];
        seven_segment[3] = candidate_votes_char[current_winner_index][1];
        WriteCommandToLCD(0xC2);
        WriteStringToLCD(candidates[current_candidate_index]);
        WriteCommandToLCD(0xCD);
        WriteStringToLCD(candidate_votes_char[current_candidate_index]);
    }
    if (tmr1_cnt_1s) {
        tmr1_cnt_1s = 0;
        if (voting_time[0] == '0' && voting_time[1] == '1') {
            voting_time[1] -= 1;
            WriteCommandToLCD(0xC1);
            WriteStringToLCD(">");
            resetTimer1();
            state = 5;
        } else {
            if (voting_time[1] == '0') {
                voting_time[0] -= 1;
                voting_time[1] += 9;
            } else {
                voting_time[1] -= 1;
            }
        }
        WriteCommandToLCD(0x8D);
        WriteStringToLCD(voting_time);
    }
}

void state5() {
    if (adc_changed) {
        adc_changed = 0;
        WriteCommandToLCD(0xC2);
        WriteStringToLCD(candidates[current_candidate_index]);
        WriteCommandToLCD(0xCD);
        WriteStringToLCD(candidate_votes_char[current_candidate_index]);
    }
}

// Main Function
void main(void) {
    setup();
    while(1) {
        switch(state) {
            case 1: state1(); break;
            case 2: state2(); break;
            case 3: state3(); break;
            case 4: updateSevenSegment(); state4(); break;
            case 5: updateSevenSegment(); state5(); break;
        }
    }
}

