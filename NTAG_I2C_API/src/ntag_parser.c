//
// Created by nirao on 13.04.17.
//

/*
 * ndef_parser.c
 *
 *  Created on: 10/7/2015
 *      Author: Jordi
 */

#include <memory.h>
#include <core_ca9.h>
#include <stdio.h>
#include <stdlib.h>
#include "inc/ntag_parser.h"



void parse_ndef( uint8_t rxbuffer[], uint8_t messagebuffer[],uint8_t *messagebuffer_size)
{
    uint8_t tnf = rxbuffer[2] & 0x07 ;
    uint8_t payload_lenght = rxbuffer[4];

    printf("tnf %x\r\n", tnf);
    if (tnf == 0x01) //well know type
    {
        if(rxbuffer[5] == 'T') //text
        {
            *messagebuffer_size = payload_lenght-3;
            memcpy(messagebuffer+16, rxbuffer+9, *messagebuffer_size);
        }
        if(rxbuffer[5] == 'U') //uri
        {
            uint8_t uriText[26];
            uint8_t *uriIDLength=0;
            memset(uriText, 0, 26);

            getUriType(rxbuffer[6], uriText, &uriIDLength);
            memcpy(messagebuffer+16, uriText, uriIDLength);

            uint8_t uri = uriIDLength;
            memcpy(messagebuffer+16+ uri, rxbuffer+7, (payload_lenght-1));
            *messagebuffer_size = uriIDLength + (payload_lenght-1);

        }
        if(rxbuffer[5] == 'S') //smartposter
        {
            uint8_t textsize= rxbuffer[9]-3;
            memcpy(messagebuffer+16, rxbuffer+14, textsize);

            uint8_t finistext= rxbuffer[9] + 9 + 1;
            uint8_t uritypestarts = finistext + 5;

            uint8_t uriText[26];
            uint8_t *uriIDLength=0;
            memset(uriText, 0, 26);

            getUriType(rxbuffer[uritypestarts], uriText, &uriIDLength);
            memcpy(messagebuffer+16+textsize+1, uriText, uriIDLength);


            uint8_t uristarts= uritypestarts+1;
            uint8_t urisize= rxbuffer[uritypestarts-2];
            uint8_t uri = uriIDLength;

            memcpy(messagebuffer+16+ textsize+ uri +1 , rxbuffer + uristarts, (urisize-1));
            *messagebuffer_size = textsize +uri + (urisize-1);

        }
        if((rxbuffer[5] == 'H') && (rxbuffer[6] == 's')) //BT handover
        {
            uint8_t recordlength = rxbuffer[18];
            uint8_t recordstart = 20;

            memcpy(messagebuffer+16, "Handover Select Record ",23);
            memcpy(messagebuffer+16+23, rxbuffer+recordstart, recordlength);
            *messagebuffer_size = 23 + recordlength;
        }
    }

}

