# chop

## これはなに？

指定された桁で入力の右側をカットするフィルタです。主に X68000 + Human68k 向けです。

コーディング AI の力を借りながら短時間ででっち上げました。動作確認も不十分です。

## 使い方

```
Usage: chop [OPTION]... [FILE]
Read FILE or standard input and process each line.
If FILE is omitted or '-', read from standard input.

  -w, --width=WIDTH  set the width (default: 96)
  -h, --help     display this help and exit
  -v, --version  output version information and exit
```

- `-w` または `--width` で桁数 (半角換算) を指定します (デフォルト : 96)

## 使用例

```bash
# input.txt を横幅 96 桁で表示する
chop input.txt

# input.txt を標準入力から読み込み、横幅 96 桁で表示する
cat input.txt | chop

# input.txt を標準入力から読み込み、横幅 96 桁で表示する
chop <input.txt

# input.txt を横幅 64 桁で表示する
chop -w64 input.txt

# input.txt を横幅 64 桁で表示する
chop --width 64 input.txt
```

## ソースコードからのビルド

X68000 ではビルドできません。 [elf2x68k](https://github.com/yunkya2/elf2x68k) が必要です。 makefile のあるディレクトリで `make` してください。

## 連絡先

https://github.com/68fpjc/chop
