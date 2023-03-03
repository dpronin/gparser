#include <cctype>
#include <cstdint>
#include <exception>
#include <limits>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>

namespace gtask {

struct params {
    std::string::size_type out_max_len = std::numeric_limits<std::string::size_type>::max();
};

template <typename Iterator>
std::string parse(Iterator first, Iterator last, params const &params = {})
{
    std::string result;
    std::stack<std::pair<std::string, uint32_t>> s;

    std::string::size_type result_estim_len = std::string::size_type{0};

    for (std::string *p = &result; first != last; ++first) {
        switch (auto const c = *first) {
        case 'a'...'z':
        case 'A'...'Z':
            if (++result_estim_len > params.out_max_len)
                throw std::range_error(std::string{"result string exceeds max len "} + std::to_string(params.out_max_len) + " given in params");
            p->push_back(c);
            break;
        case '1'...'9': {
            uint64_t k = 0;
            for (; k <= std::numeric_limits<uint32_t>::max() && first != last && std::isdigit(*first); k = k * 10 + (*first++ - '0'))
                ;

            if (k > std::numeric_limits<uint32_t>::max())
                throw std::overflow_error("number overflow detected before expression []");

            if (first == last)
                throw std::invalid_argument(std::string{"unexpected end of expression "} + std::to_string(k) + "[]");

            if (*first != '[')
                throw std::invalid_argument(std::string{"unexpected character '"} + *first + "', expected '['");

            s.push({{}, static_cast<uint32_t>(k)});
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

            result_estim_len += (k - 1) * r.size();
            if (result_estim_len > params.out_max_len)
                throw std::range_error(std::string{"result string exceeds max len "} + std::to_string(params.out_max_len) + " given in params");

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

inline std::string parse(std::string_view input, params const &params = {})
{
    return parse(input.cbegin(), input.cend(), params);
}

} // namespace gtask
