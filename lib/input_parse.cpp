#include "input_parse.h"

char* const ParseInputValues(Args& args, int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {  
        if (std::strncmp(argv[i], "-o", ParseConsts::short_keys_len) == 0 || std::strncmp(argv[i], "--output", ParseConsts::outfile_key_long_len) == 0) {
            if(std::strncmp(argv[i], "--", ParseConsts::double_dash_len) == 0 && argv[i][ParseConsts::outfile_key_long_len] == '=') {
                args.output_file_path = argv[i] + ParseConsts::outfile_key_long_len + 1;
                } else {
                    if(i + 1 < argc) {
                        args.output_file_path = argv[i+1];
                        ++i;
                    } else {
                        return ParseInputErrs::invalid_arg;
                    }
            }
        } else if (std::strncmp(argv[i], "-p", ParseConsts::tostdout_flag_key_long_len) == 0 || std::strncmp(argv[i], "--print", ParseConsts::tostdout_flag_key_long_len) == 0) {
            args.is_print = true;

        } else if (std::strncmp(argv[i], "-s", ParseConsts::short_keys_len) == 0 || std::strncmp(argv[i], "--stats", ParseConsts::top_error_requests_key_long_len) == 0) {
            if(std::strncmp(argv[i], "--", ParseConsts::double_dash_len) == 0 && argv[i][ParseConsts::top_error_requests_key_long_len] == '=') {
                args.top_req_ended_error = std::strtoll(argv[i] + ParseConsts::top_error_requests_key_long_len + 1, nullptr, ParseConsts::num_base);
                } else {
                    if(i + 1 < argc) {
                        args.top_req_ended_error = std::strtoll(argv[i+1], nullptr,  ParseConsts::num_base);
                        ++i;
                    } else {
                        return ParseInputErrs::invalid_arg;
                    }
            }

        } else if (std::strncmp(argv[i], "-w", ParseConsts::short_keys_len) == 0 || std::strncmp(argv[i], "--window", ParseConsts::window_len_key_long_len) == 0) {
            if(std::strncmp(argv[i], "--", ParseConsts::double_dash_len) == 0 && argv[i][ParseConsts::window_len_key_long_len] == '=') {
                args.freq_window_len = std::strtoll(argv[i] + ParseConsts::window_len_key_long_len + 1, nullptr,  ParseConsts::num_base);
                } else {
                    if(i + 1 < argc) {
                        args.freq_window_len = std::strtoll(argv[i+1], nullptr,  ParseConsts::num_base);
                        ++i;
                    } else {
                        return ParseInputErrs::invalid_arg;
                    }
            }
        } else if (std::strncmp(argv[i], "-f", ParseConsts::short_keys_len) == 0 || std::strncmp(argv[i], "--from", ParseConsts::start_time_key_long_len) == 0) {
            if(std::strncmp(argv[i], "--", ParseConsts::double_dash_len) == 0 && argv[i][ParseConsts::start_time_key_long_len] == '=') {
                args.start_time = std::strtoll(argv[i] + ParseConsts::start_time_key_long_len + 1, nullptr,  ParseConsts::num_base);
                } else {
                    if(i + 1 < argc) {
                        args.start_time = std::strtoll(argv[i+1], nullptr,  ParseConsts::num_base);
                        ++i;
                    } else {
                        return ParseInputErrs::invalid_arg;
                    }
            }

        } else if (std::strncmp(argv[i], "-e", ParseConsts::short_keys_len) == 0 || std::strncmp(argv[i], "--to", ParseConsts::end_time_key_long_len) == 0) {
            if(std::strncmp(argv[i], "--", ParseConsts::double_dash_len) == 0 && argv[i][ParseConsts::end_time_key_long_len] == '=') {
                args.end_time = std::strtoll(argv[i] + ParseConsts::end_time_key_long_len + 1, nullptr,  ParseConsts::num_base);
                } else {
                    if(i + 1 < argc) {
                        args.end_time = std::strtoll(argv[i+1], nullptr,  ParseConsts::num_base);
                        ++i;
                    } else {
                        return ParseInputErrs::invalid_arg;
                    }
            }
        } else { //garbage or input log file
            args.input_file_path = argv[i];
        }
    }

    if ( args.top_req_ended_error < 0 ||  args.top_req_ended_error == LLONG_MAX
        ||  args.freq_window_len < 0 ||  args.freq_window_len == LLONG_MAX
        ||  args.start_time >  args.end_time) {
        return ParseInputErrs::invalid_arg;
    }

    return nullptr;
}
