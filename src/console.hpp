#include <algorithm>
#include <ncurses.h>
#include <string>
#include <vector>

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

        ~Screen() { endwin(); }

        void run() {
            bool isok = true;
            const int nrows = LINES - 3;
            while (isok) {
                int key = getch();

                if (std::isalpha(key) || std::isdigit(key) || isvalid(key)) {
                    Policy::pattern.push_back(key);
                    update();
                    continue;
                }

                mvprintw(0, 0, "key: %d", key);

                if (key == Key::UP) {
                    if (current_position >  0) {
                        if ((nrows - current_position + 1) < Policy::data.size()) {
                            --current_position;
                        }
                    } else {
                        // Update view.
                    }
                    print_output();
                    continue;
                }

                if (key == Key::DOWN) {
                    if (current_position < nrows) {
                        ++current_position;
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
                    mvprintw(1, 0, "current_position: %d", current_position);
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
            const int nrows = LINES - 3;
            const int ncols = COLS;
            Policy::update_data();
            int idx = nrows;

            // Print out content.
            mvprintw(0, 0, "-> current_position: %d", current_position);
            for (auto const &aline : Policy::data) {
                if (idx == current_position)
                    mvprintw(idx, 0, "> %s", create_line(aline, ncols - 10).data());
                else
                    mvprintw(idx, 0, "  %s", create_line(aline, ncols - 10).data());

                if (--idx < 0) break;
            }

            std::string empty_line(ncols, ' ');
            while (idx > 3) {
                mvprintw(idx, 0, empty_line.data());
                --idx;
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
            // Init screen
            initscr();
            refresh();
            noecho();
            keypad(stdscr, TRUE);
            raw();

            // Init view
            Policy::init_data();
            Policy::update_data();
            print_input();
            print_output();

            // Update line cursor
            current_position = LINES - 3;
        }

        // The current position of a line cursor.
        int current_position;
        int begin_view;
        int end_view;
    };

} // namespace console
