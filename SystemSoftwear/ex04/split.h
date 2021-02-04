#define NF (10)

enum TOKEN { ARGUMENT, IN, OUT, REDIRECT_RIGHT, REDIRECT_LEFT };

typedef struct Word {
  char* str;
  enum TOKEN token;
  struct Word* next;
} word_t;