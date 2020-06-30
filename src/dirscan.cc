#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

using namespace std::filesystem;

struct dirscan_info {
  using filesize_t = std::uintmax_t;
  struct dir { std::string name; };
  struct file { std::string name; filesize_t size; };

  std::vector<dir> dirs;
  std::vector<file> files;

  // number of dirs, files and sum of filesizes
  filesize_t n_dir, n_file, sum_size;

  // the actual work...
  int dirscan(const std::string& dirname);
};

// -q (quiet mode) sets this to 'false'
bool use_stdout = true;

int
dirscan_info::dirscan(const std::string& dirname)
{  
  std::uintmax_t n_dir{0}, n_file{0}, sum_size{0};
  
  try {    
    for (recursive_directory_iterator i
	{
	 dirname,
	 directory_options::skip_permission_denied
	}, end{}; i != end;)
      {
	std::uintmax_t size = 0;
	std::string name = i->path().u8string();

	if (i->is_directory()) {
	  this->dirs.push_back({name});
	  ++n_dir;
	}
	else if (i->is_regular_file()) {
	  size = i->file_size();
	  sum_size += size;
	  this->files.push_back({name,size});
	  ++n_file;
	}
      
	if (size > 0 && use_stdout)
	  std::cout
	    << " "
	    << n_dir << " dirs, "
	    << n_file << " files, "
	    << sum_size << " bytes."
	    << "\r" << std::flush;
	
	// increment to next entry
	try { ++i; }
	catch (filesystem_error ex) {
	  std::cout
	    << "\n\n" << ex.what() << std::endl;
	  return 1;
	}
      }

    this->n_dir = n_dir;
    this->n_file = n_file;
    this->sum_size = sum_size;

    // provide some sort of result to the user...
    std::cout
      << this->n_dir << " dirs, "
      << this->n_file << " files, "
      << this->sum_size << " bytes. " << std::endl;
  }
  catch (filesystem_error ex) {
    std::cout << ex.what() << std::endl;
    return 1;
  }

  return 0;
}

int
main(int argc, char* argv[])
{
  dirscan_info dsi;

  if (argc == 2) {
    if (strcmp(argv[1],"-q") == 0) {
      use_stdout = false;
      return dsi.dirscan(".");
    }
    else 
      return dsi.dirscan(argv[1]);
  }
  else if (argc == 3) {
    if (strcmp(argv[1],"-q") == 0) use_stdout = false;
    return dsi.dirscan(argv[2]);
  }
  else {
    return dsi.dirscan(".");
  }  
}


