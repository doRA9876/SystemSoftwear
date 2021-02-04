#include <stdio.h>
#include <stdlib.h>

int split(int n, int w, char** dar, char* src, int del);
char* pickWord(char* token, int tlen, int sep, char* line);
void chomp(char* line);

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
    
    if(length < 0){
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