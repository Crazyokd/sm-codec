#include "sm-codec.h"
#include "log.h"

int decode_tpdu(tpdu_t *tpdu, uint8_t rp_mti, uint8_t *buf, uint8_t len) {
    memset(tpdu, 0, sizeof(tpdu_t));
    int idx = 0;
    tpdu->tp_mti = 0x03 & buf[idx];
    if (tpdu->tp_mti == SMS_DELIVERY_REPORT
        && (rp_mti == MS_NETWORK_RP_ACK || rp_mti == MS_NETWORK_RP_ERROR)) {
        /* tp-user-data-header-indication */
        uint8_t tp_udhi= 0x40 & buf[idx++];
        /* tp failure cause */
        if (rp_mti == MS_NETWORK_RP_ERROR) {
            uint8_t tp_fcs = buf[idx];
        }
        /* tp parameter indicator */
        idx++;
        uint8_t tp_pi = buf[idx];
        /* maybe have some optional IE */
        /* ... */
        return SM_CODEC_OK;
    } else if (tpdu->tp_mti == SMS_STATUS_REPORT) {
        return SM_CODEC_OK;
    } else if (tpdu->tp_mti == SMS_SUBMIT_REPORT
        && (rp_mti == NETWORK_MS_RP_ACK || rp_mti == NETWORK_MS_RP_ERROR)) {
        idx++;
        if (rp_mti == NETWORK_MS_RP_ERROR) {
            uint8_t tp_fcs = buf[idx++];
        }
        uint8_t tp_pi = buf[idx];
        /* tp-scts */
        return SM_CODEC_OK;
    } else if (tpdu->tp_mti == SMS_SUBMIT && rp_mti == MS_NETWORK_RP_DATA) {
        /**
         * TP-Reject-Duplicates
         * TP-Validity-Period-Format
         * TP-Reply-Path
         * TP-User-Data-Header-Indicator
         * TP-Status-Report-Request
         * ======================================
         * TP-Message-Reference I
         * TP-Destination-Address
         * TP-Protocol-Identifier
         * TP-Data-Coding-Scheme
         * TP-Validity-Period O
         * tp-udl
         * tp-ud
         */
        uint8_t tp_rp = 0x80 & buf[idx];
        uint8_t tp_udhi= 0x40 & buf[idx];
        uint8_t tp_srr = 0x20 & buf[idx];
        uint8_t tp_vpf = 0x18 & buf[idx];
        uint8_t tp_rd = 0x04 & buf[idx];
        idx++;
        tpdu->tp_mr = buf[idx++];
        tpdu->tp_da_len = buf[idx++];
        uint8_t tp_da_extension = 0x80 & buf[idx];
        uint8_t tp_da_type_of_number = 0x70 & buf[idx];
        uint8_t tp_da_numbering_plan = 0x0f & buf[idx];
        idx++;
        uint8_t tp_da_len_byte = (tpdu->tp_da_len + 1) >> 1;
        memcpy(tpdu->tp_da_digits, buf+idx, tp_da_len_byte);
        idx += tp_da_len_byte;
        uint8_t tp_pid = buf[idx++];
        uint8_t tp_dcs = buf[idx++];
        /* tp-vp which len maybe 0, 1 or 7 */
        tpdu->tp_user_data_length = buf[idx++];
        memcpy(tpdu->tp_user_data, buf+idx, len - idx);
        return SM_CODEC_OK;
    } else if (tpdu->tp_mti == SMS_DELIVERY && rp_mti == NETWORK_MS_RP_DATA) {
        uint8_t tp_rp = 0x80 & buf[idx];
        uint8_t tp_udhi= 0x40 & buf[idx];
        uint8_t tp_sri = 0x20 & buf[idx];
        uint8_t tp_lp = 0x08 & buf[idx];
        uint8_t tp_mms = 0x04 & buf[idx];
        /* decode tp-originating-address */
        idx++;
        uint8_t tp_originating_address_length = buf[idx];
        idx++;
        uint8_t tp_originating_address_extension = 0x80 & buf[idx];
        uint8_t tp_originating_address_type_of_number = 0x70 & buf[idx];
        uint8_t tp_originating_address_numbering_plan = 0x0f & buf[idx];
        idx++;
        uint8_t tp_oa_digits[6];
        uint8_t tp_originating_address_length_byte = (tp_originating_address_length + 1) >> 1;
        memcpy(tp_oa_digits, buf+idx, tp_originating_address_length_byte);
        idx += tp_originating_address_length_byte;

        /* decode TP-Protocol-Identifier, per bit has its own meaning. */
        /* ignore here. */
        uint8_t tp_pid = buf[idx];
        /* TP Data Coding Scheme */
        idx++;
        uint8_t tp_dcs = buf[idx++];
        /* decode tp-service-centre-time-stamp */
        /* per digit represent with bcd */
        uint8_t year = buf[idx];
        idx++;
        uint8_t month = buf[idx];
        idx++;
        uint8_t day = buf[idx];
        idx++;
        uint8_t hour = buf[idx];
        idx++;
        uint8_t minutes = buf[idx];
        idx++;
        uint8_t seconds = buf[idx];
        idx++;
        uint8_t timezone = buf[idx];
        idx++;
        /** 
         * The RP-User data field contains the TPDU and is mandatory in a RP-DATA message.
         * RP-User data is also optionally carried in an RP-Error message
         * the element has a variable length, up to 233 octets
         * and in a RP ERROR and in a RP ACK message the length is up to 234 octets.
         * ==========================================================================
         * RP-User data in an RP-Error message is conveyed as diagnostic information within the "SM-DeliveryFailureCause"
         * response to a MAP Forward-Short-Message procedure (see 3GPP TS 29.002). The diagnostic information may be sent
         * in both directions, and shall always be forwarded by the MSC if it is received. 
         */
        tpdu->tp_user_data_length = buf[idx] - 1;
        idx++;
        memcpy(tpdu->tp_user_data, buf + idx, tpdu->tp_user_data_length);
        // assert(idx + tpdu->tp_user_data_length == len);
        return SM_CODEC_OK;
    } else {
        lol_debug("unknown tp-mti:%d", tpdu->tp_mti);
        return SM_CODEC_ERROR;
    }
    return SM_CODEC_ERROR;
}

