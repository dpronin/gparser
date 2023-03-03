#include <cctype>
#include <cstddef>
#include <cstdint>

#include <exception>
#include <iterator>
#include <limits>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>

namespace gtask {

struct params {
    size_t out_max_len = std::numeric_limits<size_t>::max();
};

template <typename InputIt, typename OutputIt>
OutputIt parse(InputIt first, InputIt last, OutputIt result, params const &params = {})
{
    std::stack<std::pair<std::string, uint32_t>> s;

    size_t result_estim_len{0};

    for (; first != last; ++first) {
        switch (auto const c = *first) {
        case 'a'...'z':
        case 'A'...'Z':
            if (++result_estim_len > params.out_max_len)
                throw std::range_error(std::string{"result string exceeds max len "} + std::to_string(params.out_max_len) + " given in params");

            if (s.empty())
                *result++ = c;
            else
                s.top().first.push_back(c);

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

            break;
        }
        case ']': {
            if (s.empty())
                throw std::invalid_argument("unexpected expression termination ']'");

            auto const [r, k] = std::move(s.top());

            if (r.empty())
                throw std::invalid_argument("empty string parsed inside the expression []");

            result_estim_len += (k - 1) * r.size();
            if (result_estim_len > params.out_max_len)
                throw std::range_error(std::string{"result string exceeds max len "} + std::to_string(params.out_max_len) + " given in params");

            s.pop();
            if (s.empty()) {
                for (auto n = k; n--;)
                    result = std::copy(r.cbegin(), r.cend(), result);
            } else {
                auto &expr = s.top().first;
                expr.reserve(expr.size() + k * r.size());
                for (auto n = k; n--;)
                    expr += r;
            }
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
    std::string result;
    result.reserve(std::min((input.size() + 1) / 2, params.out_max_len));
    parse(input.cbegin(), input.cend(), std::back_inserter(result), params);
    return result;
}

} // namespace gtask
