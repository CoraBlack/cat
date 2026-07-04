#ifndef _BASE64_H_
#define _BASE64_H_

#ifdef __cplusplus
extern "C"{
#endif
#include<stdint.h>
/* --------------------------------------------------------------------------*/
/**
 * @Synopsis base64 encode
 *
 * @Param src data before encode
 * @Param length data length
 *
 * @Returns data after encoded, need free.
 */
/* ----------------------------------------------------------------------------*/
extern int base64_encode_safe(const uint8_t *src, int length, char *dest, int dest_len);

/* --------------------------------------------------------------------------*/
/**
 * @Synopsis base64 decode
 *
 * @Param dest data encoded
 *
 * @Returns data decoded
 */
/* ----------------------------------------------------------------------------*/
extern int base64_decode_safe(const char *src, int length, uint8_t *dest, int dest_len);

#ifdef __cplusplus
}
#endif

#endif // _BASE64_H_
