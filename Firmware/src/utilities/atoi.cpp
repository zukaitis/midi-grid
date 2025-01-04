#include "utilities/atoi.h"

namespace utilities
{

int32_t atoi( const etl::string_view& string )
{
    int32_t result = 0;
    int32_t multiplier = 1;

    if (string.empty())
    {
        result = -1;
    }

    for (int32_t c = string.length() - 1; c >= 0; c--)
    {
        if ((string.at(c) < '0') || (string.at(c) > '9'))
        {
            result = -1; // error
            break;
        }

        result += static_cast<int32_t>(string.at(c) - '0') * multiplier;
        multiplier *= 10;
    }

    return result;
}

} // namespace utilities