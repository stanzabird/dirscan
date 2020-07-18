#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <cstring>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <getopt.h>

namespace fs = std::filesystem;

// available commandline options
extern bool use_debug;
extern bool use_stdout;
extern bool use_st;
extern bool use_list;
extern bool use_list_details;

struct dirscan_info {
  using filesize_t = std::uintmax_t;
  struct dir { std::string name; };
  struct file { std::string name; filesize_t size; fs::file_time_type last_write_time; };

  std::vector<dir> dirs;
  std::vector<file> files;

  // number of dirs, files and sum of filesizes
  filesize_t n_dir{ 0 }, n_file{ 0 }, sum_size{ 0 };

  // provide some sort of result to the user...
  void report_status(const std::string& dirname, bool use_multiple) {
    if (use_multiple)
      std::cout
	<< dirname << ": ";
    
    std::cout
      << n_dir << " dirs, "
      << n_file << " files, "
      << sum_size << " bytes. " << std::endl;
  }
  void list_data() {
    for (auto i : files) {
      if (use_list_details) {
	std::cout << " " << i.size << " " << i.name << "\n";
      }
      else
	std::cout << i.name << "\n";
    }
  }

  // the actual work...
  int dirscan(const std::string& dirname);
  int dirscan_mt(const std::string& dirname);
};
