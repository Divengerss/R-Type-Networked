#ifndef ERROR_HPP_
#define ERROR_HPP_

#include <exception>
#include <string>

class Error: public std::exception {
    public:
        Error(const std::string &message, const std::string &where = "") noexcept :
            _message(message)
        {
            if (where.size() != 0)
                _message += " occured in " + where;
        }

        ~Error() = default;

        const char *what() const noexcept final {return ( _message.c_str());}

    private:
        std::string _message;
};

#endif /* !ERROR_HPP_ */