/* SPDX-License-Identifier: 0BSD */

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chop.h"

/**
 * @brief Prints the help message for the program.
 */
static void print_help(void) {
  printf(  //
      "Usage: " PROGRAM
      " [OPTION]... [FILE]\n"
      "Read FILE or standard input and process each line.\n"
      "If FILE is omitted or '-', read from standard input.\n\n"
      "  -w, --width=WIDTH  set the width (default: 96)\n"
      "  -h, --help     display this help and exit\n"
      "  -v, --version  output version information and exit\n");
}

/**
 * @brief Displays help information message to standard error output.
 */
static void print_help_info(void) {
  fprintf(stderr, "Try '" PROGRAM " --help' for more information.\n");
}

/**
 * @brief 標準出力への出力関数ラッパー
 */
static void stdout_output(int c, chop_output_context* ctx) {
  (void)ctx;  // 標準出力の場合は ctx は使用しない
  putchar(c);
}

/**
 * @brief Processes input from a file or stdin line by line.
 * @param input Input file stream
 * @param width Width for chop processing
 * @return 0 on success, 1 on error
 */
static int process_input(FILE* input, int width) {
  char* line = NULL;
  size_t capacity = 256;
  size_t len = 0;

  line = (char*)malloc(capacity);
  if (line == NULL) {
    perror("malloc");
    return 1;
  }

  while (1) {
    // バッファの残り領域に読み込む
    if (fgets(line + len, capacity - len, input) == NULL) {
      // EOF またはエラー
      if (len > 0) {
        // 最後に読み込んだ行を処理
        chop_output_context ctx = {stdout_output, NULL};
        chop(line, 0, width, &ctx);
        printf("\n");
      }
      break;
    }

    len = strlen(line);

    // 改行が見つかった場合
    if (len > 0 && line[len - 1] == '\n') {
      line[len - 1] = '\0';
      chop_output_context ctx = {stdout_output, NULL};
      chop(line, 0, width, &ctx);
      printf("\n");
      len = 0;
    } else if (len >= capacity - 1) {
      // バッファが満杯（改行がない）→ バッファを拡張
      capacity *= 2;
      char* newline = (char*)realloc(line, capacity);
      if (newline == NULL) {
        perror("realloc");
        free(line);
        return 1;
      }
      line = newline;
    }
  }

  free(line);
  return 0;
}

int main(int argc, char* argv[]) {
  int width = 96;  // デフォルトの幅設定
  while (1) {
    static struct option long_options[] =  //
        {{"help", no_argument, 0, 'h'},
         {"version", no_argument, 0, 'v'},
         {"width", required_argument, 0, 'w'},
         {0, 0, 0, 0}};
    int option_index = 0;
    int c = getopt_long(argc, argv, "hvw:", long_options, &option_index);
    if (c == -1) {
      break;
    }
    switch (c) {
      case 'h':
        print_help();
        return 0;
      case 'v':
        printf(PROGRAM " " VERSION " https://github.com/68fpjc/chop\n");
        return 0;
      case 'w':
        errno = 0;
        char* endptr;
        width = strtol(optarg, &endptr, 10);
        if (endptr == optarg) {
          fprintf(stderr, "Error: No digits were found in '%s'.\n", optarg);
          return 1;
        }
        if (errno == ERANGE || width <= 0) {
          fprintf(stderr, "Error: Value '%s' is out of range.\n", optarg);
          return 1;
        }
        break;
      case '?':
        print_help_info();
        return 1;
      default:
        return 1;
    }
  }

  // ファイル名が指定されているかチェック（複数のファイルは不可）
  if (optind < argc - 1) {
    fprintf(stderr, "Error: Too many arguments.\n");
    print_help_info();
    return 1;
  }

  FILE* input = stdin;
  const char* filename = NULL;

  // ファイル名が指定されている場合
  if (optind < argc) {
    filename = argv[optind];
    // "-" の場合は stdin を使用
    if (filename[0] != '-' || filename[1] != '\0') {
      input = fopen(filename, "r");
      if (input == NULL) {
        perror(filename);
        return 1;
      }
    }
  }

  int result = process_input(input, width);

  // ファイルを開いた場合はクローズ
  if (input != stdin) {
    fclose(input);
  }

  return result;
}
