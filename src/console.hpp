#include <algorithm>
#include <ncurses.h>
#include <string>
#include <vector>

namespace console {
    constexpr int BACKSPACE = 127;
    constexpr int CTRL_D = 4;
    constexpr int ENTER = 10;
    constexpr int ESC = 27;
    constexpr int UP = 661;
    constexpr int DOWN = 651;

    template <typename Policy> class Screen : public Policy {
      public:
        template<typename Params>
        Screen(Params && params) : Policy(std::forward<Params>(params)) {
            init();
        }

        ~Screen() { endwin(); }

        void run() {
            bool isok = true;
            while (isok) {
                int key = getch();
                switch (key) {
                case console::BACKSPACE:
                    if (!Policy::pattern.empty()) {
                        Policy::pattern.pop_back();
                    }
                    break;
                case ENTER:
                    break;
                case ESC:
                    isok = false;
                    break;
                case UP:
                    break;
                case DOWN:
                    break;
                case CTRL_D:
                    isok = false;
                    break;
                default:
                    Policy::pattern.push_back(key);
                }

                // Update data
                Policy::update_data();

                // Update view
                print_input();
                print_output();
            }
        }

      private:
        void print_output() {
            const int nrows = LINES - 3;
            const int ncols = COLS;
            Policy::update_data();
            int idx = LINES - 3;

            // Print out content.
            for (auto const &aline : Policy::data) {
                mvprintw(idx, 0, "%s", create_line(aline, ncols).data());
                --idx;
            }

            std::string empty_line(ncols, ' ');
            while (idx > -1) {
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

            // Init view
            Policy::init_data();
            Policy::update_data();
            print_input();
            print_output();
        }

        int current_position = 0;
    };

} // namespace console
