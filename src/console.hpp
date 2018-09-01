#include <algorithm>
#include <ncurses.h>
#include <string>
#include <vector>

#include "fmt/format.h"

// TODO: Update view logic.

namespace console {
    enum Key : int {
        BACKSPACE = 127,
        DOWN = 258,
        UP = 259,
        LEFT = 269,
        RIGHT = 261,
        ESC = 27,
        ENTER = 10, // Mark the current item.
        CTRL_D = 4,
        CTRL_C = 3,
        CTRL_N = 14,
        CTRL_O = 15,
        CTRL_P = 16,
        CTRL_S = 19,
    };

    template <typename Policy> class Screen : public Policy {
      public:
        template <typename Params>
        Screen(Params &&params) : Policy(std::forward<Params>(params)) {
            init();
        }

        ~Screen() {
            endwin();
            if (current_position < Policy::data.size()) {
                fmt::print("{}\n", Policy::data[current_position]);
            }
        }

        void run() {
            bool isok = true;
            while (isok) {
                int key = getch();

                if (std::isalpha(key) || std::isdigit(key) || isvalid(key)) {
                    Policy::pattern.push_back(key);
                    update();
                    continue;
                }

                // mvprintw(0, 40, "key: %d", key);

                if (key == Key::UP) {
                    const size_t nrows = LINES - 3;
                    const size_t end = std::min(nrows, Policy::data.size());
                    if ((current_position + 1) < end) {
                        ++current_position;
                    } else {
                        // Update view.
                    }
                    print_output();
                    continue;
                }

                if (key == Key::DOWN) {
                    if (current_position > 0) {
                        --current_position;
                    } else {
                        // Update view
                    }
                    print_output();
                    continue;
                }

                if ((key == Key::ESC) || (key == Key::CTRL_D) || (key == Key::CTRL_C)) {
                    isok = false;
                    continue;
                }

                if (key == Key::BACKSPACE) {
                    if (Policy::pattern.empty()) continue;
                    Policy::pattern.pop_back();
                    update();
                }

                if (key == Key::ENTER) {
                    mvprintw(0, 60, "%s", Policy::data[current_position].data());
                }
            }
        }

      private:
        bool isvalid(int ch) {
            return (ch == ':') || (ch == '(') || (ch == ')') || (ch == '.') || (ch == '?') ||
                   (ch == ']') || (ch == '[') || (ch == '_') || (ch == '-') || (ch == '*');
        }

        void update() {
            Policy::update_data();
            print_input();
            print_output();
            refresh();
        }

        void print_output() {
            const size_t nrows = LINES - 3;
            const size_t ncols = COLS;
            Policy::update_data();

            // Print out lines.
            auto end = std::min(nrows, Policy::data.size());
            size_t idx = 0;
            for (; idx != end; ++idx) {
                auto const &aline = Policy::data[idx];
                if (idx != current_position) {
                    mvprintw(nrows - idx, 0, "  %s", create_line(aline, ncols - 10).data());
                } else {
                    attron(A_BOLD);
                    mvprintw(nrows - idx, 0, "> %s", create_line(aline, ncols - 10).data());
                    attroff(A_BOLD);
                }
            }

            // Erase the empty lines
            std::string empty_line(ncols, ' ');
            while (idx < nrows) {
                mvprintw(nrows - idx, 0, empty_line.data());
                ++idx;
            }

            // Make sure that the cursor is at the input.
            move(nrows + 2, Policy::pattern.size() + 2);
        }

        void print_input() {
            const int ncols = COLS;
            const int nrows = LINES;
            std::string row(ncols, ' ');
            std::string sep_line(ncols, '-');
            mvprintw(nrows - 2, 0, "%s", sep_line.data());
            mvprintw(nrows - 1, 0, "> %s", create_line(Policy::pattern, ncols).data());
        }

      private:
        std::string create_line(const std::string &aline, const int ncols) {
            std::string results(aline);
            if (results.size() < ncols) {
                results.append(ncols - results.size(), ' ');
            }
            return results;
        }

        void init() {
            current_position = 0;
            
            // Init screen
            initscr();
            refresh();
            noecho();
            keypad(stdscr, TRUE);
            raw();

            // Init data
            Policy::init_data();
            Policy::update_data();

            // Init view
            const size_t nrows = LINES - 3;
            begin_view = 0;
            end_view = std::min(nrows, Policy::data.size());
            
            print_input();
            print_output();
        }

        // The current position of a line cursor.
        int current_position; // Current position of the sceen cursor
        int begin_view = 0;       // Begin view of the data
        int end_view;
    }; // End view of the data

} // namespace console
