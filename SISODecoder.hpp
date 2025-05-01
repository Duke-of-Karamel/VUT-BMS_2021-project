#ifndef __SISODECODER_HPP__
#define __SISODECODER_HPP__


class SISODecoder
{
public:
    SISODecoder(int num_bits);
    ~SISODecoder();

    char process(const double * array_in1, const double * array_in2, const double* array_in3);
    double* getLLR() const {return loglikeli;}

private:
    int num_bits;
    double (*branches)[4][2];
    double (*forward) [4];
    double (*backward)[4];
    double *loglikeli;
};

#endif
