#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

struct dirscan_info {
  using filesize_t = std::uintmax_t;
  struct dir { std::string name; };
  struct file { std::string name; filesize_t size; };

  std::vector<dir> dirs;
  std::vector<file> files;

  filesize_t n_dir, n_file, sum_size;
};

// -q (quiet mode) sets this to 'false'
bool use_stdout = true;

int
dirscan(const char* arg)
{  
  dirscan_info dsi;
  
  using namespace std::filesystem;
  std::uintmax_t n_dir{0}, n_file{0}, sum_size{0};
  
  try {    
    for (recursive_directory_iterator i
	{
	 arg,
	 directory_options::skip_permission_denied
	}, end{}; i != end;)
      {
	std::uintmax_t size = 0;
	std::string name = i->path().u8string();

	if (i->is_directory()) {
	  dsi.dirs.push_back({name});
	  ++n_dir;
	}
	else if (i->is_regular_file()) {
	  size = i->file_size();
	  sum_size += size;
	  dsi.files.push_back({name,size});
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
	
	// can't do this outside the for loop..
	if (i == end && use_stdout)
	  std::cout << std::endl;
      }

    dsi.n_dir = n_dir;
    dsi.n_file = n_file;
    dsi.sum_size = sum_size;

    // provide some sort of result to the user...
    if (use_stdout == false)
      std::cout
	<< dsi.n_dir << " dirs, "
	<< dsi.n_file << " files, "
	<< dsi.sum_size << " bytes." << std::endl;
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
  if (argc == 2) {
    if (strcmp(argv[1],"-q") == 0) {
      use_stdout = false;
      return dirscan(".");
    }
    else 
      return dirscan(argv[1]);
  }
  else if (argc == 3) {
    if (strcmp(argv[1],"-q") == 0) use_stdout = false;
    return dirscan(argv[2]);
  }
  else {
    return dirscan(".");
  }  
}


