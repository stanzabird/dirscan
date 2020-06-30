#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

using namespace std::filesystem;

// -q (quiet mode) sets this to 'false'
bool use_stdout = true;

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

int
dirscan_info::dirscan(const std::string& dirname)
{  
  std::uintmax_t n_dir{0}, n_file{0}, sum_size{0};
  
  try {    
    recursive_directory_iterator i{dirname,directory_options::skip_permission_denied}, end{};
    while (i != end) 
      {
        bool has_except; // use 'ex_caught' to see if we had an exception

	std::uintmax_t size = 0;
	std::string name;
	path p;

	// try: p = i->path();
	has_except = false;
	try { p = i->path(); }
	catch (filesystem_error ex) {
	  has_except = true;
	}
	if (has_except == false) {
	  // try: name = p.u8string();
	  has_except = false;
	  try { name = p.u8string(); }
	  catch (filesystem_error ex) {
	    has_except = true;
	  }
	  catch (std::exception ex) {
	    has_except = true;
	  }
	  if (has_except == false) {
	    // It can happen that asking meta information of the found directory entry
	    // throws, at least under windows. Let's discard such entries.
	    bool is_dir;

	    // try: is_dir = i->is_directory();
	    has_except = false;
	    try { is_dir = i->is_directory(); }
	    catch (filesystem_error ex) {
	      has_except = true;
	    }
	    if (has_except == false) {
	      if (is_dir) {
		this->dirs.push_back({ name });
		++n_dir;
	      }
	      else if (i->is_regular_file()) {
		// try: size = i->file_size();
		has_except = false;
		try { size = i->file_size(); }
		catch (filesystem_error ex) {
		  has_except = true;
		}
		if (has_except == false) {
		  sum_size += size;
		  this->files.push_back({ name,size });
		  ++n_file;
		}
	      }

	      if (size > 0 && use_stdout)
		std::cout
		<< " "
		<< n_dir << " dirs, "
		<< n_file << " files, "
		<< sum_size << " bytes."
		<< "\r" << std::flush;
	    }
	  }
	}

	
	// increment to next entry
	try { ++i; }
	catch (filesystem_error ex) {
	  std::cout
	    << "\n\niterator_increment: " << ex.what() << std::endl;
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
    std::cout << "\n\n dirscan() generic: " << ex.what() << std::endl;
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


