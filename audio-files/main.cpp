#include "riff.cpp"

int main(void) {
    std::cout << "Start..." << std::endl;
    RiffChunk test;

    std::cout << "0:" << std::endl;

    test.set_ckid(RiffChunk::CKID_RIFF);

    std::cout << "1:" << std::endl;

    test.set_cksize(5);

    std::cout << test.get_ckid() << std::endl;
    std::cout << test.get_cksize() << std::endl;

    std::cout << "2:" << std::endl;

    test.overwrite_data("Hello!-");

    std::cout << test.get_cksize() << std::endl;
    std::cout << test.get_data() << std::endl;
    std::cout << test.get_raw() << std::endl;
    std::cout << test.get_raw().length() << std::endl;
    return 0;
}
