
#ifndef CAN_HANLDER
#define CAN_HANLDER

#include "application.h"
#include "canMsgs.h"

/**
 * @brief Install can 
 */
void install_can_handler();

/**
 * @brief Inits CAN
 */
void init_can_handler();

/**
 * @brief Function to send a can message
 */
void send_msg(int index, const uint8_t* data, int length);

/**
 * @brief Function that gets called on CAN msg recv
 */
void receive_msg(App* self, uint8_t* data);

/**
 * @brief Checks timeout on all msgs
 */
void check_timeout();

#endif
