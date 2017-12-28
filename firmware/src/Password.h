#pragma once

struct Password {
    uint8_t version;
    char value[13]; // null terminated 12 byte string
    unsigned int length;
};
