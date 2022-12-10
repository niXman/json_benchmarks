
#pragma once

#include <string>
#include <memory>
#include <iosfwd>

#include <cassert>

namespace json_benchmarks {

/*************************************************************************************************/

// don't rearrange!
enum class io_type {
     string_buffer // string used as I/O buffer
    ,std_strstreams// istringstream/istringstream
//    ,std_fstreams  // std::istream/std::ostream
//    ,stdio_streams // fopen()/fread(), etc
//    ,fd_streams    // open()/read(), etc
    ,mmap_streams  // memory mapped
};

struct input_string_buffer_io;
struct input_std_strstream_io;
//struct input_std_fstream_io;
//struct input_stdio_stream_io;
//struct input_fd_stream_io;
struct input_mmap_stream_io;

struct output_string_buffer_io;
struct output_std_strstream_io;
//struct output_std_fstream_io;
//struct output_stdio_stream_io;
//struct output_fd_stream_io;
struct output_mmap_stream_io;

// don't rearrange!
enum class io_direction {
     input
    ,output
};

struct io_device {
    virtual ~io_device() = default;
    virtual io_type type() const = 0;
    virtual io_direction direction() const = 0;
    virtual void reset() = 0;
    virtual const std::string& name() const = 0;
    virtual std::size_t size() const = 0;
    virtual void reserve(std::size_t size) = 0;
    virtual void resize(std::size_t size) = 0;

    template<io_type type>
    auto* input_io() {
        using set = std::tuple<
             input_string_buffer_io
            ,input_std_strstream_io
//            ,input_std_fstream_io
//            ,input_stdio_stream_io
//            ,input_fd_stream_io
            ,input_mmap_stream_io
        >;
        using impl_type = typename std::tuple_element<static_cast<std::size_t>(type), set>::type;

        auto *dst = static_cast<impl_type *>(this);
        assert(dst->type() == type);

        return dst;
    }
    template<io_type type>
    auto* output_io() {
        using set = std::tuple<
             output_string_buffer_io
            ,output_std_strstream_io
//            ,output_std_fstream_io
//            ,output_stdio_stream_io
//            ,output_fd_stream_io
            ,output_mmap_stream_io
        >;
        using impl_type = typename std::tuple_element<static_cast<std::size_t>(type), set>::type;

        auto *dst = static_cast<impl_type *>(this);
        assert(dst->type() == type);

        return dst;
    }
};

/*************************************************************************************************/

struct input_string_buffer_io: io_device {
    // the input file will be read into a string on construction
    input_string_buffer_io(const std::string &input_fname);
    virtual ~input_string_buffer_io() = default;

    virtual io_type type() const override;
    virtual io_direction direction() const override;
    virtual void reset() override;
    virtual const std::string& name() const override;
    virtual std::size_t size() const override;
    virtual void reserve(std::size_t size) override;
    virtual void resize(std::size_t size) override;

    std::string& stream();

private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};

struct output_string_buffer_io: io_device {
    output_string_buffer_io(const std::string &output_fname);
    virtual ~output_string_buffer_io() = default;

    virtual io_type type() const override;
    virtual io_direction direction() const override;
    virtual void reset() override;
    virtual const std::string& name() const override;
    virtual std::size_t size() const override;
    virtual void reserve(std::size_t size) override;
    virtual void resize(std::size_t size) override;

    std::string& stream();

private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};

#if 0
struct input_std_fstream_io: io_device {
    input_std_fstream_io(const std::string &input_fname);
    virtual ~input_std_fstream_io() = default;

    virtual io_type type() const override;
    virtual io_direction direction() const override;
    virtual void reset() override;
    virtual const std::string& name() const override;
    virtual std::size_t size() const override;
    virtual void reserve(std::size_t size) override;

    std::istream& stream();

private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};

struct output_std_fstream_io: io_device {
    output_std_fstream_io(const std::string &output_fname);
    virtual ~output_std_fstream_io() = default;

