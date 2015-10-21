
/* © Copyright 2015, ggehlot@cs.stonybrook.edu
 * arpsingh@cs.stonybrook.edu,smehra@stonybrook.edu
 * Everyone is permitted to copy and distribute verbatim copies
 * of this license document, but changing it is not allowed.
 */

#include <sys/defs.h>
#include <sys/sbunix.h>
#include <sys/interrupts.h>
#include <sys/process.h>

/*
 * This will keep track of how many ticks that the system
 * has been running for 
 */

int timer_ticks = 0;
extern task_struct *sleep_task;
void tick_timer(struct isr_regs *r)
{
	uint32_t hrs = 0, mins = 0, secs = 0;
	uint32_t time_temp = 0;

	/* Increment our 'tick count' */
	timer_ticks++;


	/* Approximately after 1 sec,
	 * display the updated time
	 */

	if((timer_ticks % 18) == 0) {

		//decrement_sleep_time();

                //schedule();

		//convert the secs to hrs mins secs
                hrs = (timer_ticks/100)/3600;

                time_temp = (timer_ticks/100)%3600;
                mins = time_temp/60;
                secs = time_temp%60;

		if(secs == 60)
			secs = 0; 
		print_line(36, 24, "      "); 
		print_line(36, 24, "%d:%d:%d", hrs, mins, secs);

		if(sleep_task != NULL)
			decrement_sleep_time();

                schedule();

	}

}
void install_timer()
{
	
	uint16_t TIMER_frequency = 1193180/100;

	//Set the command byte
	outb(0x43,0x36);

	//Set low byte of timer divisor
        outb(0x40, (uint16_t)(TIMER_frequency & 0xFF));

	//Set high byte of timer divisor
        outb(0x40, (uint16_t)(TIMER_frequency >> 8));

}
