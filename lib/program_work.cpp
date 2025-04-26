#include "program_work.h"
#include "input_parse.h"

int64_t OffsetLogTimeZone(char* ptr) {
	int64_t delta = 0;
	for (int i = Consts::uts_offset_delta; i >= 1; --i) {
		delta += (*(ptr-i) - '0') * Consts::offsets_seconds[i - 1];
	}

	return (*(ptr - Consts::sign_offset) == '-' ? 1 : -1) * delta;
}

int32_t FindLastQuote(char* ptr) {
	int32_t last_ind = 0;

	for (int32_t i = 0; *(ptr+i) != '\0' && *(ptr+i); ++i) {
		if (*(ptr+i) == '"') {
			last_ind = i;
		}
	}

	return last_ind;
}

void ExtractUnixtime(RequestInfo& req, char*& ptr) {
	while (*ptr != '[') {
		++ptr;
	}
	++ptr;

	uint64_t date_len = 0;
	while (*ptr != ']') {
		++ptr;
		++date_len;
	}


	char calendare_date[date_len+1];
	for (int i = 0; i < date_len; ++i) {
		calendare_date[i] = *(ptr-date_len+i);
	}
	calendare_date[date_len] = '\0';

	std::tm date;
	std::istringstream date_stream(&calendare_date[0]);
	date_stream >> std::get_time(&date, "%d/%b/%Y:%H:%M:%S ");

	date.tm_isdst = 0; 
	date.tm_gmtoff = 0;
	req.request_time = std::mktime(&date) + OffsetLogTimeZone(ptr);
}

void ExtractRequest(RequestInfo& req, char*& ptr) {
	while(*ptr != '"') {
		++ptr;
	}
	++ptr;
	req.request = ptr;

	int32_t offset = FindLastQuote(ptr);
	for (int i = 0; i < offset; ++i) {
		req.request_method += *ptr;
		ptr++;
	}
}

void CollectLogValues(RequestInfo& req, char* ptr) {
	ExtractUnixtime(req, ptr);
	ExtractRequest(req, ptr);

	while (*ptr != ' '){
		ptr++;
	}
	ptr++;
	req.is_server_err = *ptr == Consts::serever_err_status_group;
}

void PrintErrorsOut(char* request, std::ofstream& out, bool to_stdout) {
	while(*request != '"') {
		if (to_stdout) {
			std::cout << *request;
		}
		out << *request;
		++request;
	}
	out << '\n';
	if (to_stdout) {
		std::cout << std::endl;
	}
}

void AddRequestToTimeWindow(TimeWindowControlVars& window_vars, std::time_t timestamp) {
  	window_vars.now_requests_at_window += 1;
	if (window_vars.window.empty()) {
		window_vars.window.push({timestamp, 1});

	} else if (window_vars.window.back().first == timestamp) {
		++window_vars.window.back().second;
	} else {
	window_vars.window.push({timestamp, 1});

	while (window_vars.window_len - 1 < timestamp - window_vars.window.front().first) {
		window_vars.now_requests_at_window -= window_vars.window.front().second;
		window_vars.window.pop();
	}
	}

	if (window_vars.max_requests_at_window < window_vars.now_requests_at_window) {
		window_vars.max_requests_at_window = window_vars.now_requests_at_window;
		window_vars.last_maximum = timestamp;
	}
}

void PrintTopReqEndedInErr(std::unordered_map<std::string, uint32_t>& request_err, int64_t top_req_ended_error) {
	if (request_err.size() == 0) {
		std::cout << "There is no requests ended in error with current parameters" << std::endl;
	} else {
	std::cout << std::endl;
	int64_t size = std::min(top_req_ended_error, static_cast<int64_t>(request_err.size()));
	std::cout << "Top " << size << " (of " << top_req_ended_error << ") requests that ended with an error:" << std::endl;

	int count = 0;
	std::pair<uint32_t, std::string> result[size];
	for (std::unordered_map<std::string, uint32_t>::iterator it = request_err.begin(); count < top_req_ended_error && it != request_err.end(); ++it, ++count) { 
		result[count] = {it->second, it->first};
	}

	std::sort(result, result+size, std::greater<>());
	for (int i = 0; i < size; ++i) {
		std::cout << i + 1 << ". \"" << result[i].second << " \" " << result[i].first << " times" << std::endl;
		}
	}
}

void PrintFreqWindow(TimeWindowControlVars& window_vars, int64_t freq_window_len) {
	if (window_vars.last_maximum == -1) {
		std::cout << "There is no window of length " << freq_window_len << " with current parameters" << std::endl;
		return;
	} 
	std::time_t start_stamp = window_vars.last_maximum - freq_window_len+1;

	std::cout << std::endl;
	std::cout << "Window with the most amount of requests: " << std::put_time(std::gmtime(&start_stamp), "%d/%b/%Y:%H:%M:%S")
	<< " - - - " << std::put_time(std::gmtime(&window_vars.last_maximum), "%d/%b/%Y:%H:%M:%S") << " (UTC)" << std::endl;
}

char* const WorkWithLogFile(Args& args) {
	std::ifstream log_file(args.input_file_path, std::ios::in);  // for reading
	std::ofstream out_file(args.output_file_path, std::ios::app);// append
	if (!log_file.is_open()) {
		return WorkProgrammErrs::input_file_err;
	}
	if (!out_file.is_open() && args.output_file_path != nullptr) {
		return WorkProgrammErrs::output_file_err;
	}
	if (args.output_file_path == nullptr && args.freq_window_len == 0) { //no work to do
		return nullptr;
	}

	TimeWindowControlVars window_vars;
	window_vars.window_len = args.freq_window_len;

	std::unordered_map<std::string, uint32_t> request_err;

	char* stream_buffer = new char[Consts::kBufferSize_stream];
	log_file.rdbuf()->pubsetbuf(stream_buffer, sizeof(stream_buffer));

	char* buffer = new char[Consts::kBufferSize];
	while (!log_file.eof()) {
		log_file.getline(buffer, Consts::kBufferSize, '\n');
		
		if (buffer[Consts::kBufferSize-2] != '\n' && buffer[Consts::kBufferSize-2] != '\0') {
			delete[] stream_buffer;
			delete[] buffer;
			return WorkProgrammErrs::buffer_err;
		}

		RequestInfo req;
		CollectLogValues(req, buffer);

		if(req.request_time < args.start_time) {
			continue;
		} else if (req.request_time > args.end_time) {
			break;
		}

		if (req.is_server_err) {
			PrintErrorsOut(req.request, out_file, args.is_print);

			request_err[req.request_method] += 1;
		}

		if (args.freq_window_len > 0) {
			AddRequestToTimeWindow(window_vars, req.request_time);
		}
	}

	if (args.top_req_ended_error > 0 && args.output_file_path != nullptr) {
		PrintTopReqEndedInErr(request_err, args.top_req_ended_error);
	}

	if (args.freq_window_len > 0) {
		PrintFreqWindow(window_vars, args.freq_window_len);
	}

	delete[] stream_buffer;
	delete[] buffer;
	return nullptr;
}
