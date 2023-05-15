#pragma once

#include <string>
#include <vector>

typedef std::vector<std::string> BookingInput;

/**
 * Split, normalize case to lowercase, and remove non-alpha, non-numeric characters.
 * The items are separated by comma, space, or  (,)
 *
 * @param rawStr The input string as the user entered it in the interface, containing
 *               the desired seats to be reserved.
 */
BookingInput convertAndSanitizeInput(const std::string& rawStr);
