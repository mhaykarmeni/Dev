#include <map>
#include <queue>
#include <absl/container/btree_map.h>

#include "ExtractUtils.h"

//Random orders' generator
void generateInputFile(const char* fileName, unsigned ordersCount) {
    std::ofstream ofstr(fileName);
    srand(time(NULL));
    for(unsigned i = 0; i < ordersCount; ++i) {
        unsigned randNumber = rand();
        unsigned traderId = randNumber % 97 + 4;//up to 100
        unsigned price = randNumber % 91 + 9;//up to 99
        unsigned quantity = randNumber % 89 + 16;//up to 104
        char side = (randNumber % 2) ? 'B' : 'S';
        
        ofstr << traderId<<" " << side<<" " << quantity<<" " << price <<'\n';
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <number_of_orders> <std_map|btree_map> <debug mode 0|1> <generate input file 0|1>\n";
        return 1;
    }
    const unsigned numOrders = std::atoi(argv[1]);
    const bool isGenerationNeeded = argv[4];
    if(isGenerationNeeded) {
        generateInputFile("input.txt", numOrders);
    }
    std::string containerType = argv[2];
    const bool isDbgMode = argv[3];

    if (containerType == "std_map" || containerType.empty()) {
        std::ifstream ifstr("input.txt");
        Extractor< std::map<unsigned, std::queue<BookOrder>, std::greater<unsigned>>, std::map<unsigned, std::queue<BookOrder>> > extractor(isDbgMode);
        extractor.process(ifstr);
    } else if (containerType == "btree_map") {
        std::ifstream ifstr("input.txt");
        Extractor< absl::btree_map<unsigned, std::queue<BookOrder>, std::greater<unsigned>>, absl::btree_map<unsigned, std::queue<BookOrder>> > extractor(isDbgMode);
        extractor.process(ifstr);
    } else {
        std::cerr << "Unknown map type: " << containerType << "\n";
        return 2;
    }
}
