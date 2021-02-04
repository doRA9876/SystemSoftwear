#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief 各文字列を結合記号で結合する。
 *
 * @param outStrLen :出力する文字列の長さ（確保してある領域の長さ）
 * @param dst :出力先のアドレス
 * @param joinChar : 文字列間の結合記号
 * @param inStrNum :入力された文字列の個数
 * @param inStr :入力された文字列
 * @return int : 出力される文字列
 */
int join(int outStrLen, char* dst, int joinChar, int inStrNum, char** inStr) {
  //出力する文字列の長さが1以下の時何もすることがないので終了
  if (outStrLen < 2)
    return 0;

  //どうせ最後の文字には終端記号が入るため-1する。
  outStrLen = outStrLen - 1;

  int sum = 0;
  for (int i = 0; i < inStrNum; i++) {
    char* str = inStr[i];
    while (*str != '\0') {
      str++;
      sum++;
    }
    sum++;
  }
  sum--;

  char* line = dst;
  if (sum < outStrLen) {
    for (int i = 0; i < inStrNum; i++) {
      char* str = inStr[i];
      while (*str != '\0') {
        *line++ = *str++;
      }
      *line++ = joinChar;
    }
  } else {
    sum = outStrLen;
    for (int i = 0; i < inStrNum; i++) {
      char* str = inStr[i];
      while (*str != '\0') {
        *line++ = *str++;
        sum--;
        if (sum < 0)
          goto out;
      }
      *line++ = joinChar;
      sum--;
      if (sum < 1)
        goto out;
    }
  }
out:
  *(--line) = '\0';
  return sum;
}