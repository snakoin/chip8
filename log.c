#include "log.h"

#include <stdio.h>
#include <SDL.h>
#include "chip8.h"
#include "assembly.h"

#ifdef NCURSES_LOGGING
    #include <ncurses.h>
    WINDOW* w_instructions;
    WINDOW* w_registers;
    WINDOW* w_stack;
    void draw_box(WINDOW* w_window, char* title) {
        wattr_on(w_window, A_REVERSE, NULL);
        box(w_window, 0, 0);
        mvwprintw(w_window, 0, 1, "%s", title);
        wattr_off(w_window, A_REVERSE, NULL);
}
#endif

bool logging = false;
Chip8 old_state;

void chip8_logging(Chip8 *chip8) {
#ifndef NCURSES_LOGGING
    if (logging && (chip8->pc != old_state.pc)) {
        memcpy(&old_state, chip8, sizeof(Chip8));
        char *instruction;
        instruction = get_asm_code(chip8->opcode);
        printf("0x%04x: %s0x%04x %s%s %sPC:0x%02X I:0x%02X V(", chip8->pc, GREY, chip8->opcode, YELLOW, instruction, GREY, chip8->pc, chip8->index);
        for (int v = 0; v < 16; v++) {
            printf("%X:%X", v, chip8->V[v]);
            if (v < 15) printf(", ");
        }
        printf(") SP:0x%02X ", chip8->sp);
        for (int i = 0; i < chip8->sp; i++) {
            printf("stack[0x%X]:0x%02X ", i, chip8->stack[i]);
        }
        printf("\n%s", RESET);
    }
#endif
#ifdef NCURSES_LOGGING
    if (logging && (chip8->pc != old_state.pc)) {
        clear();
        box(w_instructions, 0, 0);
        draw_box(w_registers, "Registers");
        for (int i = 0; i < 16; i++) {
            if (i < (LINES - 2)) mvwprintw(w_registers, i + 1, 1, "V%X: 0x%02X", i, chip8->V[i]);
        }
        mvwprintw(w_registers, 1, (int)(w_registers->_maxx - 10), "PC: 0x%02X", chip8->pc);
        mvwprintw(w_registers, 2, (int)(w_registers->_maxx - 10), "I: 0x%02X", chip8->index);
        mvwprintw(w_registers, 3, (int)(w_registers->_maxx - 10), "DT: 0x%X", chip8->delay_timer);

        draw_box(w_stack, "Stack");
        mvwprintw(w_stack, 1, 1, "SP: 0x%X", chip8->sp);
        for (int i = 0; i < chip8->sp; i++) {
            mvwprintw(w_stack, i + 1, 10, "stack[0x%X]:0x%02X", i, chip8->stack[i]);
        }

        char *instruction;
        mvwprintw(w_instructions, 0, 3, "addr");
        mvwprintw(w_instructions, 0, 8, "opcode");
        mvwprintw(w_instructions, 0, 15, "instruction");
        for (int i_pc = -2; chip8->pc + i_pc < 4096; i_pc += 2) {
            uint16_t opcode = (chip8->memory[chip8->pc + i_pc] << 8) | chip8->memory[chip8->pc + i_pc + 1];
            if (i_pc == 0) {
                wattr_on(w_instructions, A_REVERSE, NULL);
            }
            if ((i_pc) < 4096 && (i_pc + 2) / 2 < (LINES - 2)) {
                mvwprintw(w_instructions, (i_pc + 2) / 2 + 1, 1, "0x%04X 0x%04X", chip8->pc + i_pc, opcode);
                instruction = get_asm_code(opcode);
                wattr_on(w_instructions, COLOR_PAIR(1), NULL);
                mvwprintw(w_instructions, (i_pc + 2) / 2 + 1, 15, "%s", instruction);
            }
            if (i_pc == 0) {
                wattr_off(w_instructions, A_REVERSE, NULL);
            }
            wattr_off(w_instructions, COLOR_PAIR(1), NULL);
        }
        refresh();
    }
    memcpy(&old_state, chip8, sizeof(Chip8));
#endif
}

void chip8_logging_init() {
#ifdef NCURSES_LOGGING
    initscr();
    w_instructions = subwin(stdscr, LINES, 40, 0, 0);
    w_registers = subwin(stdscr, LINES, 25, 0, 40);
    w_stack = subwin(stdscr, LINES, COLS - 65, 0, 65);
    start_color();
    init_color(COLOR_YELLOW, 1000, 1000, 0);
    init_color(COLOR_BLACK, 0, 0, 0);
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    curs_set(0);
#endif
}

void chip8_logging_end() {
#ifdef NCURSES_LOGGING
    endwin();
#endif
}
