
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "io_device.hpp"
#include "mmfile.hpp"
#include "os_tools.hpp"

namespace json_benchmarks {

/*************************************************************************************************/

struct input_string_buffer_io::impl {
    impl(const std::string &input_fname)
        :ifname{input_fname}
    {
        std::ifstream is{ifname};
        assert(is);

        auto fsize = file_size(input_fname.c_str());
        istream.resize(fsize);

        is.read(istream.data(), istream.size());
    }

    std::size_t size() { return istream.size(); }
    void reserve(std::size_t size) { istream.reserve(size); }
    void resize(std::size_t size) { istream.resize(size); }

    std::string ifname;
    std::string istream;
};

input_string_buffer_io::input_string_buffer_io(const std::string &input_fname)
    :pimpl{new impl{input_fname}}
{}

io_type input_string_buffer_io::type() const { return io_type::string_buffer; }
io_direction input_string_buffer_io::direction() const { return io_direction::input; }
void input_string_buffer_io::reset() { std::string e; pimpl->istream.swap(e); }
const std::string& input_string_buffer_io::name() const { return pimpl->ifname; }
std::size_t input_string_buffer_io::size() const { return pimpl->size(); }
void input_string_buffer_io::reserve(std::size_t size) { return pimpl->reserve(size); }
void input_string_buffer_io::resize(std::size_t size) { return pimpl->resize(size); }

std::string& input_string_buffer_io::stream()
{ return pimpl->istream; }

/*************************************************************************************************/

struct output_string_buffer_io::impl {
    impl(const std::string &output_fname)
        :ofname{output_fname}
    {}

    std::string& stream() { return ostream; }
    std::size_t size() { return ostream.size(); }
    void reserve(std::size_t size) { ostream.reserve(size); }
    void resize(std::size_t size) { ostream.resize(size); }

    std::string ofname;
    std::string ostream;
};

output_string_buffer_io::output_string_buffer_io(const std::string &output_fname)
    :pimpl{new impl{output_fname}}
{}

io_type output_string_buffer_io::type() const { return io_type::string_buffer; }
io_direction output_string_buffer_io::direction() const { return io_direction::output; }
void output_string_buffer_io::reset() { std::string e; pimpl->ostream.swap(e); }
const std::string& output_string_buffer_io::name() const { return pimpl->ofname; }
std::size_t output_string_buffer_io::size() const { return pimpl->size(); }
void output_string_buffer_io::reserve(std::size_t size) { return pimpl->reserve(size); }
void output_string_buffer_io::resize(std::size_t size) { return pimpl->resize(size); }

std::string& output_string_buffer_io::stream()
{ return pimpl->stream(); }

/*************************************************************************************************/
#if 0
struct input_std_fstream_io::impl {
    impl(const std::string &input_fname)
        :ifname{input_fname}
        ,istream{ifname, std::ios::in|std::ios::binary}
    {
        assert(istream.is_open());
    }

    std::istream& stream() {
        istream.seekg(0);

        return istream;
    }
    std::size_t size() { return file_size(ifname.c_str()); }
    void reserve(std::size_t /*size*/) {}

    std::string ifname;
    std::ifstream istream;
};

input_std_fstream_io::input_std_fstream_io(const std::string &input_fname)
    :pimpl{new impl{input_fname}}
{}

io_type input_std_fstream_io::type() const { return io_type::std_fstreams; }
io_direction input_std_fstream_io::direction() const { return io_direction::input; }
void input_std_fstream_io::reset() { pimpl->istream.seekg(0); }
const std::string& input_std_fstream_io::name() const { return pimpl->ifname; }
std::size_t input_std_fstream_io::size() const { return pimpl->size(); }
void input_std_fstream_io::reserve(std::size_t size) { return pimpl->reserve(size); }

std::istream& input_std_fstream_io::stream()
{ return pimpl->stream(); }

/*************************************************************************************************/

struct output_std_fstream_io::impl {
    impl(const std::string &output_fname)
        :ofname{output_fname}
        ,ostream{}
    {
        ostream.open(ofname, mode|std::ios::trunc);
        assert(ostream.is_open());
    }

    std::ostream& stream() { return ostream; }
    std::size_t size() { return file_size(ofname.c_str()); }
    void reserve(std::size_t size) {
        ostream.close();

        assert(::truncate(ofname.c_str(), size) == 0);

        ostream.open(ofname, mode);
        assert(ostream.is_open());
        assert(file_size(ofname.c_str()) == size);
    }

