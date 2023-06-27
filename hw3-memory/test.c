#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ll_double.h"

char *decimal_to_binary(int);
int fromBinary(const char *s);
int main() {
//   int n;
//   char *p;
//   char *l;
//     // char f[] = "heyeye";
//   printf("Enter an integer in decimal number system\n");
//   scanf("%d", &n);

//   p = decimal_to_binary(n);

//   printf("Enter an integer in decimal number system\n");
//   scanf("%d", &n);

//   l = decimal_to_binary(n);
//   strcat(p,l);
//     free(l);
//     int hey = strlen(p);
//     printf("aa %d \n", hey);
//   printf("Binary string of %d is: %s\n", n, p);
    
//   free(p);

//   int n;
//   char *p;
//   char *l;
//   printf("Enter an integer in decimal number system\n");
//   scanf("%d", &n);

//   p = decimal_to_binary(n);

//     printf("Binary string of %d is: %s\n", n, p);
    
//     char yoyo[4];
//     for (int x = 0; x < 4; x++) {
//         yoyo[x] = p[x];
//         printf("%c\n", yoyo[x]);
//     }
    // int ff = fromBinary(p);
        // printf("Binary string of %d\n",ff);


    // free(p);

    // struct list stack_frame;
    // ll_init(&stack_frame);

    // struct node* new_node;
    // new_node->data = 'a';
    
    // ll_insert_head(&stack_frame, 'a');
    // struct char* new_node = ll_remove_head(&stack_frame);
    // char s = *new_node->data;
    // printf("here %s \n", (new_node->data));
    // printf("almost \n"); 

    int fafa = 1012;
    printf("%d", fafa/9);
  return 0;
}


int char_to_binary(const char *s) {
  return (int) strtol(s, NULL, 2);
}

char *decimal_to_binary(int n){
  int c, d, t;
  char *p;

  t = 0;
  p = (char*)malloc(8+1);

  if (p == NULL)
    exit(EXIT_FAILURE);

  for (c = 7 ; c >= 0 ; c--)
  {
    d = n >> c;

    if (d & 1)
      *(p+t) = 1 + '0';
    else
      *(p+t) = 0 + '0';
    t++;
  }
  *(p+t) = '\0';

  return  p;
}