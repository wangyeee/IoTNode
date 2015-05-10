#include "SwitchControl.h"

#include "stdio.h"// debug

switch_t switches[NUMBER_OF_SWITCHES];
DIO_Pin switch_mapping[NUMBER_OF_SWITCHES];

void init_switches(void) {
    uint8_t i;

    /*
     * TODO load switch configs from flash
     * e.g. some gpios may be enabled be default
     *      or switch to the last status
     */
    switch_mapping[0] = PA0;
    switch_mapping[1] = PA1;
    switch_mapping[2] = PA12;
    switch_mapping[3] = PA15;
    switch_mapping[4] = PC13;
    switch_mapping[5] = PF6;
    switch_mapping[6] = PF7;

    for (i = 0; i < NUMBER_OF_SWITCHES; i++) {
        switches[i].id = i;
        switches[i].status = OFF;
        switches[i].enabled = ENABLED;
        switches[i].pack = switch_pack;
        switches[i].unpack = switch_unpack;

        digital_init(switch_mapping[i]);
        if (switches[i].status && switches[i].enabled) {
            digital_high(switch_mapping[i]);
        } else {
            digital_low(switch_mapping[i]);
        }
    }

    nl_register_message_listener(switch_message_listener, SWITCH, 0);
}

uint8_t switch_message_listener(node_t from, uint8_t* msg, uint8_t len) {
    uint8_t inc;
    switch_t swt;

    if (len > NUMBER_OF_SWITCHES)
        len = NUMBER_OF_SWITCHES;
    inc = 0;
    while (inc < len) {
        inc += switch_unpack(&swt, msg + inc, len - inc);
        if (swt.id >= 0 && swt.id < NUMBER_OF_SWITCHES) {
            switches[swt.id].status = swt.status;
            switches[swt.id].enabled = swt.enabled;

            printf("[DEBUG] switch id = %d, status = %s, enabled = %s\n", swt.id,
                    (swt.status ? "ON" : "OFF"),
                    (swt.enabled ? "ENABLED" : "DISABLED"));

            if (swt.status && swt.enabled) {
                digital_high(switch_mapping[swt.id]);
            } else {
                digital_low(switch_mapping[swt.id]);
            }
        }
    }
    return 0;
}