    std::string ofname;
    static constexpr std::fstream::openmode mode = std::ios::in|std::ios::out|std::ios::binary;
    std::fstream ostream;
};

output_std_fstream_io::output_std_fstream_io(const std::string &output_fname)
    :pimpl{new impl{output_fname}}
{}

io_type output_std_fstream_io::type() const { return io_type::std_fstreams; }
io_direction output_std_fstream_io::direction() const { return io_direction::output; }
void output_std_fstream_io::reset() { pimpl->ostream.seekp(0); }
const std::string& output_std_fstream_io::name() const { return pimpl->ofname; }
std::size_t output_std_fstream_io::size() const { return pimpl->size(); }
void output_std_fstream_io::reserve(std::size_t size) { return pimpl->reserve(size); }

std::ostream& output_std_fstream_io::stream()
{ return pimpl->stream(); }
#endif
/*************************************************************************************************/

struct input_std_strstream_io::impl {
    impl(const std::string &input_fname)
        :ifname{input_fname}
        ,ibuffer{}
        ,istream{}
    {
        std::ifstream is{ifname};
        assert(is);

        std::size_t fsize = file_size(ifname.c_str());
        ibuffer.resize(fsize);
        is.read(ibuffer.data(), ibuffer.size());

        istream.rdbuf()->pubsetbuf(ibuffer.data(), ibuffer.size());
    }

    std::istream& stream() {
        istream.seekg(0);

        return istream;
    }

    std::size_t size() { return ibuffer.size(); }
    void reserve(std::size_t size) { assert(size || "UNIMPLEMENTED!"); }
    void resize(std::size_t size) { assert(size || "UNIMPLEMENTED!"); }

    std::string ifname;
    std::string ibuffer;
    std::istringstream istream;
};

input_std_strstream_io::input_std_strstream_io(const std::string &input_fname)
    :pimpl{new impl{input_fname}}
{}

io_type input_std_strstream_io::type() const { return io_type::std_strstreams; }
io_direction input_std_strstream_io::direction() const { return io_direction::input; }
void input_std_strstream_io::reset() { pimpl->istream.seekg(0); }
const std::string& input_std_strstream_io::name() const { return pimpl->ifname; }
std::size_t input_std_strstream_io::size() const { return pimpl->size(); }
void input_std_strstream_io::reserve(std::size_t size) { return pimpl->reserve(size); }
void input_std_strstream_io::resize(std::size_t size) { return pimpl->resize(size); }

std::istream& input_std_strstream_io::stream()
{ return pimpl->stream(); }

/*************************************************************************************************/

struct output_std_strstream_io::impl {
    impl(const std::string &output_fname)
        :ofname{output_fname}
    {}

    std::size_t size() { return ostream.tellp(); }
    void reserve(std::size_t size) {
        std::string tmp;
        tmp.resize(size);
        // the move() here is for C++20
        ostream.str(std::move(tmp));
    }
    void resize(std::size_t size) { return reserve(size); }

    std::string ofname;
    std::ostringstream ostream;
};

output_std_strstream_io::output_std_strstream_io(const std::string &output_fname)
    :pimpl{new impl{output_fname}}
{}

io_type output_std_strstream_io::type() const { return io_type::std_strstreams; }
io_direction output_std_strstream_io::direction() const { return io_direction::output; }
void output_std_strstream_io::reset() { pimpl->ostream.seekp(0); }
const std::string& output_std_strstream_io::name() const { return pimpl->ofname; }
std::size_t output_std_strstream_io::size() const { return pimpl->size(); }
void output_std_strstream_io::reserve(std::size_t size) { return pimpl->reserve(size); }
void output_std_strstream_io::resize(std::size_t size) { return pimpl->resize(size); }

std::ostringstream& output_std_strstream_io::stream()
{ return pimpl->ostream; }

/*************************************************************************************************/
#if 0
struct input_stdio_stream_io::impl {
    impl(const std::string &input_fname)
        :ifname{input_fname}
        ,istream{}
    {
        istream = std::fopen(ifname.c_str(), "rb");
        assert(istream);
    }
    ~impl()
    { std::fclose(istream); }

    std::FILE* stream() {
        std::fseek(istream, 0, SEEK_SET);

        return istream;
    }
    void reset() { std::fseek(istream, SEEK_SET, 0); }
    std::size_t size() { return file_size(ifname.c_str()); }
    void reserve(std::size_t /*size*/) {}

