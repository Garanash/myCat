#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  bool numberNonblank;     // -b
  bool markEndl;           // -e -E
  bool numberAll;          // -n
  bool squeeze;            // -s
  bool tab;                // -t -T
  bool printNonPrintable;  // -v
} Flags;

Flags catReadFlags(int argc, char *argv[]);
bool cat_match_lines(char *line, Flags flags);
void cat_print_symb(char symb, Flags flags);
void catPrint(FILE *file, Flags flags);

int main(int argc, char *argv[]) {
  Flags flags = catReadFlags(argc, argv);
  for (; optind < argc; optind++) {
    FILE *file = fopen(argv[optind], "r");
    catPrint(file, flags);
    fclose(file);
  }
  return 0;
}

bool cat_match_lines(char *line, Flags flags) {
  if (flags.numberNonblank == true) {
    if (strcmp(line, "\n") == 0) {
      return false;
    } else {
      return true;
    }
  } else if (flags.numberAll == true) {
    return true;
  } else {
    return false;
  }
}

void cat_print_symb(char symb, Flags flags) {
  if (flags.tab == true && flags.printNonPrintable == true &&
      flags.markEndl == true) {
    if (symb == 10) {
      printf("$\n");
    } else if (symb == 0) {
      printf("^@");
    } else if (symb < 32) {
      printf("^%c", symb + 64);
    } else if (symb > 126) {
      printf("^%c", symb - 64);
    } else {
      printf("%c", symb);
    }
  } else if (flags.tab == true && flags.printNonPrintable == true) {
    if (symb == 10) {
      printf("\n");
    } else if (symb == 0) {
      printf("^@");
    } else if (symb < 32) {
      printf("^%c", symb + 64);
    } else if (symb > 126) {
      printf("^%c", symb - 64);
    } else {
      printf("%c", symb);
    }
  } else if (flags.printNonPrintable == true && flags.markEndl == true) {
    if (symb == 10) {
      printf("$\n");
    } else if (symb == 0) {
      printf("^@");
    } else if (symb < 32) {
      printf("^%c", symb + 64);
    } else if (symb > 126) {
      printf("^%c", symb - 64);
    } else {
      printf("%c", symb);
    }
  } else if (flags.tab == true) {
    if (symb == '\t') {
      printf("^I");
    } else {
      printf("%c", symb);
    }
  } else if (flags.markEndl == true) {
    if (symb == '\n') {
      printf("$\n");
    } else {
      printf("%c", symb);
    }
  } else {
    printf("%c", symb);
  }
}

Flags catReadFlags(int argc, char *argv[]) {
  struct option longoptions[] = {{"number-nonblank", '0', NULL, 'b'},
                                 {"number", '0', NULL, 'n'},
                                 {"squeeze-blank", '0', NULL, 's'},
                                 {NULL, '0', NULL, '0'}};
  int currentFlag = getopt_long(argc, argv, "bevEnstT", longoptions, NULL);
  Flags flags = {false, false, false, false, false, false};
  for (; currentFlag != -1;
       currentFlag = getopt_long(argc, argv, "bevEnstT", longoptions, NULL)) {
    switch (currentFlag) {
      break;
      case 'b':
        flags.numberNonblank = true;
        break;
      case 'e':
        flags.markEndl = true;
        flags.printNonPrintable = true;
        break;
      case 'E':
        flags.markEndl = true;
        break;
      case 'n':
        flags.numberAll = true;
        break;
      case 's':
        flags.squeeze = true;
        break;
      case 't':
        flags.tab = true;
        flags.printNonPrintable = true;
      case 'T':
        flags.tab = true;
      default:
        break;
    }
  }
  return flags;
}

void catPrint(FILE *file, Flags flags) {
  int number = 1, kp = 0;
  char line[10000];
  while (fgets(line, 255, file)) {
    if (flags.squeeze == true) {
      if (strcmp(line, "\n") == 0) {
        kp++;
        if (kp > 1) {
          continue;
        } else {
          if (cat_match_lines(line, flags) == true) {
            printf("%6i\t", number);
            number++;
          }
          for (char *symb = &line[0]; *symb != '\0'; symb++) {
            cat_print_symb(*symb, flags);
          }
        }
      } else {
        kp = 0;
        if (cat_match_lines(line, flags) == true) {
          kp = 0;
          printf("%6i\t", number);
          number++;
        }
        for (char *symb = &line[0]; *symb != '\0'; symb++) {
          cat_print_symb(*symb, flags);
        }
      }
    } else {
      if (cat_match_lines(line, flags) == true) {
        kp = 0;
        printf("%6i\t", number);
        number++;
      }
      for (char *symb = &line[0]; *symb != '\0'; symb++) {
        cat_print_symb(*symb, flags);
      }
    }
  }
}