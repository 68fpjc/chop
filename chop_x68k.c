#include <stdio.h>

#include "chop.h"
#include "x68kcon/x68kcon.h"

int chop(const char* s, const int col, const int width,
         chop_output_context* ctx) {
  int current_width = 0;
  int effective_width = width - col;
  unsigned char c;
  const char* s_tmp = s;

  while ((c = (unsigned char)*s_tmp++) != '\0') {
    if (current_width >= effective_width) {
      break;
    }
    if (c == '\t') {
      int spaces = 8 - (current_width % 8);
      if (current_width + spaces > width) {
        break;  // Not enough space for tab expansion
      }
      ctx->output_func(c, ctx);
      current_width += spaces;
    } else {
      int width_diff;
      unsigned char c2;
      if (!ismbblead_x68k(c)) {
        c2 = c;
        c = 0;
      } else {
        c2 = (unsigned char)*s_tmp++;
      }
      width_diff = char_width_x68k(c, c2);
      if (width_diff == -1) {
        break;  // Invalid character, stop processing
      }
      current_width += width_diff;
      if (current_width > width) {
        break;  // Not enough space for this character
      }
      if (width_diff) {
        if (c) {
          ctx->output_func(c, ctx);
        }
        ctx->output_func(c2, ctx);
      }
    }
  }
  //   {
  //     ctx->output_func('\n', ctx);
  //     for (int i = 0; i < effective_width; i++) {
  //       ctx->output_func("1234567890"[i % 10], ctx);
  //     }
  //     ctx->output_func('\n', ctx);
  //   }

  return current_width;
}
