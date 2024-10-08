// https://stackoverflow.com/questions/9459136/how-can-i-make-an-ostream-reference-an-ofstream-c

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>

class CustomLog {
    private:
        std::ostream* target;
        std::string   target_filename;

        void dealloc() {
            if ( target_filename.length() != 0 )
                delete target;
        }

        void alloc(std::string filename) {
            std::ofstream* new_filestream = new std::ofstream(filename);

            // Try to open filestream to file
            if ( !new_filestream->is_open() ) {
                *target << "Could not allocate a filestream to file " << filename << "!\n";
                return;
            }

            // Remove old filestream (if applicable)
            dealloc();

            // Update values
            target_filename = filename;
            target          = new_filestream;
        }

    public:

        // Allow ostream input with std::cout as default
        CustomLog(std::ostream& log_stream = std::cout): target(&log_stream), target_filename("") {
        }

        // Allow filepath allocation
        CustomLog(std::string   filename): target_filename(filename) {
            alloc(filename);
        }

        ~CustomLog() {
            dealloc();
        }

        std::string get_logfile() {
            return target_filename;
        }

        const std::ostream& get_ostream() {
            return *target;
        }

        void set_logfile( std::string filename ) {
            alloc(filename);
        }

        void set_ostream() {

        }

        bool is_cout() {
            return (target == &std::cout);
        }

        bool is_cerr() {
            return (target == &std::cerr);
        }

        CustomLog operator << (const std::string& text) {
            (*target) << text;
        }
        // friend std::ostream & operator << (std::ostream &out, const CustomLog &val) { }
};

static CustomLog base_logger;

void custom_assert( bool is_asserted, std::string error_msg, CustomLog logger, int exit_code = 1 ) {
    if ( is_asserted )
        return;
    // Else
    logger << error_msg << "\n";
    exit(exit_code);
}

void assign_logfile( std::string filename ) {
    base_logger.set_logfile(filename);
}
