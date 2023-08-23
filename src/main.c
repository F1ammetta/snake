// Copyright 2023 Sergio Piñeres
#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/std/stdio.h>
#include <gint/std/stdlib.h>
#include <math.h>
#include <time.h>

int width = 126;
int height = 54;

int score = 0;

int block_size = 2;

enum Direction { UP, DOWN, LEFT, RIGHT };

typedef enum Direction Direction;

struct Node {
  int x;
  int y;
  Direction dir;
  struct Node *next;
};

typedef struct Node Node;

int main();
void createNode(Node *head);
void spawn_food(Node *head, int *food);
void move_snake(Node *head);
void check_food(Node *head, int *food);
int contains(Node *head, int *food);
void game_over(Node *head, int *food);
void check_collision(Node *head, int *food);
void draw_screen(Node *head, int *food);
void snap_to_grid(int *food);

void createNode(Node *head) {
  Node *current = head;
  while (current->next != NULL) {
    current = current->next;
  }
  Node *newNode = malloc(sizeof(Node));

  newNode->x = current->x;
  newNode->y = current->y;
  newNode->dir = current->dir;
  newNode->next = NULL;
  current->next = newNode;
}

void snap_to_grid(int *food) {
  food[0] = food[0] - (food[0] % block_size);
  food[1] = food[1] - (food[1] % block_size);
}

void spawn_food(Node *head, int *food) {
  Node *current = head;
  srand(time(NULL));
  int x = rand() % width;
  int y = rand() % height;
  while (current->next != NULL) {
    if (current->x == x && current->y == y) {
      x = rand() % width;
      y = rand() % height;
      current = head;
    }
    current = current->next;
  }
  food[0] = x;
  food[1] = y;
  snap_to_grid(food);
}

void move_snake(Node *head) {
  Node *current = head;

  int tempx = current->x;
  int tempy = current->y;

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
    int tempx2 = current->next->x;
    int tempy2 = current->next->y;
    current->next->x = tempx;
    current->next->y = tempy;
    tempx = tempx2;
    tempy = tempy2;
    current = current->next;
  }
}

void check_food(Node *head, int *food) {
  if (contains(head, food)) {
    createNode(head);
    spawn_food(head, food);
    score++;
  }
}

int contains(Node *head, int *food) {
  int *corner2 = malloc(sizeof(int) * 2);
  int *corner3 = malloc(sizeof(int) * 2);
  int *corner4 = malloc(sizeof(int) * 2);

  corner2[0] = food[0] + block_size;
  corner2[1] = food[1];

  corner3[0] = food[0];
  corner3[1] = food[1] + block_size;

  corner4[0] = food[0] + block_size;
  corner4[1] = food[1] + block_size;

  int **corners = malloc(sizeof(int *) * 4);

  corners[0] = food;
  corners[1] = corner2;
  corners[2] = corner3;
  corners[3] = corner4;

  for (int i = 0; i < 4; i++) {
    if ((corners[i][0] >= head->x && corners[i][1] >= head->y) &&
        (corners[i][0] <= head->x + block_size &&
         corners[i][1] <= head->y + block_size)) {
      return 1;
    }
  }
  return 0;
}

void game_over(Node *head, int *food) {
  dclear(C_WHITE);
  dtext(0, 0, C_BLACK, "Game Over");
  char text[30];
  sprintf(text, "Score: %d", score);
  dtext(0, 10, C_BLACK, text);
  dupdate();
  Node *current = head;
  while (current->next != NULL) {
    Node *temp = current;
    current = current->next;
    free(temp);
  }
  free(food);
  while (getkey().key != KEY_EXIT || getkey().key != KEY_EXE) {
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
}

void check_collision(Node *head, int *food) {
  Node *current = head;
  while (current->next != NULL) {
    if (head->x == current->next->x && head->y == current->next->y) {
      game_over(head, food);
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
    drect(current->next->x, current->next->y, current->next->x + block_size,
          current->next->y + block_size, C_BLACK);
    current = current->next;
  }
  for (int i = 0; i < width; i += block_size) {
    dpixel(i, 0, C_BLACK);
    dpixel(i, height, C_BLACK);
  }
  for (int i = 0; i < height; i += block_size) {
    dpixel(0, i, C_BLACK);
    dpixel(width, i, C_BLACK);
  }

  drect(food[0], food[1], food[0] + block_size, food[1] + block_size, C_BLACK);
  dupdate();
  char text[10];
  sprintf(text, "Score: %d", score);
  dtext(0, 55, C_BLACK, text);
  dupdate();
}

void check_bounds(Node *head, int *food) {
  Node *current = head;
  if (current->x < 0 || current->x + block_size > width || current->y < 0 ||
      current->y + block_size > height) {
    game_over(head, food);
  }
}

void change_dir(Node *head) {
  clearevents();
  if (keydown(KEY_UP) || keydown(KEY_8)) {
    if (head->dir != DOWN) {
      head->dir = UP;
    }
  }
  if (keydown(KEY_DOWN) || keydown(KEY_2)) {
    if (head->dir != UP) {
      head->dir = DOWN;
    }
  }
  if (keydown(KEY_LEFT) || keydown(KEY_4)) {
    if (head->dir != RIGHT) {
      head->dir = LEFT;
    }
  }
  if (keydown(KEY_RIGHT) || keydown(KEY_6)) {
    if (head->dir != LEFT) {
      head->dir = RIGHT;
    }
  }
  if (keydown(KEY_EXIT) || keydown(KEY_MENU)) {
    exit(0);
  }
}

void delay(int milliseconds, Node *head) {
  clock_t start_time = clock();
  while (clock() < start_time + milliseconds * 1000) {
    change_dir(head);
  }
}

double f(int x) {
  double y = 200 / (1 + exp(-(x - 50) / 18));
  return floor(y);
}

int main(void) {
  dclear(C_WHITE);
  dupdate();

  Node head;
  score = 0;

  head.x = 20;
  head.y = 20;
  head.dir = RIGHT;
  head.next = NULL;

  int *food = malloc(sizeof(int) * 2);
  spawn_food(&head, food);

  while (1) {
    move_snake(&head);
    check_collision(&head, food);
    check_food(&head, food);
    check_bounds(&head, food);
    draw_screen(&head, food);
    change_dir(&head);
    delay(300 - f(score), &head);
    dupdate();
  }

  return 1;
}
