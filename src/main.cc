#include "dirscan.h"

// available commandline options
bool use_debug = false;
bool use_stdout = true;
bool use_mt = false;



int main(int argc, char* argv[]) {

  // We're using getopt_long(), a usage example is here:
  // https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html

  for (;;) {
    static struct option long_options[] =
      {
       {"help",no_argument,0,'h'},
       {"debug",no_argument,0,'d'},
       {"quiet",no_argument,0,'q'},
       {"mt",no_argument,0,'m'},
       {0,0,0,0}
      };

    int option_index = 0;
    int c = getopt_long(argc, argv, "hdqm", long_options, &option_index);

    if (c == -1) break; // Detect the end of the options.

    switch (c) 
      {
      case 'h':
	std::cout
	  << "Use: dirscan [options] [path...]\n"
	  << "  -h, --help   This help\n"
	  << "  -d, --debug  Print debugging info\n"
	  << "  -q, --quiet  Don't print progress (faster)\n"
	  << "  -m, --mt     Use multithreading (faster)\n"
	  ;
	return 0;
      case 'd':
	use_debug = true;
	break;
      case 'q':
	use_stdout = false;
	break;
      case 'm':
	use_mt = true;
	break;

      case '?':
	// getopt_long() already printed an error message;
	return 1;
      default:
	abort();
      }
  }

  dirscan_info dsi;
  
  if (optind < argc) {
    while (optind < argc) {
      auto retval = use_mt ? dsi.dirscan_mt(argv[optind],true) : dsi.dirscan(argv[optind], true);
      if (retval != 0) return retval;
      optind++;
    }
  }
  else
    return use_mt ? dsi.dirscan_mt(".",false) : dsi.dirscan(".", false);

  return 0;
}
