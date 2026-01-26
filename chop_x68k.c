#include <stdio.h>

#include "chop.h"

/**
 * @brief 半角文字の処理
 * @param c 文字
 * @return 出力した幅
 */
static int process_hankaku(const unsigned char c) {
  int ret = 0;
  if (c < 0x20 || c == 0x7F) {
    // Control characters are not printed
  } else {
    // Print ASCII character
    putchar(c);
    ret = 1;
  }
  return ret;
}

/**
 * @brief 全角文字の処理
 * @param c1 1 バイト目
 * @param c2 2 バイト目
 * @return 出力した幅
 */
static int process_zenkaku(const unsigned char c1, const unsigned char c2) {
  // For simplicity, just print the two bytes representing the Zenkaku character
  putchar(c1);
  putchar(c2);
  return 2;  // Zenkaku characters occupy width of 2
}

/**
 * @brief 2 バイト半角文字の処理
 * @param c1 1 バイト目
 * @param c2 2 バイト目
 * @return 出力した幅
 */
static int process_two_bytes_hankaku(const unsigned char c1,
                                     const unsigned char c2) {
  putchar(c1);
  putchar(c2);
  return 1;  // Treat as hankaku width of 1
}

int chop(const char* s, const int width) {
  int current_width = 0;
  unsigned char c;
  while ((c = (unsigned char)*s++) != '\0') {
    if (current_width >= width) {
      break;
    }
    if (c == '\t') {
      int spaces = 8 - (current_width % 8);
      if (current_width + spaces > width) {
        break;  // Not enough space for tab expansion
      }
      putchar(c);
      current_width += spaces;
    } else if ((c < 0x80) || ((c >= 0xA0) && (c <= 0xDF))) {
      current_width += process_hankaku(c);
    } else {
      unsigned char c2 = (unsigned char)*s++;
      if (!c2) {
        break;  // Prevent reading beyond the string
      }
      if ((c == 0x80) || (c >= 0xf0)) {
        current_width += process_two_bytes_hankaku(c, c2);
      } else {
        if (current_width + 2 > width) {
          break;  // Not enough space for a zenkaku character
        }
        current_width += process_zenkaku(c, c2);
      }
    }
  }
  //   {
  //     putchar('\n');
  //     for (int i = 0; i < width; i++) {
  //       putchar("1234567890"[i % 10]);
  //     }
  //     putchar('\n');
  //   }

  return current_width;
}
