#include <menu.h>
#include <curses.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define MAX_WORD_SIZE 500

void exit_editor(int);
static void auto_complete();
static void scroll_menu(MENU*, int);

int main(int argc, const char* argv[]) {
	signal(SIGINT, exit_editor);
	(void) initscr(); // Initalizes variables needed
	(void) noecho(); // Disables automatic echoing
	(void) cbreak(); // Allowws single character buffering
	refresh(); // Clears screen entirely
	keypad(stdscr, TRUE); // Captures special keystrokes

	char *word = calloc(MAX_WORD_SIZE, 1);
	int i = 0;
	char c;
	for(;;) {
		// Capturing user input and displaying it
		c = getch();
		switch(c) {
		// If user input a period then we look to autocomplete
		case '.':
			addch(c);
			auto_complete();
			continue; // No need to go through rest of loop break;
		default:
			addch(c);
			refresh();
		}
		if (sizeof(word) == MAX_WORD_SIZE) {
			word = calloc(MAX_WORD_SIZE, 1);
		}
		word[i] = c;
		i++;
	}
}

void exit_editor(int signo) {
	if (signo == SIGINT) {
		endwin();
		exit(0);
	}
}

static void scroll_menu(MENU *menu, int length) {
	int count = 0;
	int choice = '\t';
	do {
		switch (choice) {
			case '\t':
				if (count < length-1) {
					menu_driver(menu, REQ_DOWN_ITEM);
					count++;
				}
				else {
					menu_driver(menu, REQ_UP_ITEM);
					count = 0;
				}
				break;
			case '\n':
			case KEY_ENTER:
				menu_driver(menu, REQ_TOGGLE_ITEM);
				addch(choice);
				refresh();
				return;
		}
		choice = getch();
	} while(true);
}

static void auto_complete() {
	int y, x;
	getyx(stdscr, y, x);
	ITEM **items = (ITEM **)calloc(2, sizeof(ITEM*));
	items[0] = new_item("first item", "Desc");
	items[1] = new_item("second item", "Desc");
	MENU *auto_menu = new_menu(items);
	menu_opts_on(auto_menu, O_ONEVALUE);
	WINDOW *menu_window = derwin(stdscr, 0, 0, y, x);
	set_menu_sub(auto_menu, menu_window);
	post_menu(auto_menu);
	refresh();
	if (getch()=='\t') {
			scroll_menu(auto_menu, 2);
	}
	for (int j = 0; j < sizeof(items)/sizeof(items[0]); j++) {
		free_item(items[j]);
	}
	free_menu(auto_menu);
}
