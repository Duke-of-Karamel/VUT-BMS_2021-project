#ifndef __RSCCODER_HPP__
#define __RSCCODER_HPP__


class RSCCoder
{
public:
    RSCCoder();

    int getState() const {return state;}

    const char* getStateCodetxt();
    char next(char bit_in);

private:
    int state;
};

#endif
