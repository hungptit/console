#include <algorithm>
#include <ncurses.h>
#include <string>
#include <vector>

#include "console.hpp"
#include "fmt/format.h"
#include "ioutils/ioutils.hpp"

#include "cereal/archives/json.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/vector.hpp"

#include "clara.hpp"

#include "utils/matchers.hpp"
#include "utils/regex_matchers.hpp"

namespace {
    struct SearchParams {
        bool invert_match = false; // Select non-matching lines
        bool verbose = false;      // Display verbose information.
        int regex_mode = (HS_FLAG_DOTALL | HS_FLAG_SINGLEMATCH);
        template <typename Archive> void serialize(Archive &ar) {
            ar(CEREAL_NVP(invert_match), CEREAL_NVP(verbose), CEREAL_NVP(regex_mode));
        }
    };

    struct InputParams {
        std::string pattern;
        std::string inputfile;
        SearchParams params;

        template <typename Archive> void serialize(Archive &ar) {
            ar(CEREAL_NVP(inputfile), CEREAL_NVP(params));
        }
    };

    InputParams parse_input_arguments(int argc, char *argv[]) {
        InputParams params;
        bool help = false;
        bool ignore_case = false;
        auto cli =
            clara::Help(help) |
            clara::Opt(params.params.verbose)["-v"]["--verbose"](
                "Display verbose information") |
            clara::Opt(ignore_case)["-i"]["--ignore-case"]("Ignore case") |
            clara::Opt(params.params.invert_match)["-u"]["--invert-match"](
                "Select lines that do not match specified pattern.") |
            clara::Opt(params.pattern, "pattern")["-e"]["--pattern"]("A prefilter pattern.") |
            clara::Arg(params.inputfile, "")("An input file");

        auto result = cli.parse(clara::Args(argc, argv));
        if (!result) {
            fmt::print(stderr, "Invalid option: {}\n", result.errorMessage());
            exit(1);
        }

        // Print out the help document.
        if (help) {
            std::ostringstream oss;
            oss << cli;
            fmt::print("{}", oss.str());
            exit(EXIT_SUCCESS);
        }

        if (params.inputfile.empty()) {
            fmt::print("An input file is required.");
            exit(EXIT_FAILURE);
        }

        // Init the mode for our regular expression engine.
        if (ignore_case) {
            params.params.regex_mode |= HS_FLAG_CASELESS;
        }

        // Display input arguments in JSON format if verbose flag is on
        if (params.params.verbose) {
            std::stringstream ss;
            {
                cereal::JSONOutputArchive ar(ss);
                ar(cereal::make_nvp("Input arguments", params));
            }
            fmt::print("{}\n", ss.str());
        }

        return params;
    }

    template <typename Params> struct DummyPolicy {
        DummyPolicy(Params &&params) : parameters(std::forward<Params>(params)) {}
        
        void init_data() {
            original_data.push_back("Hello");
            original_data.push_back("How");
            original_data.push_back("Can");
            original_data.push_back("we");
            original_data.push_back("do");
            original_data.push_back("this");
            original_data.push_back("Hello");
            original_data.push_back("How");
            original_data.push_back("Can");
            original_data.push_back("we");
            original_data.push_back("do");
            original_data.push_back("this");
            original_data.push_back("Hello");
            original_data.push_back("How");
            original_data.push_back("Can");
            original_data.push_back("we");
            original_data.push_back("do");
            original_data.push_back("this");
            original_data.push_back("Hello");
            original_data.push_back("How");
            original_data.push_back("Can");
            original_data.push_back("we");
            original_data.push_back("do");
            original_data.push_back("this");
            original_data.push_back("Hello");
            original_data.push_back("How");
            original_data.push_back("Can");
            original_data.push_back("we");
            original_data.push_back("do");
            original_data.push_back("this");
            original_data.push_back("Hello");
            original_data.push_back("How");
            original_data.push_back("Can");
            original_data.push_back("we");
            original_data.push_back("do");
            original_data.push_back("this");
            original_data.push_back("Hello");
            original_data.push_back("How");
            original_data.push_back("Can");
            original_data.push_back("we");
            original_data.push_back("do");
            original_data.push_back("this");
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

        std::string pattern;
        std::vector<std::string> original_data;
        std::vector<std::string> data;
        Params parameters;
    };

} // namespace

int main(const int argc, char *argv[]) {
    auto params = parse_input_arguments(argc, argv);
    using Policy = DummyPolicy<decltype(params)>;
    using Screen = console::Screen<Policy>;
    Screen scr(std::move(params));
    scr.run();
    return 0;
}
