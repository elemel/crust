#ifndef CRUST_ERROR_HPP
#define CRUST_ERROR_HPP

#include <stdexcept>
#include <string>

namespace crust {
    class Error : public std::exception {
    public:
        explicit Error(char const *message) :
            message_(message)
        { }

        explicit Error(std::string const &message) :
            message_(message)
        { }

        ~Error() throw ()
        { }

        char const *what() const throw ()
        {
            return message_.c_str();
        }
        
    private:
        std::string message_;
    };
}

#endif
