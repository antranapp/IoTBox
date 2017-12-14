#pragma once

struct NfcTagUid {
    uint8_t version;
    char uid[11]; // null terminated 10 byte string
    unsigned int length;
};
