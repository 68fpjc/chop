PROGRAM = chop
VERSION = 0.1.5-dev

# ビルド成果物の設定
TARGET = $(PROGRAM).x
ARCHIVE = $(PROGRAM)-$(VERSION).zip
DISTDIR = dist

# クロスコンパイル環境の設定
CROSS = m68k-xelf-
CC = $(CROSS)gcc
AS = $(CROSS)as
LD = $(CROSS)gcc

# コンパイルオプション
CFLAGS_COMMON = -m68000 -Wall -Wextra -MMD -D_GNU_SOURCE -DPROGRAM=\"$(PROGRAM)\" -DVERSION=\"$(VERSION)\" -I x68kcon
ifdef RELEASE_BUILD
  CFLAGS = $(CFLAGS_COMMON) -O3  # リリースビルド
else
  CFLAGS = $(CFLAGS_COMMON) -O0 -g  # 開発ビルド : デバッグ情報付き
endif
LDFLAGS = -specs=hupair.specs
OBJS = main.o chop_x68k.o  # コンパイル対象のオブジェクトファイル
LDLIBS =
DEPS = $(patsubst %.o,%.d,$(OBJS))

# ターゲット定義
.PHONY: all extra_headers clean veryclean release bump-version

# デフォルトターゲット : 実行ファイルのビルド
all: extra_headers $(TARGET)

# 実行ファイルのリンク
$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@

x68kcon/x68kcon.h:
	wget -q -P x68kcon/ https://raw.githubusercontent.com/68fpjc/x68kcon/b36f957831ada16ef9aff6fd1656004f8a70ff7a/x68kcon.h

# 依存関係ファイルの取り込み
-include $(DEPS)

extra_headers: x68kcon/x68kcon.h

# 中間ファイルの削除
clean:
	-rm -f *.x *.o *.elf* *.d
	-rm -rf x68kcon/*

# 配布ディレクトリを含めた完全クリーン
veryclean: clean
	-rm -rf $(DISTDIR)

# リリースビルドの作成
release:
	$(MAKE) clean
	$(MAKE) RELEASE_BUILD=yes
	mkdir -p $(DISTDIR)
	mv $(TARGET) $(DISTDIR)
	cp LICENSE $(DISTDIR)
	pandoc -f markdown -t plain README.md | iconv -t cp932 >$(DISTDIR)/README.txt
	cd $(DISTDIR) && 7z a $(ARCHIVE) $(TARGET) LICENSE README.txt
	$(MAKE) clean

# バージョン番号の更新
bump-version:
	@echo "Current version: $(VERSION)"
	@read -p "New version: " new_version && \
	sed -i "s/VERSION = $(VERSION)/VERSION = $$new_version/" makefile
