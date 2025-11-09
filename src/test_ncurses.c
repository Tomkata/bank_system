#include <ncurses.h>

int main() {
    // Инициализация
    initscr();              // Стартира ncurses режим
    cbreak();               // Изключва line buffering
    noecho();               // Не показва натиснатите клавиши
    keypad(stdscr, TRUE);   // Включва function keys (стрелки и т.н.)
    curs_set(0);            // Скрива курсора
    
    // Проверка за цветове
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_CYAN, COLOR_BLACK);
    }
    
    // Печат на съобщение
    attron(COLOR_PAIR(1));
    mvprintw(5, 10, "✓ ncurses WORKS!");
    attroff(COLOR_PAIR(1));
    
    attron(COLOR_PAIR(3));
    mvprintw(7, 10, "Press any key to exit...");
    attroff(COLOR_PAIR(3));
    
    refresh();
    getch();
    
    // Cleanup
    endwin();
    return 0;
}