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

    template <typename Policy>
    class Screen : public Policy {
      public:
        Screen() {
            initscr();
            refresh();
            noecho();
            nrows = LINES;
            ncols = COLS;
            number_of_output_rows = nrows - 3;
            sep_line = std::string(ncols, '-');
            empty_line = std::string(ncols, ' ');

            // Init data.
            Policy::init_data();

            // Update data
            Policy::update_data();

            // Update view
            print_input();
            print_output();
        }

        ~Screen() { endwin(); }

        void run() {
            bool isok = true;
            while (isok) {
                int key = getch();
                switch (key) {
                case console::BACKSPACE:
                    if (!pattern.empty()) {
                        pattern.pop_back();
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
                    pattern.push_back(key);
                }

                // Update data
                update_data();

                // Update view
                print_input();
                print_output();
            }
        }

      private:
        void init_data() {
            original_data.push_back("Hello");
            original_data.push_back("How");
            original_data.push_back("Can");
            original_data.push_back("we");
            original_data.push_back("do");
            original_data.push_back("this");
        }

        void update_data() {
            data.clear();
            if (pattern.empty()) {
                for (auto aline : original_data) {
                    data.push_back(aline);
                }
            } else {
                for (auto const &line : original_data) {
                    if (line.find(pattern) != std::string::npos) {
                        data.push_back(line);
                    }
                }
            }
        }

        void print_output() {
            update_data();
            int idx = number_of_output_rows;

            // Print out content.
            for (auto const &aline : data) {
                mvprintw(idx, 0, "%s", create_line(aline).data());
                --idx;
            }

            while (idx > 1) {
                mvprintw(idx, 0, empty_line.data());
                --idx;
            }

            move(nrows - 1, pattern.size() + 2);
        }

        void print_input() {
            std::string row(ncols, ' ');
            mvprintw(nrows - 2, 0, "%s", sep_line.data());
            mvprintw(nrows - 1, 0, "> %s", create_line(pattern).data());
        }

      private:
        int nrows;
        int ncols;
        int number_of_output_rows;
        std::string pattern;
        std::vector<std::string> original_data;
        std::vector<std::string> data;
        std::string sep_line;
        std::string empty_line;

        std::string create_line(const std::string &aline) {
            std::string results(aline);
            if (results.size() < ncols) {
                results.append(ncols - results.size(), ' ');
            }
            return results;
        }
    };

} // namespace console
