#include "dirscan.h"

// available commandline options
bool use_debug = false;
bool use_stdout = true;
bool use_mt = false;
bool use_list = false;
bool use_list_details = false;

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
       {"list",no_argument,0,'l'},
       {"list-details",no_argument,0,'L'},
       {0,0,0,0}
      };

    int option_index = 0;
    int c = getopt_long(argc, argv, "hdqmlL", long_options, &option_index);

    if (c == -1) break; // Detect the end of the options.

    switch (c) 
      {
      case 'h':
	std::cout
	  << "Use: dirscan [options] [path...]\n"
	  << "  -h, --help          This help\n"
	  << "  -d, --debug         Print debugging info\n"
	  << "  -q, --quiet         Don't print progress (faster)\n"
	  << "  -m, --mt            Use multithreading (faster)\n"
	  << "  -l, --list          List all found files to stdout.\n"
	  << "  -L, --list-details  List found files with size and last write time\n"
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
      case 'l':
	use_list = true;
	break;
      case 'L':
	use_list = true;
	use_list_details = true;
	break;

      case '?':
	// getopt_long() already printed an error message;
	return 1;
      default:
	abort();
      }
  }

  // we don't want progress just listing them files
  if (use_list) use_stdout = false;
  
  dirscan_info dsi;
  
  if (optind < argc) {
    while (optind < argc) {
      auto retval = use_mt ? dsi.dirscan_mt(argv[optind]) : dsi.dirscan(argv[optind]);
      if (retval != 0) return retval;
      if (use_list)
	dsi.list_data();
      else
	dsi.report_status(argv[optind],true);
      optind++;
    }
  }
  else {
    auto retval = use_mt ? dsi.dirscan_mt(".") : dsi.dirscan(".");
    if (retval != 0) return retval;
    if (use_list)
      dsi.list_data();
    else
      dsi.report_status(".",false);
  }
  return 0;
}
