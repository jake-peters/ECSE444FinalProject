/*
 * @brief Header file for functions to help display data through UART.
 *
 */
#ifndef UART_DISPLAY_h
#define UART_DISPLAY_h

#include <stdlib.h>
// User input messages
char user_response[1] = "";

// Instruction
char print_instruction[] = "";

// Prompt messages
char count_down[] = "54321";
char start_menu_message[] = "\r\n================= Press \"s\" to start the game! ==================";
char game_starting_message[] = "\r\n============== The game will now be starting... Get ready in ==============\r\n";
char crash_message[] = "\r\n============= GAME OVER - You have crashed ===============";
char instruction1[] = "\r\n\n\n===Instruction 1: smooth climb up to 3000m! ===============";
char instruction2[90] = "";
char instruction33[] = "\r\n\n\n===Instruction 3: Turn left at 45 degrees to find enemies=======";
char instruction3[] = "\r\n\n\n===Instruction 3: Enemies ahead! Fire! ===============\n\n\n";
char instruction4[] = "\r\n\n\n===Instruction 4: Do a barrel roll! ===============\n\n\n";
char instruction5[] = "\r\n\n\n===Congratulations! You have passed all the instructions! Land smoothly! ===============\n\n";
char warning1[] = "\r\n===Warning! Correct your climbing degree ===============\n\n\n";
char warning2[] = "\r\n\n\n===Good! You have been escaped! ===============\n\n";
char warning3[] = "\r\n\n\n===Great Job! ===============\n\n\n";
// Flight data
char fd_title[] = "\r\n================== FLIGHT DATA =================";
char fd_previous_row[63] = "";
char fd_previous2_row[63] = "";
char fd_first_row[40] = "";
char fd_second_row[65] = "\r\nHorizontal Position Left: 0 m \tHorizontal Position Right: 0 m";
char fd_third_row[70] = "";
uint32_t magnitude;
uint32_t Tiltangle;

// ASCII Images of plane
static char plane_straight[] = "\r\n\t\t     \\ /\r\n\t\t+----o0o----+";
static char plane_right[] = "\r\n\t\t+--__   / /\r\n\t\t      -o0o__\r\n\t\t            --+";
static char plane_left[] = "\r\n\t\t      \\ \\   __+\r\n\t\t     __o0o--\r\n\t\t+__--";
static char plane_up[] = "\r\n\t\t      / \\\r\n\t\t   _/ |0| \\_\r\n\t\t  /  |   |  \\\r\n\t\t /  _ \\ / _  \\\r\n\t\t+__/ \\o0o/ \\__+";
static char plane_down[] = "\r\n\t\t     \\ /\r\n\t\t+____o0o____+\r\n\t\t \\   | |   /\r\n\t\t  \\__| |__/\r\n\t\t     \\ /";
static char parachute[] = "\r\n\t\t .---.            P ! !\r\n\t\t(_____)         L\r\n\t\t \\   /        E\r\n\t\t _\\o/_H   E\r\n\t\t   \\\\\n";

void instruction2countdown(char *instrction2, int count){
	snprintf(instruction2,90,"\r\n\n\n===Instruction 2: You are locked! You have %d seconds to escape ===============\n\n",count);
}

void flightDataPreviousRow(char *previous_row, int xaxis) {
	Tiltangle = abs(xaxis);
	int null = 0;
	if(xaxis >= 0){
		snprintf(previous_row,63,"\r\nNose up degrees: %d degrees \tNose down degrees: %d degrees", Tiltangle, null);
	}
	if(xaxis < 0){
			snprintf(previous_row,63,"\r\nNose up degrees: %d degrees \tNose down degrees: %d degrees", null, Tiltangle);
		}


}

void flightDataPrevious2Row(char *previous2_row, int yaxis) {
	Tiltangle = abs(yaxis);
	int null = 0;
	if(yaxis >= 0){
			snprintf(previous2_row,63,"\r\nTilt Left degrees: %d degrees \tTilt Right degrees: %d degrees", Tiltangle, null);
		}
	if(yaxis < 0){
			snprintf(previous2_row,63,"\r\nTilt Left degrees: %d degrees \tTilt Right degrees: %d degrees", null, Tiltangle);
		}

}
void flightDataFirstRow(char *first_row, int32_t altitude, uint16_t speed) {
	snprintf(first_row,40,"\r\nAltitude: %06u m \tSpeed: %d km/h", altitude, speed);
}

void flightDataSecondtRow(char *second_row, int32_t horizontal) {
	magnitude = abs(horizontal);
	if (horizontal < 0) {
		snprintf(second_row,65,"\r\nHorizontal Position Left: %d m \tHorizontal Position Right: 0 m", magnitude);
	} else if (horizontal > 0) {
		snprintf(second_row,65,"\r\nHorizontal Position Left: 0 m \tHorizontal Position Right: %d m", magnitude);
	} else {
		sprintf(second_row, "\r\nHorizontal Position Left: 0 m \tHorizontal Position Right: 0 m");
	}
}

void flightDataThirdRow(char *third_row, char *instruction, uint8_t time) {
	snprintf(third_row,70,"\r\nInstruction: %s \tRemaining Time: %d s\r\n", instruction, time);
}



#endif // UART_DISPLAY_H
