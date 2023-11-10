#include "../include/constant.h"
#include "../include/type.h"
#include "../include/utils.h"
#include "../include/io.h"
#include "../include/log.h"

/// @brief handle keyboard interrupt
/// @param vector 
void keyboard_handler(i32 vector) {
    assert(vector == 0x21);
    send_eoi(vector);
    u8 scan_code = readb(KEYBOARD_DATA_PORT);
    trace("scan code: 0x%x\n", scan_code);
}

/// @brief init keyboard interrupt
void keyboard_init() {
    set_interrupt_handler(0x21, keyboard_handler);
    set_interrupt_mask(0x21);
}