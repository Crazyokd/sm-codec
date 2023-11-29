#ifndef SM_CODEC_H_
#define SM_CODEC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdlib.h"
#include "string.h"

typedef unsigned char uint8_t;


#define SM_CODEC_OK             0
#define SM_CODEC_ERROR          -1

/* see 23.040 #9.2 for detail */
typedef struct tpdu_s {
#define SMS_DELIVERY                        0 /* sc->ms */
#define SMS_DELIVERY_REPORT                 0 /* ms->sc */
#define SMS_STATUS_REPORT                   2 /* sc->ms */
#define SMS_COMMAND                         2 /* ms->sc */
#define SMS_SUBMIT                          1 /* ms->sc */
#define SMS_SUBMIT_REPORT                   1 /* sc->ms */
#define SMS_RESERVED                        3
    uint8_t tp_mti;
    uint8_t tp_mr;
    uint8_t tp_da_len;
    uint8_t tp_da_digits[6];
    uint8_t tp_user_data_length;
    uint8_t tp_user_data[232];
} tpdu_t;

/* see ts 24.011 #7.3 for detail */
typedef struct nas_cp_message_s {
#define SMS_MESSAGE                 0x09
    uint8_t protocol_discriminator;
#define ALLOCATED_BY_SENDER         0
#define ALLOCATED_BY_RECEIVED       1
    uint8_t ti_flag;
    uint8_t tio;

#define CP_DATA         0x01
#define CP_ACK          0x04
#define CP_ERROR        0x10
    uint8_t message_type;

#define NETWORK_FAILURE                                                 17
#define CONGESTION                                                      22
#define INVALID_TRANSACTION_IDENTIFIER_VALUE                            81
#define SEMANTICALLY_INCORRECT_MESSAGE                                  95
#define INVALID_MANDATORY_INFORMATION                                   96
#define MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED                    97
#define MESSAGE_NOT_COMPATIBLE_WITH_THE_SHORT_MESSAGE_PROTOCOL_STATE    98
#define INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED             99
#define RPOTOCOL_ERROR                                                  111 
    uint8_t cause_value;

    struct {
        /* The message type indicator, MTI, is a 3-bit field,
         * located in the first octet of all RP-messages. 
         * The coding of the MTI is defined by table 8.3/3GPP TS 24.011.
         */
#define MS_NETWORK_RP_DATA              0x0 /* ms->network with rp-data */
#define NETWORK_MS_RP_DATA              0x1
#define MS_NETWORK_RP_ACK               0x2
#define NETWORK_MS_RP_ACK               0x3
#define MS_NETWORK_RP_ERROR             0x4
#define NETWORK_MS_RP_ERROR             0x5
#define MS_NETWORK_RP_SMMA              0x6 /* This message is sent by the mobile station to relay a notification
                                             * to the network that the mobile has memory available to
                                             * receive one or more short messages.
                                             */
        uint8_t message_type_indicator; /* 3 bits */
        /* The message reference field contains a sequence number
         * in the range 0 through 255, and is used to link an RP-ACK message
         * or RP-ERROR message to the associated (preceding) RP-DATA or RP-SMMA message transfer attempt
         */
        uint8_t message_reference;

#define ADDRESS_MAX_LEN                 11 /* [2, 11] */
        /*
         * In the case of MT transfer this element contains the originating Service Centre address
         * The RP-Originator Address information element is coded as shown in figure 8.5/3GPP TS 24.011
         * The RP-Originator Address is a type 4 information element
         */
        struct {
            uint8_t length; /* n->ms [2, 11]; ms->n 0 */
            uint8_t extension;
            uint8_t type_of_number;
            uint8_t numbering_plan_identification;
            /**
             * The contents are the same as those defined for the Called Party BCD Number IE
             * defined in 3GPP TS 24.008 [5].
             * If the RP-Originator Address contains an odd number of digits,
             * bits 5 to 8 of the last octet shall be filled with an end mark coded as "1111"
            */
            uint8_t called_party_bcd_number[ADDRESS_MAX_LEN-1]; /* (11 + 1) >> 1 */
        } originator_address;
        /**
         * In the case of MO transfer, this element contains the destination Service Centre address
         * 
        */
        struct {
            uint8_t length; /* n->ms [2, 11]; ms->n 0 */
            uint8_t extension;
            uint8_t type_of_number;
            uint8_t numbering_plan_identification;
            /**
             * The contents are the same as those defined for the Called Party BCD Number IE
             * defined in 3GPP TS 24.008 [5].
             * If the RP-Originator Address contains an odd number of digits,
             * bits 5 to 8 of the last octet shall be filled with an end mark coded as "1111"
            */
            uint8_t called_party_bcd_number[ADDRESS_MAX_LEN-1]; /* (11 + 1) >> 1 */
        } destination_address;
#define TPDU_MAXIMUM_LENGTH     232
        uint8_t tpdu_length;
        uint8_t tpdu[TPDU_MAXIMUM_LENGTH];


/* may be contained in an RP-ERROR message in a mobile originating SM-transfer attempt */
#define UNASSIGNED_OR_UNALLOCATED_NUMBER                            1
#define OPERATOR_DETERMINED_BARRING                                 8
#define CALL_BARRED                                                 10
#define RESERVED                                                    11
#define SHORT_MESSAGE_TRANSFER_REJECTED                             21
#define DESTINATION_OUT_OF_ORDER                                    27
#define UNIDENTIFIED_SUBSCRIBER                                     28
#define FACILITY_REJECTED                                           29
#define UNKNOWN_SUBSCRIBER                                          30
#define NETWORK_OUT_OF_ORDER                                        38
#define TEMPORARY_FAILURE                                           41 /* all other cause value shall be treated as this */
#define MO_CONGESTION                                               42
#define RESOURCE_UNAVAILABLE                                        47
#define REQUESTED_FACILITY_NOT_SUBSCRIBED                           50
#define REQUESTED_FACILITY_NOT_IMPLEMENTED                          69
#define INVALID_SHORT_MESSAGE_TRANSFER_REFERENCE_VALUE              81
#define SEMANTICALLY_INCORRECT_MESSAGE                              95
#define INVALID_MANDATORY_INFORMATION                               96
#define MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED                97
#define MESSAGE_NOT_COMPATIBLE_WITH_SHORT_MESSAGE_PROTOCOL_STATE    98
#define INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED         99
#define PROTOCOL_ERROR                                              111
#define INTERWORKING                                                127

/* may be contained in an RP-ERROR message in a mobile terminating SM-transfer attempt */
#define MEMORY_CAPACITY_EXCEEDED                                    22
#define INVALID_SHORT_MESSAGE_TRANSFER_REFERENCE_VALUE              81
#define SEMANTICALLY_INCORRECT_MESSAGE                              95
#define INVALID_MANDATORY_INFORMATION                               96
#define MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED                97
#define MESSAGE_NOT_COMPATIBLE_WITH_SHORT_MESSAGE_PROTOCOL_STATE    98
#define INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED         99
#define PROTOCOL_ERROR                                              111 /* all other cause value shall be treated as this */

/* may be contained in an RP-ERROR message in a memory available notification attempt */
#define UNKNOWN_SUBSCRIBER                                          30
#define NETWORK_OUT_OF_ORDER                                        38
#define TEMPORARY_FAILURE                                           41 /* all other cause value shall be treated as this */
#define MEMORY_AVAILABLE_CONGESTION                                 42
#define RESOURCES_UNAVAILABLE                                       47
#define REQUESTED_FACILITY_NOT_IMPLEMENTED                          69
#define SEMANTICALLY_INCORRECT_MESSAGE                              95
#define INVALID_MANDATORY_INFORMATION                               96
#define MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED                97
#define MESSAGE_NOT_COMPATIBLE_WITH_SHORT_MESSAGE_PROTOCOL_STATE    98
#define INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED         99
#define PROTOCOL_ERROR                                              111
#define INTERWORKING                                                127
        uint8_t cause_value;
    } rp_message;

#define SMS_TIMER_TR1M 40 /* timer TR1M shall be greater than 35 seconds and less than 45 seconds */
#define SMS_TIMER_TRAM 30 /* timer TRAM shall be greater than 25 seconds and less than 35 seconds */
#define SMS_TIMER_TR2M 16 /* timer TR2M shall be greater than 12 seconds and less than 20 seconds */
} nas_cp_message_t;



int decode_tpdu(tpdu_t *tpdu, uint8_t rp_mti, uint8_t *buf, uint8_t len);
int decode_message_container(nas_cp_message_t *cm, uint8_t length, uint8_t *buf);

/* invoker response for free return value */
uint8_t* encode_cp_ack(uint8_t *len);
uint8_t* encode_rp_ack(uint8_t message_reference, uint8_t *len);
uint8_t* encode_rp_data(uint8_t *sc_address, uint8_t sc_address_len,
    uint8_t *tpud, uint8_t tpudl, uint8_t *len);

int encode_sms_delivery(tpdu_t *tpdu, uint8_t *buf);

int test();

#ifdef __cplusplus
}
#endif

#endif