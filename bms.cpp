#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>

#include "bms.hpp"
#include "RSCCoder.hpp"
#include "SISODecoder.hpp"

int main(int argc, char* argv[])
{
    int mode = 0;
    std::vector<int> interleaver;
    
    //////////////////////////////////////////////////////////////
    // Parse arguments
    for (int i = 0; i<argc; i++)
    {
        ////////////////
        // ENCODE
        if (!strcmp(argv[i],"-e") || !strcmp(argv[i],"--encode")){
            if (mode == 0){
                mode = 'e';
            } else {
                std::cerr << "Only one of --encode / --decode arguments is allowed.\n";
                return 99;
            }
        }

        ////////////////
        // DECODE
        else if (!strcmp(argv[i],"-d") || !strcmp(argv[i],"--decode")) {
            if (mode == 0){
                mode = 'd';
            } else {
                std::cerr << "Only one of --encode / --decode arguments is allowed.\n";
                return 99;
            }
        }

        ////////////////
        // INTERLEAVER
        else if (!strcmp(argv[i],"-i") || !strcmp(argv[i],"--interleaver")) {
            i++;
            if (argc > i){
                std::stringstream ss(argv[i]);
                for (int k; ss >> k;){ // parse string to int until some err bit
                    interleaver.push_back(k);
                    if(ss.peek()==','){ // explode around ','
                       ss.ignore();
                    }
                }
                if (!ss.eof()){ // check what err bit
                    std::cerr << "Argument '"<< argv[i-1] << "' expects a VALID interleaver scheme.";
                    return 99;
                }
            } else {
                std::cerr << "Argument '"<< argv[i-1] << "' expects an interleaver scheme.";
                return 99;
            }
        }

        ////////////////
        // HELP
        else if (!strcmp(argv[i],"-h") || !strcmp(argv[i],"--help")) {
            std::cerr << "A program I suppose...\n"; //TODO: add help argument info
            return 0;
        }
    }

    if (mode == 0){
        std::cerr << "One of --encode / --decode arguments is required.\n";
        return 99;
    }
    if (interleaver.size() == 0){
        std::cerr << "Argument --interleaver is required\n";
    }
    // <end> parse arguments
    //////////////////////////////////////////////////////////////


    /*******************************************************/
    /**                    ENCODE MODE                    **/
    /*******************************************************/
    if (mode == 'e'){
        auto coder1 = RSCCoder();
        auto coder2 = RSCCoder();
        
        int size = interleaver.size();
        auto input = new char[size];
        std::cin.read(input, size);
        if (std::cin.eof()){
            std::cerr<< "Not enough bits on input\n";
            return 1;
        }

        // follow trellis and code input
        for (int i = 0; i<size; i++){
            std::cout<< input[i];
            std::cout<< coder1.next(input[i]);
            std::cout<< coder2.next(input[interleaver[i]]);
        }
        // Code RSC registers and append them
        const char* state1 = coder1.getStateCodetxt();
        const char* state2 = coder2.getStateCodetxt();
        std::cout<< state1[0] << state1[0] << state2[0] << state1[1] << state1[1] << state2[1] <<std::endl;

        delete input;
    }

    /*******************************************************/
    /**                    DECODE MODE                    **/
    /*******************************************************/
    else if (mode == 'd'){
        const int ntail = 2; // bit delka registru

        std::vector<double> input[3];
        double input_temp;

        int size = interleaver.size();
        for (int i = 0; i<3*(size+ntail); i++){
            if(std::cin >> input_temp){
                if(std::cin.peek()==','){ // explode around ','
                    std::cin.ignore();
                }
                // input[i%3].push_back((char)(input_temp > 0));
                input[i%3].push_back(input_temp); // split input at stride 3
            } else {
                std::cerr<< "Not enough data on input\n";
                return 1;
            }
        }

        auto decoder1 = SISODecoder(size + ntail);
        auto decoder2 = SISODecoder(size + ntail);
        auto loglikeli          = new double[size + ntail]();
        auto llr_interleaved    = new double[size + ntail]();
        auto input_interleaved  = new double[size + ntail]();


        // ITERATE FOR ACCURACY
        for (char iter=0; iter<16; iter++)
        {
            // Decoder 1
            decoder1.process(input[0].data(), input[1].data(), loglikeli);
            auto LLR1 = decoder1.getLLR();
            for (int i=0; i<size+ntail; i++){
                LLR1[i] = LLR1[i] - loglikeli[i] - 2*input[0][i];
            }
            for (int i=0; i<size; i++){ //interleave
                llr_interleaved[i] = LLR1[interleaver[i]];
                input_interleaved[i] = input[0][interleaver[i]];
            }

            // Decoder 2
            decoder2.process(input_interleaved, input[2].data(), llr_interleaved);
            auto LLR2 = decoder2.getLLR();
            for (int i=0; i<size+ntail; i++){
                LLR2[i] = LLR2[i] - llr_interleaved[i] - 2*input_interleaved[i];
            }
            for (int i=0; i<size; i++){ //deinterleave
                loglikeli[interleaver[i]] = LLR2[i];
            }

            // end iteration early
            int count = 0;
            for (int i=0; i<size+ntail; i++){
                count += (int)((LLR1[i] > 0) == (loglikeli[i] > 0));
            }
            if (count == size+ntail){
                break;
            }
        }

        //output
        for (int i=0; i<size; i++){
            std::cout<<(int)(loglikeli[i] > 0);
        }
        std::cout<<std::endl;

        delete loglikeli;
        delete llr_interleaved;
        delete input_interleaved;
    }
    /*******************************************************/
    return 0;
}