/* defined in clause 7.2 in 3GPP TS 24.011 */
int decode_message_container(nas_cp_message_t *cm, uint8_t length, uint8_t *buf) {
    // assert(length >= 2);
    // nas_cp_message_t *cm = malloc(sizeof(nas_cp_message_t));
    memset(cm, 0, sizeof(nas_cp_message_t));
    int idx = 0;
    /* defined in ts 24.007 */
    uint8_t protocol_discriminator = 0x0f & buf[idx];
    uint8_t ti_flag = 0x80 & buf[idx];
    uint8_t tio = 0x70 & buf[idx];

    /* subclause 8.1.3 */
    idx++;
    uint8_t message_type = buf[idx];
    cm->message_type = message_type;
    idx++;
    if (message_type == CP_DATA) {
        uint8_t cp_data_length = buf[idx];
        /* RP-DATA */
        idx++;
        // assert(length == cp_data_length + idx);
        cm->rp_message.message_type_indicator = 0x07 & buf[idx];
        idx++;
        cm->rp_message.message_reference = buf[idx];
        idx++;
        if (cm->rp_message.message_type_indicator == NETWORK_MS_RP_DATA
            || cm->rp_message.message_type_indicator == MS_NETWORK_RP_DATA) {
            uint8_t rp_originator_address_length = buf[idx++];
            cm->rp_message.originator_address.length = rp_originator_address_length;
            if (rp_originator_address_length > 0) {
                cm->rp_message.originator_address.extension = 0x80 & buf[idx];
                cm->rp_message.originator_address.type_of_number = 0x70 & buf[idx];
                cm->rp_message.originator_address.numbering_plan_identification = 0x0f & buf[idx];
                idx++;
                memcpy(cm->rp_message.originator_address.called_party_bcd_number, buf + idx,
                    rp_originator_address_length - 1);
                idx += rp_originator_address_length - 1;
            }
            cm->rp_message.destination_address.length = buf[idx++];
            if (cm->rp_message.destination_address.length > 0) {
                // decode rp-destination-address
                cm->rp_message.destination_address.extension = 0x80 & buf[idx];
                cm->rp_message.destination_address.type_of_number = 0x70 & buf[idx];
                cm->rp_message.destination_address.numbering_plan_identification = 0x0f & buf[idx];
                idx++;
                memcpy(cm->rp_message.destination_address.called_party_bcd_number, buf + idx,
                    cm->rp_message.destination_address.length - 1);
                idx += cm->rp_message.destination_address.length - 1;
            }
            cm->rp_message.tpdu_length = buf[idx];
            idx++;
            memcpy(cm->rp_message.tpdu, buf + idx, cm->rp_message.tpdu_length);
            /* decode TPDU. see 23.040 #9.2 for detail */
            tpdu_t *tpdu = malloc(sizeof(tpdu_t));
            decode_tpdu(tpdu, cm->rp_message.message_type_indicator,
                cm->rp_message.tpdu, cm->rp_message.tpdu_length);
        } else if (cm->rp_message.message_type_indicator == MS_NETWORK_RP_ACK) {

        } else if (cm->rp_message.message_type_indicator == NETWORK_MS_RP_ACK) {

        } else if (cm->rp_message.message_type_indicator == MS_NETWORK_RP_ERROR
            || cm->rp_message.message_type_indicator == NETWORK_MS_RP_ERROR) {
            uint8_t rp_cause_length = buf[idx++];
            cm->rp_message.cause_value = buf[idx++];
            /* follow with n Diagnostic field, per diagnostic is 1 Byte */
        }
    } else if (message_type == CP_ACK) {
        // cp-ack
        lol_debug("received cp-ack");
    } else if (message_type == CP_ERROR) {
        // cp-error
        cm->cause_value = buf[idx];
    } else {
        // decode error
        lol_debug("decode nas message container message type failed");
        return SM_CODEC_ERROR;
    }
    return SM_CODEC_ERROR;
}
