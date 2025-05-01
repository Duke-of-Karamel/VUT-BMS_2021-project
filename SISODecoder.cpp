
#include <limits>
#include <algorithm>
#include <vector>

#include "SISODecoder.hpp"

SISODecoder::SISODecoder(int num_bits)
{
    this->num_bits = num_bits;
    branches = new double[num_bits][4][2]();
    forward  = new double[num_bits+1][4];
    backward  = new double[num_bits+1][4];
    loglikeli = new double[num_bits];

    for (int i=0; i<num_bits; i++){
        forward [i][0] = 0;
        backward[i][0] = 0;
        forward [i][1] = std::numeric_limits<double>::min();
        backward[i][1] = std::numeric_limits<double>::min();
        forward [i][2] = std::numeric_limits<double>::min();
        backward[i][2] = std::numeric_limits<double>::min();
        forward [i][3] = std::numeric_limits<double>::min();
        backward[i][3] = std::numeric_limits<double>::min();
    }
}

SISODecoder::~SISODecoder()
{
    delete branches;
    delete forward;
    delete backward;
    delete loglikeli;
}

char SISODecoder::process(const double* vector_in_1, const double* vector_in_2, const double* vector_in_3)
{
    for (int i=0; i<num_bits; i++){
        // Branches / paths
        // 00
        branches[i][0][0] = -1 * vector_in_1[i] + -1 * vector_in_2[i] + -1 * vector_in_3[i];
        branches[i][0][1] =  1 * vector_in_1[i] +  1 * vector_in_2[i] +  1 * vector_in_3[i];

        // 01
        branches[i][1][0] = -1 * vector_in_1[i] +  1 * vector_in_2[i] + -1 * vector_in_3[i];
        branches[i][1][1] =  1 * vector_in_1[i] + -1 * vector_in_2[i] +  1 * vector_in_3[i];

        // 10
        branches[i][2][0] = -1 * vector_in_1[i] + -1 * vector_in_2[i] + -1 * vector_in_3[i];
        branches[i][2][1] =  1 * vector_in_1[i] +  1 * vector_in_2[i] +  1 * vector_in_3[i];

        // 11
        branches[i][3][0] = -1 * vector_in_1[i] +  1 * vector_in_2[i] + -1 * vector_in_3[i];
        branches[i][3][1] =  1 * vector_in_1[i] + -1 * vector_in_2[i] +  1 * vector_in_3[i];
    }

    for (int i=0; i<num_bits; i++){
        // Forwards / Backwards
        // 00
        forward[i+1][0] = std::max(forward[i][0]+branches[i][0][0], forward[i][1]+branches[i][1][1]);
        backward[i+1][0] = std::max(backward[i][0]+branches[num_bits-1-i][0][0], backward[i][2]+branches[num_bits-1-i][0][1]);

        // 01
        forward[i+1][1] = std::max(forward[i][2]+branches[i][2][0], forward[i][3]+branches[i][3][1]);
        backward[i+1][1] = std::max(backward[i][0]+branches[num_bits-1-i][1][1], backward[i][2]+branches[num_bits-1-i][1][0]);

        // 10
        forward[i+1][2] = std::max(forward[i][0]+branches[i][0][1], forward[i][1]+branches[i][1][0]);
        backward[i+1][2] = std::max(backward[i][1]+branches[num_bits-1-i][2][0], backward[i][3]+branches[num_bits-1-i][2][1]);

        // 11
        forward[i+1][3] = std::max(forward[i][2]+branches[i][2][1], forward[i][3]+branches[i][3][0]);
        backward[i+1][3] = std::max(backward[i][1]+branches[num_bits-1-i][3][1], backward[i][3]+branches[num_bits-1-i][3][0]);
    }

    for (int i=0; i<num_bits; i++){
        // LLR
        
        std::vector<double> positive;
        std::vector<double> negative;

        // 0 -(0)> 0 = (0)
        negative.push_back(forward[i][0] + branches[i][0][0] + backward[num_bits-1-i][0]);
        // 0 -(1)> 2 = (1)
        positive.push_back(forward[i][0] + branches[i][0][1] + backward[num_bits-1-i][2]);
        // 1 -(0)> 2 = (1)
        negative.push_back(forward[i][1] + branches[i][1][0] + backward[num_bits-1-i][2]);
        // 1 -(1)> 0 = (0)
        positive.push_back(forward[i][1] + branches[i][1][1] + backward[num_bits-1-i][0]);
        // 2 -(0)> 1 = (0)
        negative.push_back(forward[i][2] + branches[i][2][0] + backward[num_bits-1-i][1]);
        // 2 -(1)> 3 = (1)
        positive.push_back(forward[i][2] + branches[i][2][1] + backward[num_bits-1-i][3]);
        // 3 -(0)> 3 = (1)
        negative.push_back(forward[i][3] + branches[i][3][0] + backward[num_bits-1-i][3]);
        // 3 -(1)> 1 = (0)
        positive.push_back(forward[i][3] + branches[i][3][1] + backward[num_bits-1-i][1]);

        loglikeli[i] = *std::max_element(positive.begin(), positive.end()) - *std::max_element(negative.begin(), negative.end());
    }

    return 0;
}
