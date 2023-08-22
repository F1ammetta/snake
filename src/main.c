// Copyright 2023 Sergio Piñeres
#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/std/stdio.h>
#include <gint/std/stdlib.h>
#include <time.h>

int width = 128;
int height = 64;

int score = 0;

int block_size = 5;

enum Direction { UP, DOWN, LEFT, RIGHT };

typedef enum Direction Direction;

struct Node {
  int x;
  int y;
  Direction dir;
  struct Node *next;
};

int main();

typedef struct Node Node;

void createNode(Node *head) {
  Node *newNode = malloc(sizeof(Node));
  newNode->next = NULL;

  Node *current = head;
  while (current->next != NULL) {
    current = current->next;
  }

  switch (current->dir) {
  case UP:
    newNode->x = current->x;
    newNode->y = current->y + block_size;
    break;
  case DOWN:
    newNode->x = current->x;
    newNode->y = current->y - block_size;
    break;
  case LEFT:
    newNode->x = current->x + block_size;
    newNode->y = current->y;
    break;
  case RIGHT:
    newNode->x = current->x - block_size;
    newNode->y = current->y;
    break;
  }

  newNode->dir = current->dir;
  current->next = newNode;
}

void spawn_food(Node *head, int *food) {
  Node *current = head;
  srand(time(NULL));
  int x = rand() % (width / block_size);
  int y = rand() % (height / block_size);
  // while (current->next != NULL) {
  //   if (current->x == x && current->y == y) {
  //     x = rand() % (width / block_size);
  //     y = rand() % (height / block_size);
  //     current = head;
  //   }
  //   current = current->next;
  // }
  food[0] = x;
  food[1] = y;
}

void move_snake(Node *head) {
  Node *current = head;

  switch (current->dir) {
  case UP:
    current->y -= block_size;
    break;
  case DOWN:
    current->y += block_size;
    break;
  case LEFT:
    current->x -= block_size;
    break;
  case RIGHT:
    current->x += block_size;
    break;
  }

  while (current->next != NULL) {
    Node *next = current->next;
    next->x = current->x;
    next->y = current->y;
    next->dir = current->dir;
  }
}

void check_food(Node *head, int *food) {
  Node *current = head;
  if (current->x == food[0] && current->y == food[1]) {
    score++;
    spawn_food(head, food);
    createNode(head);
  }
}

void game_over() {
  dclear(C_WHITE);
  dtext(0, 0, C_BLACK, "Game Over");
  dupdate();
  // Node *current = head;
  // while (current->next != NULL) {
  //   Node *temp = current;
  //   current = current->next;
  //   free(temp);
  // }
  // free(food);
  key_event_t key = getkey();
  switch (key.key) {
  case KEY_EXIT:
    exit(0);
    break;
  case KEY_EXE:
    main();
    break;
  }
}

void check_collision(Node *head) {
  Node *current = head;
  while (current->next != NULL) {
    if (current->x == current->next->x && current->y == current->next->y) {
      game_over();
    }
    current = current->next;
  }
}

void draw_screen(Node *head, int *food) {
  dclear(C_WHITE);
  Node *current = head;
  drect(current->x, current->y, current->x + block_size,
        current->y + block_size, C_BLACK);
  while (current->next != NULL) {
    current = current->next;
    drect(current->x, current->y, current->x + block_size,
          current->y + block_size, C_BLACK);
    current = current->next;
  }
  drect(food[0] + 1, food[1] + 1, food[0] + block_size - 1,
        food[1] + block_size - 1, C_BLACK);
  dupdate();
  char text[10];
  sprintf(text, "Score: %d", score);
  dtext(0, 55, C_BLACK, text);
  dupdate();
}

void check_bounds(Node *head) {
  Node *current = head;
  if (current->x < 0 || current->x > width || current->y < 0 ||
      current->y > height) {
    game_over();
  }
}

void change_dir(Node *head) {
  // if (keydown(KEY_UP)) {
  //   head->dir = UP;
  // }
  // if (keydown(KEY_DOWN)) {
  //   head->dir = DOWN;
  // }
  // if (keydown(KEY_LEFT)) {
  //   head->dir = LEFT;
  // }
  // if (keydown(KEY_RIGHT)) {
  //   head->dir = RIGHT;
  // }
  key_event_t key = getkey();
  switch (key.key) {
  case KEY_UP:
    head->dir = UP;
    break;
  case KEY_DOWN:
    head->dir = DOWN;
    break;
  case KEY_LEFT:
    head->dir = LEFT;
    break;
  case KEY_RIGHT:
    head->dir = RIGHT;
    break;
  }
}

void delay(int milliseconds, Node *head) {
  clock_t start_time = clock();
  while (clock() < start_time + milliseconds * 1000) {
    change_dir(head);
  }
}

int main(void) {
  dclear(C_WHITE);
  dupdate();

  Node head;

  head.x = 20;
  head.y = 20;
  head.dir = RIGHT;
  head.next = NULL;

  int *food = malloc(sizeof(int) * 2);
  spawn_food(&head, food);

  while (1) {
    move_snake(&head);
    check_food(&head, food);
    // check_collision(&head);
    check_bounds(&head);
    draw_screen(&head, food);
    change_dir(&head);
    // delay(300, &head);
    dupdate();
  }

  return 1;
}
