#include "input_parse.h"
#include "program_work.h"

int main(int argc, char* argv[]) {
    Args args;
    char* error_message = ParseInputValues(args, argc, argv);
    if (error_message != nullptr) {
      std::cerr << error_message << std::endl;
      return EXIT_FAILURE;
    }

    error_message = WorkWithLogFile(args);
    if (error_message != nullptr) {
        std::cerr << error_message << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;  
}
