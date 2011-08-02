
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <X11/Xlib.h>
#include <iostream>
#include <fstream>
#include <tuple>
#include "utility.hpp"


static char const* net_rx_f = "/sys/class/net/wlan0/statistics/rx_bytes";
static char const* net_tx_f = "/sys/class/net/wlan0/statistics/tx_bytes";

static char const* date_str = "%a %d %b %H:%M:%S";
static char const* frmt_str = "[%f %f %f] [%dMB (%d%%)] [d:%u u:%u KB/s] %s";


typedef std::tuple<double, double, double> load_pack_t;
typedef std::tuple<unsigned long, unsigned long> net_pack_t;


void
get_time(std::string& buf)
{
    static char rb[25];
    static time_t rawtime;

    ::time(&rawtime);
    tm* timeinfo = ::localtime(&rawtime);

    size_t s = ::strftime(rb, sizeof(rb), date_str, timeinfo);
    buf.append(rb, s);
}


load_pack_t
get_loadavg()
{
    static double a[3];

    ::getloadavg(a, 3); // check
    return load_pack_t{a[0], a[1], a[2]};
}


net_pack_t
get_net_traffic(int const delta_sec)
{
    using std::ifstream;

    static char rb[16];
    static unsigned long rxb_prev = 0;
    static unsigned long txb_prev = 0;

    unsigned long rxb = 0;
    unsigned long txb = 0;

    ifstream rif(net_rx_f, ifstream::in|ifstream::binary);
    if (rif.is_open()) {
        rif.getline(rb, sizeof(rb));
        rxb = ::strtoul(rb, 0, 10);
    }

    ifstream tif(net_tx_f, ifstream::in|ifstream::binary);
    if (tif.is_open()) {
        tif.getline(rb, sizeof(rb));
        txb = ::strtoul(rb, 0, 10);
    }

    unsigned long rxs = (rxb - rxb_prev) / 1024 / delta_sec;
    unsigned long txs = (txb - txb_prev) / 1024 / delta_sec;

    rxb_prev = rxb;
    txb_prev = txb;

    return net_pack_t{ rxs, txs };
}


int
main(void)
{
    Display* xdisp;

    if (!(xdisp = ::XOpenDisplay(0))) {
        std::cerr << "dwmstatus: cannot open display.\n";
        return 1;
    }

    long mem_pages = ::sysconf(_SC_PHYS_PAGES);
    long mem_page_size = ::sysconf(_SC_PAGE_SIZE);
    long mem_total = mem_pages * mem_page_size / 1024/1024;

    std::string status_buf;
    std::string time_buf;
    time_buf.reserve(30);

    unsigned int sleep_val = 1;

    for (;; ::sleep(sleep_val))
    {
        status_buf.clear();
        time_buf.clear();

        get_time(time_buf);
        load_pack_t const& load = get_loadavg();
        net_pack_t const& net = get_net_traffic(sleep_val);

        long mem_avail = ::sysconf(_SC_AVPHYS_PAGES) * mem_page_size / 1024/1024;
        long mem_used = mem_total - mem_avail;
        unsigned int mem_used_pc = 100 * mem_used / mem_total;

        utility::format(status_buf, frmt_str
                       ,std::get<0>(load), std::get<1>(load), std::get<2>(load)
                       ,mem_used, mem_used_pc
                       ,std::get<0>(net), std::get<1>(net)
                       ,time_buf);

        ::XStoreName(xdisp, DefaultRootWindow(xdisp), status_buf.c_str());
        ::XSync(xdisp, 0);
    }

    XCloseDisplay(xdisp);

    return 0;
}
