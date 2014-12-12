/*
 * t11_main.h
 *
 * T11 - Suspicious Package Training Device
 * Edward Sayers, Seth Ward, Devin Lorenzen, Jeremiah Franke
 * 
 */ 


#ifndef TD_MAIN_H_
#define TD_MAIN_H_
	
/* Helper functions for bit manipulation */
#define setbit(reg, bit) reg |= (1 << bit)
#define clearbit(reg, bit) reg &= ~(1 << bit)
#define togglebit(reg, bit) reg ^= (1 << bit)
	
/* Set CPU frequency */
#define F_CPU 1000000UL

/* Function Declarations */
void soundBuzzer(void);
void triggerDevice(char source);

#endif /* TD_MAIN_H_ */