typedef struct findArgs {
  // 2進数 0b001:name 0b010:type 0b100:print
  char option_flag;

  char* file_name;
  unsigned char file_type;

  int path_num;
  char** path;
} findArgs_t;

// for file type determination 
#define T_ISLNK 0b00000001
#define T_ISREG 0b00000010
#define T_ISDIR 0b00000100
#define T_ISCHR 0b00001000
#define T_ISBLK 0b00010000
#define T_ISFIFO 0b00100000
#define T_ISSOCK 0b01000000

// for option determination
#define OPT_NAME 0b00000001
#define OPT_TYPE 0b00000010
#define OPT_PRINT 0b00000100
