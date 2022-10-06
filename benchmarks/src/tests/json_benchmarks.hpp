#ifndef JSON_BENCHMARKS_HPP
#define JSON_BENCHMARKS_HPP

#include "measurements.hpp"
#include <vector>
#include <memory>

namespace json_benchmarks {

    class benchmarks
    {
    public:
        virtual ~benchmarks() = default;
        virtual measurements measure_small(const std::string& input, std::string& output) = 0;
        virtual measurements measure_big(const char *input_filename, const char* output_filename) = 0;
        virtual std::vector<test_suite_result> run_test_suite(std::vector<test_suite_file>& pathnames) = 0;

        virtual const std::string& name() const = 0;
        virtual const std::string& url() const = 0;
        virtual const std::string& version() const = 0;
        virtual const std::string& notes() const = 0;
    };
    using benchmarks_ptr = std::shared_ptr<benchmarks>;

    class cjson_benchmarks : public benchmarks
    {
    public:
        measurements measure_small(const std::string& input, std::string& output) override;
        measurements measure_big(const char *input_filename, const char* output_filename) override;
        std::vector<test_suite_result> run_test_suite(std::vector<test_suite_file>& pathnames) override;

        const std::string& name() const override;
        const std::string& url() const override;
        const std::string& version() const override;
        const std::string& notes() const override;
    };

    class json11_benchmarks : public benchmarks
    {
    public:
        measurements measure_small(const std::string& input, std::string& output) override;
        measurements measure_big(const char *input_filename, const char* output_filename) override;
        std::vector<test_suite_result> run_test_suite(std::vector<test_suite_file>& pathnames) override;

        const std::string& name() const override;
        const std::string& url() const override;
        const std::string& version() const override;
        const std::string& notes() const override;
    };

    class json_spirit_benchmarks : public benchmarks
    {
    public:
        measurements measure_small(const std::string& input, std::string& output) override;
        measurements measure_big(const char *input_filename, const char* output_filename) override;
        std::vector<test_suite_result> run_test_suite(std::vector<test_suite_file>& pathnames) override;

        const std::string& name() const override;
        const std::string& url() const override;
        const std::string& version() const override;
        const std::string& notes() const override;
    };

    class jsoncons_benchmarks : public benchmarks
    {
    public:
        measurements measure_small(const std::string& input, std::string& output) override;
        measurements measure_big(const char *input_filename, const char* output_filename) override;
        std::vector<test_suite_result> run_test_suite(std::vector<test_suite_file>& pathnames) override;

        const std::string& name() const override;
        const std::string& url() const override;
        const std::string& version() const override;
        const std::string& notes() const override;
    };

    class jsoncpp_benchmarks : public benchmarks
    {
    public:
        measurements measure_small(const std::string& input, std::string& output) override;
        measurements measure_big(const char *input_filename, const char* output_filename) override;
        std::vector<test_suite_result> run_test_suite(std::vector<test_suite_file>& pathnames) override;

        const std::string& name() const override;
        const std::string& url() const override;
        const std::string& version() const override;
        const std::string& notes() const override;
    };

    class nlohmann_benchmarks : public benchmarks
    {
    public:
        measurements measure_small(const std::string& input, std::string& output) override;
        measurements measure_big(const char *input_filename, const char* output_filename) override;
        std::vector<test_suite_result> run_test_suite(std::vector<test_suite_file>& pathnames) override;

        const std::string& name() const override;
        const std::string& url() const override;
        const std::string& version() const override;
        const std::string& notes() const override;
    };

    class rapidjson_benchmarks : public benchmarks
    {
    public:
        measurements measure_small(const std::string& input, std::string& output) override;
        measurements measure_big(const char *input_filename, const char* output_filename) override;
        std::vector<test_suite_result> run_test_suite(std::vector<test_suite_file>& pathnames) override;

        const std::string& name() const override;
        const std::string& url() const override;
        const std::string& version() const override;
        const std::string& notes() const override;
    };

    class taojson_benchmarks : public benchmarks
    {
    public:
        measurements measure_small(const std::string& input, std::string& output) override;
        measurements measure_big(const char *input_filename, const char* output_filename) override;
        std::vector<test_suite_result> run_test_suite(std::vector<test_suite_file>& pathnames) override;

        const std::string& name() const override;
        const std::string& url() const override;
        const std::string& version() const override;
        const std::string& notes() const override;
    };
}

#endif

