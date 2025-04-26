#pragma once

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <queue>
#include <string>
#include <stdlib.h>
#include <unordered_map>

namespace Consts {
static const uint32_t kBufferSize = std::pow(2, 10);
static const uint32_t kBufferSize_stream = std::pow(2, 10)*10;

const int offsets_seconds[4] = {60, 600, 3600, 36000};
const int uts_offset_delta = 4;
const int sign_offset = 5;

const int num_base = 10;

const char serever_err_status_group = '5';
};

namespace WorkProgrammErrs {
char* const input_file_err = const_cast<char*>("No such log file in the directoy or unsuccesfully open.");
char* const output_file_err = const_cast<char*>("No such output file in the directoy or unsuccesfully open.");
char* const buffer_err = const_cast<char*>("Buffer overflow.");
}

struct Args;


struct RequestInfo {
	std::time_t request_time = std::time(nullptr);
	std::string request_method;
	bool is_server_err = false;
	char* request = nullptr;
};


struct TimeWindowControlVars {
	int64_t window_len = 0;
	std::queue<std::pair<std::time_t, int32_t>> window = {};

	int64_t now_requests_at_window = 0;
	int64_t max_requests_at_window = 0;

	std::time_t last_maximum = -1;
};

char* const WorkWithLogFile(Args& arg);
void CollectLogValues(RequestInfo* req, char* ptr);

void ExtractUnixtime(RequestInfo& req, char*& ptr);
void ExtractRequest(RequestInfo& req, char*& ptr);

void PrintErrorsOut(char* ptr, std::ofstream& file, bool to_stdout);
void AddRequestToTimeWindow(TimeWindowControlVars& window_vars);
int64_t OffsetLogTimeZone(char* ptr);
int32_t FindLastQuote(char* ptr);

void PrintTopReqEndedInErr(std::unordered_map<std::string, uint32_t>& request_err, int64_t top_req_ended_error);
void PrintFreqWindow(TimeWindowControlVars& window_vars, int64_t freq_window_len);
