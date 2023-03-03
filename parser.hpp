#include <cctype>
#include <exception>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>

namespace gtask {

template <typename Iterator>
std::string parse(Iterator first, Iterator last)
{
    std::string result;

    std::stack<std::pair<std::string, uint32_t>> s;

    for (std::string *p = &result; first != last; ++first) {
        switch (auto const c = *first) {
        case 'a'...'z':
        case 'A'...'Z':
            p->push_back(c);
            break;
        case '1'...'9': {
            uint32_t k = 0;
            for (; first != last && std::isdigit(*first); k = k * 10 + (*first++ - '0'))
                ;

            if (first == last)
                throw std::invalid_argument(std::string{"unexpected end of expression "} + std::to_string(k) + "[]");

            if (*first != '[')
                throw std::invalid_argument(std::string{"unexpected character '"} + *first + "', expected '['");

            s.push({{}, k});
            p = &s.top().first;

            break;
        }
        case ']': {
            if (s.empty())
                throw std::invalid_argument("unexpected expression termination ']'");

            auto const [r, k] = std::move(s.top());

            if (r.empty())
                throw std::invalid_argument("empty string parsed inside the expression []");

            s.pop();
            p = s.empty() ? &result : &s.top().first;

            p->reserve(p->size() + k * r.size());
            for (auto n = k; n--;)
                *p += r;
        } break;
        default:
            throw std::invalid_argument(std::string{"unexpected character '"} + c + "'");
        }
    }

    if (!s.empty())
        throw std::invalid_argument("unexpected end of expression []");

    return result;
}

inline std::string parse(std::string_view input)
{
    return parse(input.cbegin(), input.cend());
}

} // namespace gtask
