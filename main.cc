#include <iostream>
#include <string>
#include <tuple>
#include <boost/filesystem/operations.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/search.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/range/combine.hpp>
#include <cc/Slice.h>
#include <boost/range/algorithm/copy.hpp>
#include <cc/File.hpp>
#include <cc/thread/Thread.hpp>
#include <cc/range/collect.hpp>
#include <boost/range/numeric.hpp>
#include <cc/range/fold.hpp>
#include <boost/range/algorithm/for_each.hpp>


using std::string_view;
using std::string;
using std::begin;
using std::end;
using std::distance;
using boost::for_each;
using namespace std::string_view_literals;
using cc::thread::Thread;
using cc::range::fold;


using boost::adaptors::filtered;
using boost::adaptors::indexed;
using boost::adaptors::transformed;
using boost::search;
using boost::filesystem::path;
using boost::filesystem::current_path;
using cc::range::collect;

using cc::File;
using cc::Slice;

constexpr auto pattern = "qt_prfxpath="sv;
constexpr auto longest_path = 256;

std::string to_forward_slash (const path & p)
{
    auto qt_path_str = p.string ();
    for_each (qt_path_str | filtered ([] (auto c) { return c == '\\'; }),
              [] (auto & c) { c = '/'; });

    return qt_path_str;
}

void in_memory_set_value (Slice<char> buf, string_view bin_str)
{
    const auto pos = search (buf, pattern);
    if (pos == end (buf))
    {
        return;
    }

    const auto offset = distance (begin (buf), pos);
    const auto start_point = offset + static_cast<ssize_t> (pattern.size());


    auto zeroing_buf = buf.subspan (start_point, longest_path);

    for_each (zeroing_buf, [] (auto & c) { c = 0; });

    auto name_buf = zeroing_buf.subspan (0, static_cast<ssize_t> (bin_str.size()));

    for (auto it : bin_str | indexed ())
    {
        name_buf.at(it.index()) = it.value();
    }
}



bool modify_file (const path& p, string_view prefix)
{
    auto file_content = File::read_all(p.string().data());
    if (not file_content)
    {
        return false;
    }
    in_memory_set_value(file_content.value(), prefix);

    const auto res = File::override(p.string().data(), file_content.value());

    return res;
}

constexpr auto file_list =
{
    "Qt5Core.dll",
    "Qt5Cored.dll"
};

int main()
{
    const auto qt_prefix = current_path().append("qt");
    const auto qt_prefix_str = to_forward_slash(qt_prefix);
    const auto qt_bin = path (qt_prefix).append("bin");

    for (auto && it : file_list | transformed ([&] (auto it) { return path (qt_bin).append(it); }))
    {
        const auto res = modify_file(it, qt_prefix_str);
        if (res)
        {
            std::cout << "fixed file:" << it << '\n';
        }
        else
        {
            std::cout << "failed to fix file:" << it << '\n';
        }
    }

    getchar();
}

