#include "InputSanitizer.hpp"

#include <cctype>

#include <boost/algorithm/string/split.hpp>


namespace {
BookingInput spliterate(const std::string& rawText) {
    BookingInput result;
    boost::split(result, rawText, [](char c){ return c== ',' || c == ' '; }, boost::token_compress_on);
    return result;
}

void normalizeCase(BookingInput::value_type& entry) {
    std::transform(entry.begin(), entry.end(),
                    entry.begin(),
                    [](char c){ return std::tolower(static_cast<unsigned char>(c)); });
}

BookingInput::value_type filterInvalidCharacters(const BookingInput::value_type& entry) {
    BookingInput::value_type result;
    std::copy_if(entry.begin(), entry.end(),
                std::back_inserter(result),
                [](char c){ return std::isalnum(static_cast<unsigned char>(c)); });
    return result;
}

}

/**
 * Split, normalize case to lowercase, and remove non-alpha, non-numeric characters.
 * The items are separated by comma, space, or  (,)
 *
 * @param rawStr The input string as the user entered it in the interface, containing
 *               the desired seats to be reserved.
 */
BookingInput convertAndSanitizeInput(const std::string& rawStr) {
    BookingInput cleanStrings;
    for(auto&& entry : spliterate(rawStr)) {
        normalizeCase(entry);
        auto result(filterInvalidCharacters(entry));
        if(result.size()) {
            cleanStrings.push_back(std::move(result));
        }
    }
    return cleanStrings;
}
