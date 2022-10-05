#include <fstream>
#include <boost/json.hpp>
#include <boost/json/src.hpp>

using namespace boost::json;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        exit(1);
    }
    try
    {
        value val;
        stream_parser stream;
        std::ifstream is(argv[1]);
        if ( !is ) {
            exit(1);
        }

        char buf[4096];
        do {
            is.read(buf, sizeof(buf));
            auto rd = is.gcount();
            stream.write(buf, rd);
        } while ( !is.eof() );

        stream.finish();
        val = stream.release();

        return !(!val.is_null());
    }
    catch (const std::exception&)
    {
        return 1;
    }
}
