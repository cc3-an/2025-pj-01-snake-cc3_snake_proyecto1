#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

// Definiciones de funciones de ayuda.
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Tarea 1 */
game_state_t* create_default_state() {
  game_state_t* state = malloc(sizeof(game_state_t));
  if (!state) return NULL;

  state->num_rows = 18;
  state->num_snakes = 1;
  state->snakes = malloc(sizeof(snake_t));
  if (!state->snakes) {
      free(state);
      return NULL;
  }

  // 2. Inicializar serpiente
  snake_t* snake = &state->snakes[0];
  snake->tail_row = 2;
  snake->tail_col = 2;
  snake->head_row = 2;
  snake->head_col = 4;
  snake->live = true;

  // 3. Crear el tablero (board)
  state->board = malloc(state->num_rows * sizeof(char*));
  if (!state->board) {
      free(state->snakes);
      free(state);
      return NULL;
  }

  for (int i = 0; i < state->num_rows; i++) {
      state->board[i] = malloc(21);  // 20 columnas + 1 para '\0'
      if (!state->board[i]) {
          for (int j = 0; j < i; j++) free(state->board[j]);
          free(state->board);
          free(state->snakes);
          free(state);
          return NULL;
      }

      if (i == 0 || i == state->num_rows - 1) {
          // bordes superior e inferior
          strcpy(state->board[i], "####################");
      } else if (i == 2) {
          // Fila con la serpiente y fruta
          strcpy(state->board[i], "# d>D    *         #");
      } else {
          // Fila vacía
          strcpy(state->board[i], "#                  #");
      }
  }

  return state;
}


/* Tarea 2 */
void free_state(game_state_t* state) {
  if (!state) return;

    // Liberar cada fila del tablero
    if (state->board) {
        for ( int i = 0; i < state->num_rows; i++) {
            free(state->board[i]);
        }
        free(state->board);
    }

    // Liberar el arreglo de serpientes
    if (state->snakes) {
        free(state->snakes);
    }

    // Liberar la estructura del estado
    free(state);
}


/* Tarea 3 */
void print_board(game_state_t* state, FILE* fp) {
  for (int i = 0; i< state->num_rows; i++){
    fprintf(fp,"%s\n", state->board[i]);
  }
  return;
}


/**
 * Guarda el estado actual a un archivo. No modifica el objeto/struct state.
 * (ya implementada para que la utilicen)
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Tarea 4.1 */


/**
 * Funcion de ayuda que obtiene un caracter del tablero dado una fila y columna
 * (ya implementado para ustedes).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}


/**
 * Funcion de ayuda que actualiza un caracter del tablero dado una fila, columna y
 * un caracter.
 * (ya implementado para ustedes).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}


/**
 * Retorna true si la variable c es parte de la cola de una snake.
 * La cola de una snake consiste de los caracteres: "wasd"
 * Retorna false de lo contrario.
*/
static bool is_tail(char c) {
  // TODO: Implementar esta funcion.
  return c == 'w' || c == 'a' || c == 's' || c == 'd';
}


/**
 * Retorna true si la variable c es parte de la cabeza de una snake.
 * La cabeza de una snake consiste de los caracteres: "WASDx"
 * Retorna false de lo contrario.
*/
static bool is_head(char c) {
  // TODO: Implementar esta funcion.
  return c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x';
}