    virtual io_type type() const override;
    virtual io_direction direction() const override;
    virtual void reset() override;
    virtual const std::string& name() const override;
    virtual std::size_t size() const override;
    virtual void reserve(std::size_t size) override;

    std::ostream& stream();

private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};
#endif

struct input_std_strstream_io: io_device {
    // the input file will be read into input stream on construction
    input_std_strstream_io(const std::string &input_fname);
    virtual ~input_std_strstream_io() = default;

    virtual io_type type() const override;
    virtual io_direction direction() const override;
    virtual void reset() override;
    virtual const std::string& name() const override;
    virtual std::size_t size() const override;
    virtual void reserve(std::size_t size) override;
    virtual void resize(std::size_t size) override;

    std::istream& stream();

private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};

struct output_std_strstream_io: io_device {
    output_std_strstream_io(const std::string &output_fname);
    virtual ~output_std_strstream_io() = default;

    virtual io_type type() const override;
    virtual io_direction direction() const override;
    virtual void reset() override;
    virtual const std::string& name() const override;
    virtual std::size_t size() const override;
    virtual void reserve(std::size_t size) override;
    virtual void resize(std::size_t size) override;

    std::ostringstream& stream();

private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};

#if 0
struct input_stdio_stream_io: io_device {
    input_stdio_stream_io(const std::string &input_fname);
    virtual ~input_stdio_stream_io() = default;

    virtual io_type type() const override;
    virtual io_direction direction() const override;
    virtual void reset() override;
    virtual const std::string& name() const override;
    virtual std::size_t size() const override;
    virtual void reserve(std::size_t size) override;

    std::FILE* stream();

private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};

struct output_stdio_stream_io: io_device {
    output_stdio_stream_io(const std::string &output_fname);
    virtual ~output_stdio_stream_io() = default;

    virtual io_type type() const override;
    virtual io_direction direction() const override;
    virtual void reset() override;
    virtual const std::string& name() const override;
    virtual std::size_t size() const override;
    virtual void reserve(std::size_t size) override;

    std::FILE* stream();

private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};

struct input_fd_stream_io: io_device {
    input_fd_stream_io(const std::string &input_fname);
    virtual ~input_fd_stream_io() = default;

    virtual io_type type() const override;
    virtual io_direction direction() const override;
    virtual void reset() override;
    virtual const std::string& name() const override;
    virtual std::size_t size() const override;
    virtual void reserve(std::size_t size) override;

    int stream();

private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};

struct output_fd_stream_io: io_device {
    output_fd_stream_io(const std::string &output_fname);
    virtual ~output_fd_stream_io() = default;

    virtual io_type type() const override;
    virtual io_direction direction() const override;
    virtual void reset() override;
    virtual const std::string& name() const override;
    virtual std::size_t size() const override;
    virtual void reserve(std::size_t size) override;

    int stream();

private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};
#endif

struct input_mmap_stream_io: io_device {
    input_mmap_stream_io(const std::string &input_fname);
    virtual ~input_mmap_stream_io() = default;

    virtual io_type type() const override;
    virtual io_direction direction() const override;
    virtual void reset() override;
    virtual const std::string& name() const override;
    virtual std::size_t size() const override;
    virtual void reserve(std::size_t size) override;
    virtual void resize(std::size_t size) override;

    std::pair<char *, std::size_t> stream();

private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};

struct output_mmap_stream_io: io_device {
    output_mmap_stream_io(const std::string &output_fname);
    virtual ~output_mmap_stream_io() = default;

    virtual io_type type() const override;
    virtual io_direction direction() const override;
    virtual void reset() override;
    virtual const std::string& name() const override;
    virtual std::size_t size() const override;
    virtual void reserve(std::size_t size) override;
    virtual void resize(std::size_t size) override;

    std::pair<char *, std::size_t> stream();

private:
    struct impl;
    std::unique_ptr<impl> pimpl;
};

/*************************************************************************************************/

std::unique_ptr<io_device> create_io(io_direction dir, io_type type, const std::string &fname);

/*************************************************************************************************/

} // ns json_benchmarks
