#include "dirscan.h"

int dirscan_info::dirscan(const std::string& dirname)
{  
  std::uintmax_t n_dir{0}, n_file{0}, sum_size{0};
  
  try {    
    fs::recursive_directory_iterator i{dirname,fs::directory_options::skip_permission_denied}, end{};
    while (i != end) 
      {
        bool has_except; // use 'has_except' to see if we had an exception

	std::uintmax_t size = 0;
	std::string name;
	fs::path p;

	// TRY: p = i->path();
	has_except = false;
	try { p = i->path(); }
	catch (fs::filesystem_error ex) {
	  has_except = true;
	}
	if (has_except == false) {

	  // TRY: name = p.u8string();
	  has_except = false;
	  try { name = p.u8string(); }
	  catch (std::system_error ex) {
	    // this happens in visual studio in the recycle bin.
	    has_except = true;
	  }
	  if (has_except == false) {
	    // It can happen that asking meta information of the found directory entry
	    // throws, at least under windows. Let's discard such entries.
	    bool is_dir;

	    // TRY: is_dir = i->is_directory();
	    has_except = false;
	    try { is_dir = i->is_directory(); }
	    catch (fs::filesystem_error ex) {
	      has_except = true;
	    }
	    if (has_except == false) {
	      if (is_dir) {

		// TRY: auto tmp = directory_iterator(name);
		has_except = false;
		try { auto tmp = fs::directory_iterator(name); }
		catch (fs::filesystem_error ex) {
		  has_except = true;
		  i.disable_recursion_pending();
		}
		if (has_except == false) {
		  dirs.push_back({ name });
		  ++n_dir;
		}
	      }
	      else if (i->is_regular_file()) {
		fs::file_time_type timestamp;
		
		// TRY: size = i->file_size();
		has_except = false;
		try {
		  size = i->file_size();
		  timestamp = fs::last_write_time(p);
		}
		catch (fs::filesystem_error ex) {
		  has_except = true;
		}
		if (has_except == false) {
		  sum_size += size;
		  this->files.push_back({ name,size, timestamp });
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

	// increment to next entry.
	try { ++i; }
	catch (fs::filesystem_error ex) {
	  std::cout
	    << "\n\niterator_increment: " << ex.what() << std::endl;
	  return 1;
	}
      }

    this->n_dir = n_dir;
    this->n_file = n_file;
    this->sum_size = sum_size;
  }
  catch (fs::filesystem_error ex) {
    std::cout << "\n\ndirscan() generic filesystem exception: " << ex.what() << std::endl;
    return 1;
  }

  return 0;
}