/**
 * Retorna true si la variable c es parte de una snake.
 * Una snake consiste de los siguientes caracteres: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implementar esta funcion.
  return is_head(c) || is_tail(c) || c == '^' || c == 'v' || c == '<' || c == '>';
}


/**
 * Convierte un caracter del cuerpo de una snake ("^<v>")
 * al caracter que correspondiente de la cola de una
 * snake ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implementar esta funcion.
  switch (c) {
    case '^': return 'w';
    case '<': return 'a';
    case 'v': return 's';
    case '>': return 'd';
    default:  return '?'; 
  }
}


/**
 * Convierte un caracter de la cabeza de una snake ("WASD")
 * al caracter correspondiente del cuerpo de una snake
 * ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implementar esta funcion.
  switch (c) {
    case 'W': return '^';
    case 'A': return '<';
    case 'S': return 'v';
    case 'D': return '>';
    default:  return '?';
  }
}


/**
 * Retorna cur_row + 1 si la variable c es 'v', 's' o 'S'.
 * Retorna cur_row - 1 si la variable c es '^', 'w' o 'W'.
 * Retorna cur_row de lo contrario
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implementar esta funcion.
  if (c == 'v' || c == 's' || c == 'S') return cur_row + 1;
  if (c == '^' || c == 'w' || c == 'W') return cur_row - 1;
  return cur_row;
}


/**
 * Retorna cur_col + 1 si la variable c es '>' or 'd' or 'D'.
 * Retorna cur_col - 1 si la variable c es '<' or 'a' or 'A'.
 * Retorna cur_col de lo contrario
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implementar esta funcion.
  if (c == '>' || c == 'd' || c == 'D') return cur_col + 1;
  if (c == '<' || c == 'a' || c == 'A') return cur_col - 1;
  return cur_col;
}


/**
 * Tarea 4.2
 *
 * Funcion de ayuda para update_state. Retorna el caracter de la celda
 * en donde la snake se va a mover (en el siguiente paso).
 *
 * Esta funcion no deberia modificar nada de state.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  snake_t snake = state->snakes[snum];

    unsigned int head_row = snake.head_row;
    unsigned int head_col = snake.head_col;

    char head_char = get_board_at(state, head_row, head_col);

    unsigned int next_row = get_next_row(head_row, head_char);
    unsigned int next_col = get_next_col(head_col, head_char);

    return get_board_at(state, next_row, next_col);
}


/**
 * Tarea 4.3
 *
 * Funcion de ayuda para update_state. Actualiza la cabeza de la snake...
 *
 * ... en el tablero: agregar un caracter donde la snake se va a mover (¿que caracter?)
 *
 * ... en la estructura del snake: actualizar el row y col de la cabeza
 *
 * Nota: esta funcion ignora la comida, paredes, y cuerpos de otras snakes
 * cuando se mueve la cabeza.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  snake_t* snake = &state->snakes[snum];

  unsigned int head_row = snake->head_row;
  unsigned int head_col = snake->head_col;
  char head_char = get_board_at(state, head_row, head_col);

  unsigned int new_row = get_next_row(head_row, head_char);
  unsigned int new_col = get_next_col(head_col, head_char);

  set_board_at(state, head_row, head_col, head_to_body(head_char));

  set_board_at(state, new_row, new_col, head_char);

  snake->head_row = new_row;
  snake->head_col = new_col;
}


/**
 * Tarea 4.4
 *
 * Funcion de ayuda para update_state. Actualiza la cola de la snake...
 *
 * ... en el tablero: colocar un caracter blanco (spacio) donde se encuentra
 * la cola actualmente, y cambiar la nueva cola de un caracter de cuerpo (^<v>)
 * a un caracter de cola (wasd)
 *
 * ...en la estructura snake: actualizar el row y col de la cola
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  snake_t* snake = &state->snakes[snum];

  unsigned int tail_row = snake->tail_row;
  unsigned int tail_col = snake->tail_col;
  char tail_char = get_board_at(state, tail_row, tail_col);

  set_board_at(state, tail_row, tail_col, ' ');

  unsigned int new_row = get_next_row(tail_row, tail_char);
  unsigned int new_col = get_next_col(tail_col, tail_char);

  char new_tail_body = get_board_at(state, new_row, new_col);

  set_board_at(state, new_row, new_col, body_to_tail(new_tail_body));

  snake->tail_row = new_row;
  snake->tail_col = new_col;
}

/* Tarea 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implementar esta funcion.
  for (unsigned int i = 0; i < state->num_snakes; i++) {
    snake_t* snake = &state->snakes[i];

    if (get_board_at(state, snake->head_row, snake->head_col) == 'x') {
      continue;
    }

    char next = next_square(state, i);

    if (next == '#' || is_snake(next)) {
      set_board_at(state, snake->head_row, snake->head_col, 'x'); // marcar como muerta en el tablero
      snake->live = false; // ¡también márcala como muerta en la estructura!
      continue;
    }

    if (next == '*') {
      update_head(state, i);
      add_food(state);       
      continue;
    }

    if (next == ' ') {
      update_head(state, i);
      update_tail(state, i);
    }
  }
}

/* Tarea 5 */
game_state_t* load_board(char* filename) {
  // TODO: Implementar esta funcion.
  FILE* fp = fopen(filename, "r");
  if (fp == NULL) return NULL;

  game_state_t* state = malloc(sizeof(game_state_t));
  if (!state) {
    fclose(fp);
    return NULL;
  }

  state->num_rows = 0;
  state->board = NULL;
  state->num_snakes = 0;
  state->snakes = NULL;

  int c;
  char* row = NULL;
  size_t row_capacity = 0;
  size_t row_length = 0;

  while ((c = fgetc(fp)) != EOF) {
    if (c == '\n') {
      // Fin de una línea: terminamos la cadena con '\0'
      char* finished_row = malloc(row_length + 1);
      if (!finished_row) {
        free(row);
        fclose(fp);
        return NULL;
      }
      memcpy(finished_row, row, row_length);
      finished_row[row_length] = '\0';

      // Agregamos la fila al tablero
      char** new_board = realloc(state->board, sizeof(char*) * (state->num_rows + 1));
      if (!new_board) {
        free(row);
        free(finished_row);
        fclose(fp);
        return NULL;
      }
      state->board = new_board;
      state->board[state->num_rows++] = finished_row;

      // Reiniciamos para la siguiente fila
      row_length = 0;
    } else {
      // Asegurarnos de tener suficiente espacio
      if (row_length + 1 > row_capacity) {
        size_t new_capacity = row_capacity == 0 ? 8 : row_capacity * 2;
        char* new_row = realloc(row, new_capacity);
        if (!new_row) {
          free(row);
          fclose(fp);
          return NULL;
        }
        row = new_row;
        row_capacity = new_capacity;
      }
      row[row_length++] = (char)c;
    }
  }

  if (row_length > 0) {
    char* finished_row = malloc(row_length + 1);
    if (!finished_row) {
      free(row);
      fclose(fp);
      return NULL;
    }
    memcpy(finished_row, row, row_length);
    finished_row[row_length] = '\0';

    char** new_board = realloc(state->board, sizeof(char*) * (state->num_rows + 1));
    if (!new_board) {
      free(row);
      free(finished_row);
      fclose(fp);
      return NULL;
    }
    state->board = new_board;
    state->board[state->num_rows++] = finished_row;
  }

  free(row);
  fclose(fp);
  return state;
}