    std::string ifname;
    std::FILE *istream;
};

input_stdio_stream_io::input_stdio_stream_io(const std::string &input_fname)
    :pimpl{new impl{input_fname}}
{}

io_type input_stdio_stream_io::type() const { return io_type::stdio_streams; }
io_direction input_stdio_stream_io::direction() const { return io_direction::input; }
void input_stdio_stream_io::reset() { return pimpl->reset(); }
const std::string& input_stdio_stream_io::name() const { return pimpl->ifname; }
std::size_t input_stdio_stream_io::size() const { return pimpl->size(); }
void input_stdio_stream_io::reserve(std::size_t size) { return pimpl->reserve(size); }

std::FILE* input_stdio_stream_io::stream()
{ return pimpl->stream(); }

/*************************************************************************************************/

struct output_stdio_stream_io::impl {
    impl(const std::string &output_fname)
        :ofname{output_fname}
        ,ostream{}
    {
        ostream = std::fopen(ofname.c_str(), "r+b");
        assert(ostream);
    }

    std::FILE* stream() { return ostream; }
    void reset() { std::fseek(ostream, SEEK_SET, 0); }
    std::size_t size() { return file_size(ofname.c_str()); }
    void reserve(std::size_t size) {
        assert(::ftruncate(fileno(ostream), size) == 0);
    }

    std::string ofname;
    std::FILE *ostream;
};

output_stdio_stream_io::output_stdio_stream_io(const std::string &output_fname)
    :pimpl{new impl{output_fname}}
{}

io_type output_stdio_stream_io::type() const { return io_type::stdio_streams; }
io_direction output_stdio_stream_io::direction() const { return io_direction::output; }
void output_stdio_stream_io::reset() { return pimpl->reset(); }
const std::string& output_stdio_stream_io::name() const { return pimpl->ofname; }
std::size_t output_stdio_stream_io::size() const { return pimpl->size(); }
void output_stdio_stream_io::reserve(std::size_t size) { return pimpl->reserve(size); }

std::FILE* output_stdio_stream_io::stream()
{ return pimpl->stream(); }

/*************************************************************************************************/

struct input_fd_stream_io::impl {
    impl(const std::string &input_fname)
        :ifname{input_fname}
        ,istream{-1}
    {
        istream = ::open(ifname.c_str(), O_RDONLY);
        assert(istream != -1);
    }

    int stream() {
        ::lseek(istream, 0, SEEK_SET);

        return istream;
    }
    void reset() { ::lseek(istream, 0, SEEK_SET); }
    std::size_t size() { return file_size(ifname.c_str()); }
    void reserve(std::size_t /*size*/) {}

    std::string ifname;
    int istream;
};

input_fd_stream_io::input_fd_stream_io(const std::string &input_fname)
    :pimpl{new impl{input_fname}}
{}

io_type input_fd_stream_io::type() const { return io_type::fd_streams; }
io_direction input_fd_stream_io::direction() const { return io_direction::input; }
void input_fd_stream_io::reset() { return pimpl->reset(); }
const std::string& input_fd_stream_io::name() const { return pimpl->ifname; }
std::size_t input_fd_stream_io::size() const { return pimpl->size(); }
void input_fd_stream_io::reserve(std::size_t size) { return pimpl->reserve(size); }

int input_fd_stream_io::stream()
{ return pimpl->stream(); }

/*************************************************************************************************/

struct output_fd_stream_io::impl {
    impl(const std::string &output_fname)
        :ofname{output_fname}
        ,ostream{-1}
    {
        ostream = ::open(ofname.c_str(), O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
        assert(ostream != -1);
    }

    int stream() { return ostream; }
    void reset() { ::lseek(ostream, 0, SEEK_SET); }
    std::size_t size() { return file_size(ofname.c_str()); }
    void reserve(std::size_t size) { assert(::ftruncate(ostream, size) == 0); }

    std::string ofname;
    int ostream;
};

output_fd_stream_io::output_fd_stream_io(const std::string &output_fname)
    :pimpl{new impl{output_fname}}
{}

io_type output_fd_stream_io::type() const { return io_type::fd_streams; }
io_direction output_fd_stream_io::direction() const { return io_direction::output; }
void output_fd_stream_io::reset() { return pimpl->reset(); }
const std::string& output_fd_stream_io::name() const { return pimpl->ofname; }
std::size_t output_fd_stream_io::size() const { return pimpl->size(); }
void output_fd_stream_io::reserve(std::size_t size) { return pimpl->reserve(size); }

int output_fd_stream_io::stream()
{ return pimpl->stream(); }

/*************************************************************************************************/
#endif

struct input_mmap_stream_io::impl {
    impl(const std::string &input_fname)
        :ifname{input_fname}
        ,istream{ifname.c_str()}
    {}

