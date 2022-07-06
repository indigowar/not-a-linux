#include "screen.h"

#include "../kernel/utils.h"
#include "ports.h"

// Private functions

int get_cursor_offset();
void set_cursor_offset(int offset);
int print_char(char c, int col, int row, char attr);
int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);

// ===================================================
//              Public functions
// ===================================================

void screen_clear() {
  int screen_size = MAX_COLS * MAX_ROWS;
  char *screen = (char *)VIDEO_MEMORY;
  for (int i = 0; i < screen_size; i++) {
    screen[i * 2] = ' ';
    screen[i * 2 + 1] = WHITE_ON_BLACK;
  }
  set_cursor_offset(get_offset(0, 0));
}

// print message(0) in columnt(1) and row(2)
void screen_print_at(char *msg, int col, int row) {
  int offset;
  if (col >= 0 && row >= 0) {
    offset = get_offset(col, row);
  } else {
    offset = get_cursor_offset();
    row = get_offset_row(offset);
    col = get_offset_col(offset);
  }

  for (int i = 0; msg[i] != 0; i++) {
    offset = print_char(msg[i], col, row, WHITE_ON_BLACK);
    row = get_offset_row(offset);
    col = get_offset_col(offset);
  }
}

void screen_print(char *msg) { screen_print_at(msg, -1, -1); }

// ===================================================
//              Private functions
// ===================================================

// print symbol(0) at column(1) and row(2) with attribute(3)
int print_char(char symbol, int col, int row, char attr) {
  unsigned char *vmem = (unsigned char *)VIDEO_MEMORY;

  if (!attr) {
    attr = WHITE_ON_BLACK;
  }

  if (col >= MAX_COLS || row >= MAX_ROWS) {
    int addr = 2 * MAX_COLS * MAX_COLS - 2;
    vmem[addr] = 'E';
    vmem[addr + 1] = RED_ON_WHITE;
  }

  int offset;
  if (col >= 0 && row >= 0) {
    offset = get_offset(col, row);
  } else {
    offset = get_cursor_offset();
  }

  if (symbol == '\n') {
    row = get_offset_row(offset);
    offset = get_offset(0, row + 1);
  } else {
    vmem[offset] = symbol;
    vmem[offset + 1] = attr;
    offset += 2;
  }

  // handle scrolling
  if (offset >= MAX_ROWS * MAX_COLS * 2) {
    for (int i = 1; i < MAX_ROWS; i++) {
      memncpy(
          /* source */ (char *)((void *)VIDEO_MEMORY + get_offset(0, i)),
          /* dest   */ (char *)((void *)VIDEO_MEMORY + get_offset(0, i - 1)),
          /* count  */ MAX_COLS);
    }

    char *last_line = get_offset(0, MAX_ROWS - 1) + (char *)VIDEO_MEMORY;
    for (int i = 0; i < MAX_COLS * 2; i++) last_line[i] = 0;

    offset -= 2 * MAX_COLS;
  }

  set_cursor_offset(offset);
  return offset;
}

int get_cursor_offset() {
  port_byte_out(REG_SCREEN_CTRL, 14);
  int offset = port_byte_in(REG_SCREEN_DATA) << 8; /* High byte: << 8 */
  port_byte_out(REG_SCREEN_CTRL, 15);
  offset += port_byte_in(REG_SCREEN_DATA);
  return offset * 2; /* Position * size of character cell */
}

void set_cursor_offset(int offset) {
  offset /= 2;
  port_byte_out(REG_SCREEN_CTRL, 14);
  port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
  port_byte_out(REG_SCREEN_CTRL, 15);
  port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));
}

int get_offset(int col, int row) { return 2 * (row * MAX_COLS + col); }

int get_offset_row(int offset) { return offset / (2 * MAX_COLS); }

int get_offset_col(int offset) {
  return (offset - (get_offset_row(offset) * 2 * MAX_COLS)) / 2;
}
