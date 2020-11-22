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
char crash_message[] = "============= GAME OVER - You have crashed ===============";

// Flight data
char fd_title[] = "\r\n================== FLIGHT DATA =================";
char fd_first_row[40] = "";
char fd_second_row[65] = "\r\nHorizontal Position Left: 0 m \tHorizontal Position Right: 0 m";
char fd_third_row[70] = "";
uint32_t magnitude;

// ASCII Images of plane
static char plane_straight[] = "\r\n\t\t     \\ /\r\n\t\t+----o0o----+";
static char plane_right[] = "\r\n\t\t+--__   / /\r\n\t\t      -o0o__\r\n\t\t            --+";
static char plane_left[] = "\r\n\t\t      \\ \\   __+\r\n\t\t     __o0o--\r\n\t\t+__--";
static char plane_up[] = "\r\n\t\t      / \\\r\n\t\t   _/ |0| \\_\r\n\t\t  /  |   |  \\\r\n\t\t /  _ \\ / _  \\\r\n\t\t+__/ \\o0o/ \\__+";
static char plane_down[] = "\r\n\t\t     \\ /\r\n\t\t+____o0o____+\r\n\t\t \\   | |   /\r\n\t\t  \\__| |__/\r\n\t\t     \\ /";

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
