#include "split.h"

#include <stdio.h>
#include <stdlib.h>

int split(int n, int w, char** dar, char* src, int del);
char* pickWord(char* token, int tlen, int sep, char* line);
void chomp(char* line);
int merge_split_shell(word_t*** head_list, char* src);
int split_bar(int chunkNum, int chunkLen, char** dar, char* src);
int split_shell(word_t** head, char* src);
word_t* createWordList(char* src);
void addWordList(word_t** head, word_t* w);
void copyWord(word_t* w, char* src, long head, long tail);
void deleteList(word_t* h);
void showWordList(word_t* m);

/**
 * @brief 文字列を区切り文字で区切る。
 *
 * @param n : 保存先のチャンクの数
 * @param w : 保存先のチャンクの長さ
 * @param dar : 切り出し結果文字列
 * @param src : 切り出し元文字列
 * @param del : 区切り文字
 *
 * @return int : チャンクの数
 */
int split(int chunkNum, int chunkLen, char** dar, char* src, int del) {
  char* pos = src;
  int index = 0;

  while (*pos) {
    if (chunkNum <= index)
      break;
    char* privious_pos = pos;
    pos = pickWord(dar[index], chunkLen, del, pos);
    if (privious_pos != pos)
      index++;
    if (*pos)
      pos++;
  }

  return index;
}

/**
 * @brief 区切り文字までのワードを返す。
 *
 * @param token : ワードの保存先の先頭ポインタ
 * @param tlen : 保存先の最大長
 * @param sep : 区切り文字
 * @param line : 切り出し文の先頭ポインタ
 * @return char* : 切り出し位置のポインタ
 */
char* pickWord(char* token, int tlen, int sep, char* line) {
  char *p = line, *q = token;
  int c = 0;

  while (c < tlen && *p && *p != sep) {
    *q++ = *p++;
    c++;
  }
  *q = '\0';
  return p;
}

/**
 * @brief 改行・復帰文字を削除
 *
 * @param : 文字列
 */
void chomp(char* line) {
  char* p = line;
  while (*p) {
    if (*p == '\n' || *p == '\r') {
      *p = '\0';
      break;
    }
    p++;
  }
}

int merge_split_shell(word_t*** head_list, char* src) {
  char** chunk = (char**)malloc(sizeof(char*) * NF);
  if (!chunk) {
    exit(3);
  }
  for (int i = 0; i < NF; i++) {
    chunk[i] = (char*)malloc(sizeof(char) * BUFSIZ);
    if (!chunk[i]) {
      exit(5);
    }
    int m = rand() % 36 + 3;
    int j;
    for (j = 0; j < m; j++) {
      chunk[i][j] = 'A' + (rand() % 26);
    }
    chunk[i][j] = '\0';
  }

  int ik = split_bar(NF, BUFSIZ, chunk, src);
  *head_list = (word_t**)malloc(sizeof(word_t*) * ik);
  if (!*head_list) {
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < ik; i++) {
    word_t* h;
    if (0 < split_shell(&h, chunk[i])) {
      *(*head_list + i) = h;
    } else {
      return -1;
    }
  }
  return ik;
}

/**
 * @brief 文字列を縦棒で分解
 *
 * @param chunkNum :保存先のチャンクの数
 * @param chunkLen :保存先のチャンクの長さ
 * @param dar :切り出し結果文字列
 * @param src :切り出し元文字列
 * @return int 分割数
 */
int split_bar(int chunkNum, int chunkLen, char** dar, char* src) {
  int num1 = split(chunkNum, chunkLen, dar, src, '|');
  int num2 = num1;

  for (int i = 0, j = 0; i < num1; i++, j++) {
    int head = 0, length = 0;
    char* p = dar[i];

    while (*p == ' ' && *p != '\0') {
      p++;
      head++;
    }

    while (*p != '\0') {
      p++;
      length++;
    }
    p--;

    while (*p == ' ') {
      p--;
      length--;
    }
    // printf("num:%d, head:%d, length:%d\n",i, head, length);

    if (length < 0) {
      j--;
      num2--;
      continue;
    }

    p = dar[j];
    char* q = dar[i];
    q += head;
    for (int k = 0; k < length; k++) {
      *p++ = *q++;
    }
    *p = '\0';
  }
  return num2;
}

