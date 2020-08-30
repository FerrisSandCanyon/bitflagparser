// g++ args: -Wall -Ofast -std=c++17 -static-libgcc -static-libstdc++

#include <stdio.h>

#define __ver "1.1"

#define ERR_TERMINATE(msg, retcode) \
puts(msg); \
return retcode;

#define ERR_TERMINATE_ARG(msg, arg, retcode) \
printf(msg, arg); \
return retcode;

enum class BITFLAG_TYPE
{
    BFT_BIN,
    BFT_DEC,
    BFT_HEX,
    BFT_UNK,
};

constexpr int ce_strlen(const char* str)
{
    int len = 0;
    while (str[len++]);
    return len;
}

int main(int argc, char** argv)
{
    const char*        bfstr_start  = argv[1];               // Starting point of the bitflag
    const char*        bfstr_end    = argv[1];               // End point of the bitflag
    BITFLAG_TYPE       bfstr_type   = BITFLAG_TYPE::BFT_BIN; // Bit flag data type
    unsigned long long bfstr_result = 0;                     // Result of the bitflags parsed from the parameter
    unsigned int       opt_hexlen   = 0;                     // printf width for printing hex values

    // Check if an argument was supplied
    if (!bfstr_start)
    {
        ERR_TERMINATE_ARG("Please supply a parameter that can represent a bitflag to be parsed!\r\n"
                          "Usage: %s < value > <hex width count>\r\n",
                          argv[0], 1);
    }

    if ([&, argv]() -> bool {
        constexpr int _help_len = ce_strlen("--help");
        for (int idx = 0; idx < _help_len; idx++)
            if (argv[1][idx] != "--help"[idx])
                return false;
        return true;
    }()) {
        puts("Bitflag parser v" __ver "\r\n"
             "A tool for separating the individual bitflags from the provided parameter.\r\n"
             "Build date: " __DATE__ " " __TIME__ "\r\n");
        return 0;
    }

    // Check if hex width is supplied
    if (argv[2])
    {
        do
        {
            if (*argv[2] < '0' || *argv[2] > '9')
            {
                ERR_TERMINATE_ARG("Invalid argument or hex width parameter contains an invalid value. At: %c", *argv[2], 2);
            }

            opt_hexlen *= 10;
            opt_hexlen += *argv[2] - '0';

        } while (*++argv[2]);

        if (opt_hexlen > 16)
        {
            ERR_TERMINATE("Hex width count exceeded maximum width space. (Max is 16 for 8 bytes / 64 bits)", 3);
        }
    }
    
    // Parse the argument, checks if invalid, and determines the type
    do
    {
        bfstr_end++;

        // Prefix type forcing
        // Can only force type to binary if the current determined type is binary since if the determined type is decimal it means the param contains a value out of the binary's range (2-9)
        if (bfstr_type == BITFLAG_TYPE::BFT_BIN && (*(bfstr_end + 1) == 'B' || *(bfstr_end + 1) == 'b') )
        {
            break;
        }
        // Can only force type to decimal if the current determined type is either binary or decimal since if the determined type is hexadecimal it means the param contains a value out of the binary and decimal's range (A-F)
        else if (bfstr_type != BITFLAG_TYPE::BFT_HEX && (*(bfstr_end + 1) == 'D' || *(bfstr_end + 1) == 'd') )
        {
            bfstr_type = BITFLAG_TYPE::BFT_DEC;
            break;
        }
        // Can force the type to hexadecimal regardless of current determined type
        else if (*(bfstr_end + 1) == 'H' || *(bfstr_end + 1) == 'h') 
        {
            bfstr_type = BITFLAG_TYPE::BFT_HEX;
            break;
        }

        // Ensures parameter is in binary range
        if (bfstr_type == BITFLAG_TYPE::BFT_BIN)
        {
            if (*bfstr_end == '0' || *bfstr_end == '1')
            {
                continue;
            }
            else
                bfstr_type = BITFLAG_TYPE::BFT_DEC;
        }

        // Ensures parameter is in numerical range
        if (bfstr_type == BITFLAG_TYPE::BFT_DEC || bfstr_type == BITFLAG_TYPE::BFT_HEX)
        {
            if (*bfstr_end >= '0' && *bfstr_end <= '9')
            {
                continue;
            }
            else
                bfstr_type = BITFLAG_TYPE::BFT_HEX;
        }

        // Ensures parameter is in hex 10/A-15/F range, numerical hex is checked by the previous statement
        if ( (*bfstr_end >= 'A' && *bfstr_end <= 'F') ||  (*bfstr_end >= 'a' && *bfstr_end <= 'f') )
            continue;

        // Allow 'x' that's used in hex format
        if (*bfstr_end == 'X' || *bfstr_end == 'x')
        {
            // If the current index indicates a X or x, set the starting position +1 from it to properly set the starting point of the hex format
            if ( (*bfstr_end == 'X' || *bfstr_end == 'x') 
            &&   (*(bfstr_start = bfstr_end + 1) == '\0') 
            ) {
                ERR_TERMINATE("Invalid argument. Hex format is invalid.", 2);
            }

            bfstr_type = BITFLAG_TYPE::BFT_HEX;
            continue;
        }
        
        ERR_TERMINATE_ARG("Invalid argument or type cannot be determined. At: %c", *bfstr_end, 4);
    } while(*(bfstr_end + 1));

    // Convert the parameter to the individual bits
    do
    {
        switch (bfstr_type)
        {
            case BITFLAG_TYPE::BFT_BIN:
            {
                bfstr_result <<= 1;
                bfstr_result += *bfstr_start == '1' ? 1 : 0;
                continue;
            }

            case BITFLAG_TYPE::BFT_DEC:
            {
                bfstr_result *= 10;
                bfstr_result += *bfstr_start - '0';
                continue;
            }

            case BITFLAG_TYPE::BFT_HEX:
            {
                bfstr_result <<= 4;
                if (*bfstr_start >= '0' && *bfstr_start <= '9')
                    bfstr_result += *bfstr_start - '0';
                else if (*bfstr_start >= 'A' && *bfstr_start <= 'F')
                    bfstr_result += (*bfstr_start - 'A') + 10;
                else if (*bfstr_start >= 'a' && *bfstr_start <= 'f')
                    bfstr_result += (*bfstr_start - 'a') + 10;
                continue;
            }

            case BITFLAG_TYPE::BFT_UNK:
            default:
            {
                ERR_TERMINATE_ARG("Invalid argument or type cannot be determined. At: %c", *bfstr_start, 5);
                break;
            }
        }
    } while (++bfstr_start <= bfstr_end);    
    
    static const char* BITFLAG_TYPE_STR[] =
    {
        "Binary",
        "Decimal",
        "Hexadecimal",
        "Unknown type"
    };

    // Print information
    printf("\r\n"
           "Type: %s\r\n"
           "Result: 0x%llx\r\n"
           "==================[Bitflags]==================\r\n",
           BITFLAG_TYPE_STR[static_cast<int>(bfstr_type)], bfstr_result);

    unsigned long long currentbit = 0x1ull;

    do
    {
        unsigned long long result = currentbit & bfstr_result;

        if (result)
            printf("Hex: 0x%0*llx, Dec: %lld\r\n", opt_hexlen, result, result);

    } while ( (currentbit <<= 1) != (1ull << 63) );
    

    puts(  "==============================================\r\n");
}