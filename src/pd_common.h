#pragma once
/// @file pd_common.h
/// @brief General Phantom Dust helper code used in multiple file formats
/// @author Vu (text encoding discovery & research)
/// @author Torphedo (implementation)
/// @author Blasianblazy (improvements & original Python code)

#ifdef __cplusplus
extern "C" {
#endif

#include <common/int.h>

enum {
    // Number of characters encoded in a u32
    ENCODED_CHAR_COUNT = 6,
};

// Useful when decoding larger pieces of text
typedef struct {
    char data[ENCODED_CHAR_COUNT * 2];
    char null_terminator;
}decoded_text;


/// @brief Decode 6 characters from a 32-bit integer.
/// @param output buffer to store decoded characters in
/// @param val value to decode
void decode_single32(char* output, u32 val);

/// @brief Decode 12 characters from 2 integers
decoded_text decode_double(u32 text1, u32 text2);

/// @brief Encode 6 characters into a 32-bit integer.
///
/// The only allowed characters are alphanumeric ASCII, spaces and underscores,
/// and NUL. Periods and dashes will be converted to underscores.
/// @param text string to encode (returns 0 if NULL)
///
/// @return encoded value representing the input string
u32 encode_single32(char* text);

#ifdef __cplusplus
}
#endif
