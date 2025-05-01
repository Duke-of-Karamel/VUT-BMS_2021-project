
#include <iostream>

#include "RSCCoder.hpp"

RSCCoder::RSCCoder()
{
    state = 0;
}

const char* RSCCoder::getStateCodetxt() {
    switch(state)
    {
        case 0:
            state = 0;
            return "00";
        case 1:
            state = 0;
            return "10"; // for reasons LSB MSB is swaped
        case 2:
            state = 0;
            return "01"; // for reasons LSB MSB is swaped
        case 3:
            state = 0;
            return "11";
    }
    state = 0;
    return "err\n";
}

char RSCCoder::next(char bit_in)
{
    switch (bit_in)
    {
    case '0':
        switch(state)
        {
            case 0:
                state = 0;
                return '0';
            case 1:
                state = 2;
                return '1';
            case 2:
                state = 1;
                return '0';
            case 3:
                state = 3;
                return '1';
        }
        break;
    case '1':
        switch(state)
        {
            case 0:
                state = 2;
                return '1';
            case 1:
                state = 0;
                return '0';
            case 2:
                state = 3;
                return '1';
            case 3:
                state = 1;
                return '0';
        }
        break;
    default:
        std::cerr << "Wrong bit on input";
        return '\n';
    }
    return '\n';
}