/**
 * Tarea 6.1
 *
 * Funcion de ayuda para initialize_snakes.
 * Dada una structura de snake con los datos de cola row y col ya colocados,
 * atravezar el tablero para encontrar el row y col de la cabeza de la snake,
 * y colocar esta informacion en la estructura de la snake correspondiente
 * dada por la variable (snum)
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implementar esta funcion.
  unsigned int row = state->snakes[snum].tail_row;
  unsigned int col = state->snakes[snum].tail_col;

  while (true) {
    char current = get_board_at(state, row, col);
    if (current == 'W' || current == 'A' || current == 'S' || current == 'D') {
      break;
    }

    if (current == '>' || current == 'd') {
      col++;
    } else if (current == '<' || current == 'a') {
      col--;
    } else if (current == '^' || current == 'w') {
      row--;
    } else if (current == 'v' || current == 's') {
      row++;
    } else {
      break;
    }
  }

  state->snakes[snum].head_row = row;
  state->snakes[snum].head_col = col;
}

/* Tarea 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implementar esta funcion.
  unsigned int count = 0;

  for (unsigned int row = 0; row < state->num_rows; row++) {
    for (unsigned int col = 0; state->board[row][col] != '\0'; col++) {
      char tail = state->board[row][col];
      if (tail == 'w' || tail == 'a' || tail == 's' || tail == 'd') {
        count++;
      }
    }
  }

  state->num_snakes = count;
  state->snakes = malloc(sizeof(snake_t) * count);


  unsigned int index = 0;
  for (unsigned int row = 0; row < state->num_rows; row++) {
    for (unsigned int col = 0; state->board[row][col] != '\0'; col++) {
      char tail = state->board[row][col];
      if (tail == 'w' || tail == 'a' || tail == 's' || tail == 'd') {
        snake_t* snake = &state->snakes[index];

        snake->tail_row = row;
        snake->tail_col = col;
        snake->live = true;

        find_head(state, index);

        index++;
      }
    }
  }

  return state;
}