/**
 * @brief シェル独特のルールに従って分解。
 *
 * @param head : 線形リストの先頭ポインタword_t*のポインタ
 * @param src : 分解するシェル文字列
 * @return int : 成功時：1, 失敗時：0
 */
int split_shell(word_t** head, char* src) {
  *head = createWordList(src);

  word_t* p = *head;
  while (p != NULL) {
    if (p->token == REDIRECT_LEFT || p->token == REDIRECT_RIGHT) {
      if (p->next == NULL) {
        printf("syntax error\n");
        return -1;
      }
    }

    if (p->token == REDIRECT_LEFT)
      p->next->token = IN;

    if (p->token == REDIRECT_RIGHT)
      p->next->token = OUT;

    p = p->next;
  }
  return 1;
}

/**
 * @brief word_tのメタデータを入力文字列に従って生成
 *
 * @param src : 文字列
 * @return wordmeta_t(meta data)
 */
word_t* createWordList(char* src) {
  word_t* HEAD = NULL;
  char* str_p = src;

  while (*str_p != '\0') {
    while (*str_p == ' ')
      str_p++;

    word_t* tmp = (word_t*)malloc(sizeof(word_t));
    if (!tmp) {
      exit(EXIT_FAILURE);
    }
    long head = 0, tail = 0;
    tmp->next = NULL;

    if (*str_p == '<' || *str_p == '>') {
      head = tail = str_p - src;
      if (*str_p == '<') {
        tmp->token = REDIRECT_LEFT;
      } else {
        tmp->token = REDIRECT_RIGHT;
      }
      str_p++;
    } else {
      head = str_p - src;
      while (*str_p && *str_p != ' ' && *str_p != '<' && *str_p != '>')
        str_p++;
      tail = (str_p - 1) - src;
      tmp->token = ARGUMENT;
    }
    copyWord(tmp, src, head, tail);
    addWordList(&HEAD, tmp);
  }
  return HEAD;
}

/**
 * @brief 線形リストword_tの最後尾にデータを追加
 *
 * @param head : 線形リストの先頭ポインタword_t*のポインタ
 * @param w : 追加するデータ
 */
void addWordList(word_t** head, word_t* w) {
  if (*head == NULL) {
    *head = w;
  } else {
    word_t* p = *head;
    word_t* q = p;
    while (p != NULL) {
      q = p;
      p = p->next;
    }
    q->next = w;
  }
}

/**
 * @brief シェル中の文字列を先頭位置と後尾位置に従って、word_tに文字列を格納
 *
 * @param w : word_tデータ
 * @param src : 文字列
 * @param head : 文字列の先頭位置
 * @param tail : 文字列の後尾位置
 */
void copyWord(word_t* w, char* src, long head, long tail) {
  long index = head;
  long length = tail - head + 2;
  w->str = (char*)malloc(sizeof(char) * length);
  if (!w->str) {
    exit(EXIT_FAILURE);
  }
  char* s = w->str;
  while (index <= tail) {
    *s = src[index];
    s++;
    index++;
  }
  *s = '\0';
}

/**
 * @brief リストのメモリ開放
 *
 * @param h リストの先頭アドレス
 */
void deleteList(word_t* h) {
  while (h != NULL) {
    word_t* p = h;
    h = h->next;
    free(p);
  }
}

/**
 * @brief 指定のフォーマットに出力
 *
 * @param m 出力する線形リストword_tの先頭ポインタ
 */
void showWordList(word_t* m) {
  word_t* p = m;
  int arg_count = 0;
  while (p != NULL) {
    switch (p->token) {
      case REDIRECT_RIGHT:
        p = p->next;
        continue;
        break;
      case REDIRECT_LEFT:
        p = p->next;
        continue;
        break;
      case ARGUMENT:
        printf("arg  %d ", arg_count);
        arg_count++;
        break;
      case IN:
        printf("in     ");
        break;
      case OUT:
        printf("out    ");
        break;
      default:
        printf(" ");
        break;
    }
    printf("|%s|\n", p->str);
    p = p->next;
  }
}