bool check_ndef_presence(uint8_t rxbuffer[])
{
    return 1;
}
void getUriType(uint8_t uriType, uint8_t uriText[], uint8_t* uriLength)
{
    switch (uriType)
    {
        case 0x01:
            *uriLength = 11;
            memcpy(uriText, "http://www.", *uriLength);

            break;
        case 0x02:
            *uriLength = 12;
            memcpy(uriText, "https://www.", *uriLength);
            break;

        case 0x03:
            *uriLength = 7;
            memcpy(uriText, "http://", *uriLength);
            break;
        case 0x04:
            *uriLength = 8;
            memcpy(uriText, "https://", *uriLength);
            break;
        case 0x05:
            *uriLength = 4;
            memcpy(uriText, "tel:", *uriLength);
            break;
        case 0x06:
            *uriLength = 7;
            memcpy(uriText, "mailto:", *uriLength);
            break;
        case 0x07:
            *uriLength = 26;
            memcpy(uriText, "ftp://anonymous:anonymous@", *uriLength);
            break;
        case 0x08:
            *uriLength = 10;
            memcpy(uriText, "ftp://ftp.", *uriLength);
            break;
        case 0x09:
            *uriLength = 7;
            memcpy(uriText, "ftps://", *uriLength);
            break;
        case 0x0A:
            *uriLength = 7;
            memcpy(uriText, "sftp://", *uriLength);
            break;
        case 0x0B:
            *uriLength = 6;
            memcpy(uriText, "smb://", *uriLength);
            break;
        case 0x0C:
            *uriLength = 6;
            memcpy(uriText, "nfs://", *uriLength);
            break;
        case 0x0D:
            *uriLength = 6;
            memcpy(uriText, "ftp://", *uriLength);
            break;
        case 0x0E:
            *uriLength = 6;
            memcpy(uriText, "dav://", *uriLength);
            break;
        case 0x0F:
            *uriLength = 5;
            memcpy(uriText, "news:", *uriLength);
            break;
        case 0x10:
            *uriLength = 9;
            memcpy(uriText, "telnet://", *uriLength);
            break;
        case 0x11:
            *uriLength = 5;
            memcpy(uriText, "imap:", *uriLength);
            break;
        case 0x12:
            *uriLength = 7;
            memcpy(uriText, "rtsp://", *uriLength);
            break;
        case 0x13:
            *uriLength = 4;
            memcpy(uriText, "urn:", *uriLength);
            break;
        case 0x14:
            *uriLength = 4;
            memcpy(uriText, "pop:", *uriLength);
            break;
        case 0x15:
            *uriLength = 4;
            memcpy(uriText, "sip:", *uriLength);
            break;
        case 0x16:
            *uriLength = 5;
            memcpy(uriText, "sips:", *uriLength);
            break;
        case 0x17:
            *uriLength = 5;
            memcpy(uriText, "tftp:", *uriLength);
            break;
        case 0x18:
            *uriLength = 8;
            memcpy(uriText, "btspp://", *uriLength);
            break;
        case 0x19:
            *uriLength = 10;
            memcpy(uriText, "btl2cap://", *uriLength);
            break;
        case 0x1A:
            *uriLength = 9;
            memcpy(uriText, "btgoep://", *uriLength);
            break;
        case 0x1B:
            *uriLength = 10;
            memcpy(uriText, "tcpobex://", *uriLength);
            break;
        case 0x1C:
            *uriLength = 11;
            memcpy(uriText, "irdaobex://", *uriLength);
            break;
        case 0x1D:
            *uriLength = 7;
            memcpy(uriText, "file://", *uriLength);
            break;
        case 0x1E:
            *uriLength = 11;
            memcpy(uriText, "urn:epc:id:", *uriLength);
            break;
        case 0x1F:
            *uriLength = 12;
            memcpy(uriText, "urn:epc:tag:", *uriLength);
            break;
        case 0x20:
            *uriLength = 12;
            memcpy(uriText, "urn:epc:pat:", *uriLength);
            break;
        case 0x21:
            *uriLength = 12;
            memcpy(uriText, "urn:epc:raw:", *uriLength);
            break;
        case 0x22:
            *uriLength = 8;
            memcpy(uriText, "urn:epc:", *uriLength);
            break;
        case 0x23:
            *uriLength = 11;
            memcpy(uriText, "urn:epc:raw", *uriLength);
            break;
//	default:
//		return NFCSTATUS_INVALID_FORMAT;
    }


}

//NdefRecord r = NdefRecord();
//r.setTnf(TNF_WELL_KNOWN);
//
//uint8_t RTD_TEXT[1] = { 0x54 }; // TODO this should be a constant or preprocessor
//r.setType(RTD_TEXT, sizeof(RTD_TEXT));
//
//// X is a placeholder for encoding length
//// TODO is it more efficient to build w/o string concatenation?
//String payloadString = "X" + encoding + text;
//
//byte payload[payloadString.length() + 1];
//payloadString.getBytes(payload, sizeof(payload));
//
//// replace X with the real encoding length
//payload[0] = encoding.length();
//
//r.setPayload(payload, payloadString.length());
//
//
//
//
//createRecord(String text) throws UnsupportedEncodingException {
//
//    //create the message in according with the standard
//    String lang = "en";
//    byte[] textBytes = text.getBytes();
//    byte[] langBytes = lang.getBytes("US-ASCII");
//    int langLength = langBytes.length;
//    int textLength = textBytes.length;
//
//    byte[] payload = new byte[1 + langLength + textLength];
//    payload[0] = (byte) langLength;
//
//    // copy langbytes and textbytes into payload
//    System.arraycopy(langBytes, 0, payload, 1, langLength);
//    System.arraycopy(textBytes, 0, payload, 1 + langLength, textLength);
//
//    NdefRecord recordNFC = new NdefRecord(NdefRecord.TNF_WELL_KNOWN, NdefRecord.RTD_TEXT, new byte[0], payload);
//    return recordNFC;
//}

//uint32_t createNDEFShortRecord(uint8_t *message, uint8_t payloadLen, uint8_t *NDEFMessage)
//{
////Serial.print("Message: ");
////Serial.println((char *)message);
//uint8_t * NDEFMessageHdr = ALLOCATE_HEADER_SPACE(NDEFMessage, NDEF_SHORT_RECORD_MESSAGE_HDR_LEN);
//
//NDEFMessageHdr[0] =  NDEF_MESSAGE_BEGIN_FLAG | NDEF_MESSAGE_END_FLAG | NDEF_MESSAGE_SHORT_RECORD | TYPE_FORMAT_MEDIA_TYPE;
//NDEFMessageHdr[1] =  SHORT_RECORD_TYPE_LEN;
//NDEFMessageHdr[2] =  payloadLen;
//memcpy(&NDEFMessageHdr[3], TYPE_STR, SHORT_RECORD_TYPE_LEN);
//memcpy(NDEFMessage, message, payloadLen);
////Serial.print("NDEF Message: ");
////Serial.println((char *)NDEFMessage);
//NDEFMessage = NDEFMessageHdr;
//return (payloadLen + NDEF_SHORT_RECORD_MESSAGE_HDR_LEN);
//}
