#include "SwitchControl.h"

#define NUMBER_OF_SWITCHS 7

switch_t switches[NUMBER_OF_SWITCHS];

void init_switchs(void) {
    uint8_t i;

    /*
     * TODO load switch configs from flash
     * e.g. some gpios may be enabled be default
     *      or switch to the last status
     */
    switches[0].pin = PA0;
    switches[1].pin = PA1;
    switches[2].pin = PA12;
    switches[3].pin = PA15;
    switches[4].pin = PC13;
    switches[5].pin = PF6;
    switches[6].pin = PF7;

    for (i = 0; i < NUMBER_OF_SWITCHS; i++) {
        switches[i].id = i;
        switches[i].status = OFF;
        switches[i].enabled = DISABLED;

        digital_init(switches[i].pin);
        if (switches[i].status && switches[i].enabled) {
            digital_high(switches[i].pin);
        } else {
            digital_low(switches[i].pin);
        }
    }

    nl_register_message_listener(switch_message_listener, SWITCH, 0);
}

uint8_t switch_message_listener(node_t from, uint8_t* msg, uint8_t len) {
    uint8_t i;
    switch_t* swt;

    if (len > NUMBER_OF_SWITCHS) len = NUMBER_OF_SWITCHS;

    for (i = 0; i < len; i++) {
        /* warning, only access the first 8 bits!!! */
        swt = (switch_t*) &(msg[i]);
        if (swt->id > 0 && swt->id < NUMBER_OF_SWITCHS) {
            switches[swt->id].status = swt->status;
            switches[swt->id].enabled = swt->enabled;

            if (switches[swt->id].status && switches[swt->id].enabled) {
                digital_high(switches[swt->id].pin);
            } else {
                digital_low(switches[swt->id].pin);
            }
        }
    }
    return 0;
}
