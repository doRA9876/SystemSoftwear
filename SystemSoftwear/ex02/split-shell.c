#include <stdio.h>
#include <stdlib.h>
#include "split.h"

extern int split_bar(int chunkNum, int chunkLen, char** dar, char* src);

int split_shell(word_t** h, char* src);
word_t* createWordList(char* src);
void addWordList(word_t** m, word_t* w);
void copyWord(word_t* w, char* src, long head, long tail);
void deleteList(word_t* h);
void showWordList(word_t* m);

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
    if(!tmp)
    {
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
    while (p != NULL)
    {
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
  if(!w->str)
  {
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
void deleteList(word_t* h)
{
  while(h != NULL)
  {
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
