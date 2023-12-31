#include "sm-codec.h"
#include "stdio.h"
#include "log.h"


int main() {
    lol_debug("start decode");
    /* ms->network cp-data */
    uint8_t data[] = {
        0x19, 0x01, 0x21, 0x00, 0x01, 0x00, 0x08, 0x91, 0x68, 0x31, 0x08, 0x10, 0x83, 0x00, 0xf0, 0x14,
        0x01, 0xaf, 0x0b, 0x81, 0x31, 0x08, 0x21, 0x43, 0x05, 0xf3, 0x00, 0x00, 0x08, 0xce, 0xe0, 0x14,
        0x04, 0x9a, 0x36, 0xa7
    };
    struct {
        uint8_t length;
        uint8_t buffer[250];
    } mc;
    mc.length = 36;
    memcpy(mc.buffer, data, mc.length);
    nas_cp_message_t *cm = malloc(sizeof(nas_cp_message_t));
    decode_message_container(cm, mc.length, mc.buffer);

    /* ms->network cp-data 2 */
    uint8_t data12[] = {
        0x29, 0x01, 0x1d, 0x00, 0x18, 0x00, 0x08, 0x91, 0x68, 0x31, 0x08, 0x10, 0x83, 0x00, 0xf0, 0x10,
        0x01, 0x56, 0x0b, 0x81, 0x31, 0x08, 0x21, 0x43, 0x05, 0xf8, 0x00, 0x00, 0x03, 0xc7, 0xf3, 0x19
    };
    mc.length = 32;
    memcpy(mc.buffer, data12, mc.length);
    decode_message_container(cm, mc.length, mc.buffer);

    /* network->ms rp-data*/
    uint8_t data2[] = {
        0x09, 0x01, 0x27, 0x01, 0x00, 0x07, 0x91, 0x31, 0x08, 0x10, 0x83, 0x00, 0xf0, 0x00, 0x1b, 0x24,
        0x0b, 0xa1, 0x31, 0x08, 0x21, 0x43, 0x05, 0xf3, 0x00, 0x00, 0x22, 0x11, 0x01, 0x41, 0x74, 0x65,
        0x23, 0x09, 0xee, 0xf0, 0x1c, 0x04, 0x02, 0xcd, 0xdb, 0x73
    };
    mc.length = 42;
    memcpy(mc.buffer, data2, mc.length);
    decode_message_container(cm, mc.length, mc.buffer);

    /* network->ms cp-ack */
    uint8_t data3[] = {0x99, 0x04};
    mc.length = 2;
    memcpy(mc.buffer, data3, mc.length);
    decode_message_container(cm, mc.length, mc.buffer);

    /* network->ms rp-ack */
    uint8_t data4[] = {0x99, 0x01, 0x02, 0x03, 0x01};
    mc.length = 5;
    memcpy(mc.buffer, data4, mc.length);
    decode_message_container(cm, mc.length, mc.buffer);

    /* test encoder */
    uint8_t len;
    uint8_t *buf;
    uint8_t sc_address[] = {0x31, 0x08, 0x10, 0x83, 0x00, 0xf0};
    uint8_t sm_rp_ui[] = {0x24, 0x0b, 0xa1, 0x31, 0x08, 0x21,
        0x43, 0x05, 0xf3, 0x00, 0x00, 0x22, 0x11, 0x01, 0x41, 0x74,
        0x65, 0x23, 0x09, 0xee, 0xf0, 0x1c, 0x04, 0x02, 0xcd, 0xdb, 0x73};
    buf = encode_rp_data(sc_address,
        6,
        sm_rp_ui,
        27,
        &len);
    // assert(buf);
    mc.length = len;
    memcpy(mc.buffer, buf, len);
    free(buf);
    decode_message_container(cm, mc.length, mc.buffer);

    buf = encode_cp_ack(&len);
    mc.length = len;
    memcpy(mc.buffer, buf, len);
    free(buf);
    decode_message_container(cm, mc.length, mc.buffer);

    buf = encode_rp_ack(1, &len);
    mc.length = len;
    memcpy(mc.buffer, buf, len);
    free(buf);
    decode_message_container(cm, mc.length, mc.buffer);
    free(cm);

    /* test tpdu decoder */
    char sm_rp_ui3[] = {0x01, 0xaf, 0x0b, 0x81, 0x31, 0x08, 0x21, 0x43, 0x05, 0xf3, 0x00,
        0x00, 0x08, 0xce, 0xe0, 0x14, 0x04, 0x9a, 0x36, 0xa7};
    uint8_t tpdul = 20;
    tpdu_t *tpdu = malloc(sizeof(tpdu_t));
    decode_tpdu(tpdu, MS_NETWORK_RP_DATA, (uint8_t*)sm_rp_ui3, tpdul);
    buf = calloc(sizeof(uint8_t), 27);
    // assert(buf);
    int buf_len = encode_sms_delivery(tpdu, buf);
    decode_tpdu(tpdu, NETWORK_MS_RP_DATA, buf, buf_len);
    free(buf);

    char sm_rp_ui2[28] = {0x24, 0x0b, 0xa1, 0x31, 0x08, 0x21, 0x43, 0x05, 0xf3, 0x00,
        0x00, 0x22, 0x11, 0x01, 0x41, 0x74, 0x65, 0x23, 0x09, 0xee,
        0xf0, 0x1c, 0x04, 0x02, 0xcd, 0xdb, 0x73};
    tpdul = 27;
    decode_tpdu(tpdu, NETWORK_MS_RP_DATA, (uint8_t*)sm_rp_ui, tpdul);
    
    free(tpdu);
    lol_debug("decode success");
    return 0;
}