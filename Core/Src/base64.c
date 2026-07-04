#include <stdint.h>
#include <string.h>

// Base64 字符映射表
static const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * @brief 计算 Base64 编码后的长度（包含结尾的 \0）
 */
static int get_base64_len(int length) {
    return ((length / 3) + (length % 3 > 0 ? 1 : 0)) * 4 + 1;
}

/**
 * @brief 无动态内存分配的 Base64 编码函数
 * @param src      源数据指针
 * @param length   源数据长度
 * @param dest     目标缓冲区指针（由调用者分配）
 * @param dest_len 目标缓冲区的最大容量
 * @return         成功返回编码后的实际长度（不含 \0），失败返回 -1
 */
int base64_encode_safe(const uint8_t *src, int length, char *dest, int dest_len) {
    // 1. 检查目标缓冲区是否足够大
    int required_len = get_base64_len(length);
    if (dest_len < required_len) {
        return -1; // 目标缓冲区太小，拒绝写入
    }

    int i, j = 0;
    uint8_t a, b, c;

    // 2. 处理完整的 3 字节组
    for (i = 0; i < length / 3; i++) {
        a = src[i * 3];
        b = src[i * 3 + 1];
        c = src[i * 3 + 2];

        dest[j++] = base64_table[(a >> 2) & 0x3F];
        dest[j++] = base64_table[((a & 0x03) << 4) | ((b >> 4) & 0x0F)];
        dest[j++] = base64_table[((b & 0x0F) << 2) | ((c >> 6) & 0x03)];
        dest[j++] = base64_table[c & 0x3F];
    }

    // 3. 处理不足 3 字节的尾部数据
    if (length % 3 == 1) {
        a = src[i * 3];
        dest[j++] = base64_table[(a >> 2) & 0x3F];
        dest[j++] = base64_table[(a & 0x03) << 4];
        dest[j++] = '=';
        dest[j++] = '=';
    } else if (length % 3 == 2) {
        a = src[i * 3];
        b = src[i * 3 + 1];
        dest[j++] = base64_table[(a >> 2) & 0x3F];
        dest[j++] = base64_table[((a & 0x03) << 4) | ((b >> 4) & 0x0F)];
        dest[j++] = base64_table[(b & 0x0F) << 2];
        dest[j++] = '=';
    }

    dest[j] = '\0'; // 确保字符串正确结束
    return j;       // 返回编码后的有效长度
}

/**
 * @brief 获取 Base64 字符对应的数值 (0-63)
 */
static uint8_t get_base64_value(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return 0; // 遇到 '=' 或其他非法字符返回 0
}

/**
 * @brief 无动态内存分配的 Base64 解码函数
 * @param src      源 Base64 字符串指针
 * @param length   源 Base64 字符串的长度
 * @param dest     目标缓冲区指针（由调用者分配）
 * @param dest_len 目标缓冲区的最大容量
 * @return         成功返回解码后的实际字节长度，失败返回 -1
 */
int base64_decode_safe(const char *src, int length, uint8_t *dest, int dest_len) {
    // 1. 长度必须是 4 的倍数，否则格式错误
    if (length % 4 != 0) {
        return -1; 
    }

    // 2. 计算解码后的实际长度
    int decoded_len = (length / 4) * 3;
    if (src[length - 1] == '=') decoded_len--;
    if (src[length - 2] == '=') decoded_len--;

    // 3. 检查目标缓冲区是否足够大
    if (dest_len < decoded_len) {
        return -1; // 目标缓冲区太小，拒绝写入
    }

    int i, j = 0;
    uint8_t a, b, c, d;

    // 4. 每次处理 4 个字符，还原为 3 个字节
    for (i = 0; i < length; i += 4) {
        a = get_base64_value(src[i]);
        b = get_base64_value(src[i + 1]);
        c = get_base64_value(src[i + 2]);
        d = get_base64_value(src[i + 3]);

        dest[j++] = (a << 2) | (b >> 4);
        
        // 只有当不是第一个 '=' 填充符时，才写入第二个字节
        if (src[i + 2] != '=') {
            dest[j++] = ((b & 0x0F) << 4) | (c >> 2);
        }
        
        // 只有当不是第二个 '=' 填充符时，才写入第三个字节
        if (src[i + 3] != '=') {
            dest[j++] = ((c & 0x03) << 6) | d;
        }
    }

    return j; // 返回解码后的有效字节长度
}