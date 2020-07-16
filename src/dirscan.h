#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <getopt.h>

namespace fs = std::filesystem;

// available commandline options
extern bool use_debug;
extern bool use_stdout;
extern bool use_mt;

struct dirscan_info {
  using filesize_t = std::uintmax_t;
  struct dir { std::string name; };
  struct file { std::string name; filesize_t size; fs::file_time_type last_write_time; };

  std::vector<dir> dirs;
  std::vector<file> files;

  // number of dirs, files and sum of filesizes
  filesize_t n_dir{ 0 }, n_file{ 0 }, sum_size{ 0 };

  // the actual work...
  int dirscan(const std::string& dirname, bool use_multiple);
  int dirscan_mt(const std::string& dirname, bool use_multiple);
};
