/*
 * @brief Header file for functions to help display data through UART.
 *
 */
#ifndef UART_DISPLAY_h
#define UART_DISPLAY_h

// User input messages
char user_response[1] = "";

// Prompt messages
char count_down[] = "54321";
char start_menu_message[] = "\n================= Press \"s\" to start the game! ==================";
char game_starting_message[] = "\n============== The game will now be starting... Get ready in ==============\n";

// Flight data
char fd_title[] = "\n================== FLIGHT DATA =================";
char fd_first_row[40] = "";
char fd_second_row[70] = "";


// ASCII Images of plane
static char plane_straight[] = "\n\t\t     \\ /\n\t\t+----o0o----+";
static char plane_right[] = "\n\t\t+--__   / /\n\t\t      -o0o__\n\t\t            --+";
static char plane_left[] = "\n\t\t      \\ \\   __+\n\t\t     __o0o--\n\t\t+__--";
static char plane_up[] = "\n\t\t      /\\\n\t\t   _/ |0| \\_\n\t\t  /   |  |  \\\n\t\t /  _ \\ / _  \\\n\t\t+__/ \\o0o/ \\__+";
static char plane_down[] = "\n\t\t     \\ /\n\t\t+____o0o____+\n\t\t \\   | |   /\n\t\t  \\__| |__/\n\t\t     \\/";

void flightDataFirstRow(char *first_row, uint16_t altitude, uint16_t speed) {
	snprintf(first_row,40,"\nAltitude: %d ft \tSpeed: %d km/h", altitude, speed);
}

void flightDataSecondRow(char *second_row, char *instruction, uint8_t time) {
	snprintf(second_row,70,"\nInstruction: %s \tRemaining Time: %d s\n", instruction, time);
}



#endif // UART_DISPLAY_H
