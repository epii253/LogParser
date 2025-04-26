#pragma once

#include <cstdint>
#include <cstring>
#include <ctime>
#include <limits.h>
#include <iostream>
#include <iomanip>

namespace ParseConsts {
const int double_dash_len = 2;
const int outfile_key_long_len = 8;
const int tostdout_flag_key_long_len = 7;
const int top_error_requests_key_long_len = 7;
const int window_len_key_long_len = 8;
const int start_time_key_long_len = 6;
const int end_time_key_long_len = 4;

const int short_keys_len = 2;

const int num_base = 10;
} 

namespace ParseInputErrs {
char* const invalid_arg = const_cast<char*>("Invalid command's argument");
}


struct Args {
  	const char* output_file_path = nullptr;
	const char* input_file_path = nullptr;
	bool is_print = false;
	int64_t top_req_ended_error = 10;
	int64_t freq_window_len = 0;
	std::time_t start_time = 0ll;
	std::time_t end_time = LLONG_MAX;
};

char* const ParseInputValues(Args& results, int argc, char** argv);