    std::pair<char *, std::size_t> stream() { return {istream.data(), istream.size()}; }
    void reset() {}
    std::size_t size() { return istream.size(); }
    void reserve(std::size_t size) { assert(size || "UNIMPLEMENTED!"); }
    void resize(std::size_t size) { assert(size || "UNIMPLEMENTED!"); }

    std::string ifname;
    mmsource istream;
};

input_mmap_stream_io::input_mmap_stream_io(const std::string &input_fname)
    :pimpl{new impl{input_fname}}
{}

io_type input_mmap_stream_io::type() const { return io_type::mmap_streams; }
io_direction input_mmap_stream_io::direction() const { return io_direction::input; }
void input_mmap_stream_io::reset() { return pimpl->reset(); }
const std::string& input_mmap_stream_io::name() const { return pimpl->ifname; }
std::size_t input_mmap_stream_io::size() const { return pimpl->size(); }
void input_mmap_stream_io::reserve(std::size_t size) { return pimpl->reserve(size); }
void input_mmap_stream_io::resize(std::size_t size) { return pimpl->resize(size); }

std::pair<char *, std::size_t> input_mmap_stream_io::stream()
{ return pimpl->stream(); }

/*************************************************************************************************/

struct output_mmap_stream_io::impl {
    impl(const std::string &output_fname)
        :ofname{output_fname}
        ,ostream{ofname.c_str(), 1024}
    {}

    std::pair<char *, std::size_t> stream() {
        return {ostream.data(), ostream.size()};
    }

    void reset() {}

    std::size_t size() { return ostream.size(); }
    void reserve(std::size_t size) { ostream.resize(size); }
    void resize(std::size_t size) { return reserve(size); }

    std::string ofname;
    mmsink ostream;
};

output_mmap_stream_io::output_mmap_stream_io(const std::string &output_fname)
    :pimpl{new impl{output_fname}}
{}

io_type output_mmap_stream_io::type() const { return io_type::mmap_streams; }
io_direction output_mmap_stream_io::direction() const { return io_direction::output; }
void output_mmap_stream_io::reset() { return pimpl->reset(); }
const std::string& output_mmap_stream_io::name() const { return pimpl->ofname; }
std::size_t output_mmap_stream_io::size() const { return pimpl->size(); }
void output_mmap_stream_io::reserve(std::size_t size) { return pimpl->reserve(size); }
void output_mmap_stream_io::resize(std::size_t size) { return pimpl->resize(size); }

std::pair<char *, std::size_t> output_mmap_stream_io::stream()
{ return pimpl->stream(); }

/*************************************************************************************************/

std::unique_ptr<io_device> create_io(io_direction dir, io_type type, const std::string &fname) {
    using ptr = std::unique_ptr<io_device>;
    using creator = ptr (*)(const std::string &);
    static const creator map[2][6] = {
         {   [](const std::string &fname) -> ptr { return std::make_unique<input_string_buffer_io>(fname); }
            ,[](const std::string &fname) -> ptr { return std::make_unique<input_std_strstream_io>(fname); }
//            ,[](const std::string &fname) -> ptr { return std::make_unique<input_std_fstream_io>(fname); }
//            ,[](const std::string &fname) -> ptr { return std::make_unique<input_stdio_stream_io>(fname); }
//            ,[](const std::string &fname) -> ptr { return std::make_unique<input_fd_stream_io>(fname); }
            ,[](const std::string &fname) -> ptr { return std::make_unique<input_mmap_stream_io>(fname); }
         }
        ,{   [](const std::string &fname) -> ptr { return std::make_unique<output_string_buffer_io>(fname); }
            ,[](const std::string &fname) -> ptr { return std::make_unique<output_std_strstream_io>(fname); }
//            ,[](const std::string &fname) -> ptr { return std::make_unique<output_std_fstream_io>(fname); }
//            ,[](const std::string &fname) -> ptr { return std::make_unique<output_stdio_stream_io>(fname); }
//            ,[](const std::string &fname) -> ptr { return std::make_unique<output_fd_stream_io>(fname); }
            ,[](const std::string &fname) -> ptr { return std::make_unique<output_mmap_stream_io>(fname); }
         }
    };

    auto fnptr = map[static_cast<std::size_t>(dir)][static_cast<std::size_t>(type)];
    auto up = fnptr(fname);

    return up;
}

/*************************************************************************************************/

} // ns json_benchmarks
