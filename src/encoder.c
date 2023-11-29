#include "sm-codec.h"
#include "stdio.h"

#include "sys/time.h"
#include "time.h"

static struct tm* get_time() {
    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);
    return localtime(&tv.tv_sec);
}

static uint8_t time_to_rvbcd(int val) {
    return (((val / 10) % 10) | ((val % 10) << 4));
}

int encode_sms_delivery(tpdu_t *tpdu, uint8_t *buf) {
    // assert(tpdu && buf);
    int idx = 0;
    buf[idx++] = 0x24;
    buf[idx++] = tpdu->tp_da_len;
    buf[idx++] = 0xa1;
    memcpy(buf+idx, tpdu->tp_da_digits, (tpdu->tp_da_len+1)>>1);
    idx += (tpdu->tp_da_len+1)>>1;
    buf[idx++] = 0; /* tp-pid */
    buf[idx++] = 0; /* tp-dcs*/
    /* tp-service-centre-time-stamp */
    struct tm *st = get_time();
    buf[idx++] = time_to_rvbcd(st->tm_year + 1900);
    buf[idx++] = time_to_rvbcd(st->tm_mon + 1);
    buf[idx++] = time_to_rvbcd(st->tm_mday);
    buf[idx++] = time_to_rvbcd(st->tm_hour);
    buf[idx++] = time_to_rvbcd(st->tm_min);
    buf[idx++] = time_to_rvbcd(st->tm_sec);
    buf[idx++] = 0x23;
    /* tp-udl*/
    buf[idx++] = tpdu->tp_user_data_length + 1;
    memcpy(buf+idx, tpdu->tp_user_data, tpdu->tp_user_data_length);
    return idx + tpdu->tp_user_data_length;
}

uint8_t* encode_cp_ack(uint8_t *len) {
    *len = 2;
    uint8_t *buf = NULL;
    buf = malloc(*len);
    // assert(buf);
    uint8_t tio = 0x10;
    uint8_t sms_message = 0x09;
    buf[0] = (ALLOCATED_BY_RECEIVED << 7) | tio | sms_message;
    buf[1] = CP_ACK;
    return buf;
}

uint8_t* encode_rp_ack(uint8_t message_reference, uint8_t *len) {
    *len = 5;
    uint8_t *buf = NULL;
    buf = malloc(*len);
    // assert(buf);
    buf[0] = 0x99;
    buf[1] = CP_DATA;
    buf[2] = 2;
    buf[3] = NETWORK_MS_RP_ACK;
    buf[4] = message_reference;
    return buf;
}

/* the sc_address not include 0x91 */
uint8_t* encode_rp_data(uint8_t *sc_address, uint8_t sc_address_len,
    uint8_t *tpud, uint8_t tpudl, uint8_t *len)
{
    *len = 9 + tpudl + sc_address_len;
    uint8_t *buf = malloc(*len);
    // assert(buf);
    int idx = 0;
    buf[idx++] = 0x09;
    buf[idx++] = CP_DATA;
    buf[idx++] = 6 + tpudl + sc_address_len;
    buf[idx++] = NETWORK_MS_RP_DATA;
    buf[idx++] = 0; /* message reference*/
    buf[idx++] = sc_address_len + 1; /* rp-originator address len */
    buf[idx++] = 0x91;
    memcpy(buf+idx, sc_address, sc_address_len);
    idx += sc_address_len;
    buf[idx++] = 0; /* rp-destination address len */
    buf[idx++] = tpudl; /* tpdu len */
    memcpy(buf+idx, tpud, tpudl);
    return buf;
}

