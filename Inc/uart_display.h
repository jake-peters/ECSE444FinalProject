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
char start_menu_message[] = "Press \"s\" to start the game!";
char game_starting_message[] = "The game will now be starting... Get ready in";


// ASCII Images of plane
static char plane_straight[] = "\n     \\ /\n+----o0o----+";
static char plane_right[] = "\n+--__   / /\n      -o0o__\n            --+";
static char plane_left[] = "\n      \\ \\   __+\n     __o0o--\n+__--";

#endif // UART_DISPLAY_H
