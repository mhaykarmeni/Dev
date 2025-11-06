#include <map>
#include <queue>
#include <flat_map>
#include <absl/container/btree_map.h>

#include "ExtractUtils.h"
#include "Logger.h"

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

void addCurrentDateTimeIntoLog(Common::Logger* p_logger) {
    std::string tmpStr{};
    std::string* dateTimeStr = &tmpStr;
    Common::getCurrentTimeStr(dateTimeStr);
    p_logger->log("%\n", *dateTimeStr);
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <number_of_orders> <std_map|btree_map> <debug mode 0|1> <generate input file 0|1>\n";
        return 1;
    }
    Common::Logger& logger = Common::Logger::getInstance();
    logger.log("Trade Matching Engine program launched at ");
    addCurrentDateTimeIntoLog(&logger);
    const unsigned numOrders = std::atoi(argv[1]);
    const bool isGenerationNeeded = std::atoi(argv[4]);
    if(isGenerationNeeded) {
        logger.log("Enabling auto generation of orders for % entries.\n", numOrders);
        generateInputFile("tme_input.txt", numOrders);
    }
    else {
        std::ifstream ifstr("tme_input.txt");
        if (!ifstr.good()) {
            logger.log("Error: 'tme_input.txt' not found. Nothing to load. Exiting.\n");
            return 1;
        } else {
            logger.log("Found 'tme_input.txt', proceeding...\n");
        }
    }
    std::string containerType = argv[2];
    const bool isDbgMode = std::atoi(argv[3]);

    if (containerType == "std_map" || containerType.empty()) {
        logger.log("std::map is selected for internal representations of main order pool conatiners.\n");
        logger.log("Debug mode: %\n", isDbgMode);
        std::ifstream ifstr("tme_input.txt");
        Extractor< std::map<unsigned, std::queue<BookOrder>, std::greater<unsigned>>, std::map<unsigned, std::queue<BookOrder>> > extractor(isDbgMode);
        extractor.process(ifstr);
    } else if (containerType == "btree_map") {
        logger.log("btree_map is selected for internal representations of main order pool containers.\n");
        logger.log("Debug mode: %\n", isDbgMode);
        std::ifstream ifstr("tme_input.txt");
        Extractor< absl::btree_map<unsigned, std::queue<BookOrder>, std::greater<unsigned>>, absl::btree_map<unsigned, std::queue<BookOrder>> > extractor(isDbgMode);
        extractor.process(ifstr);
    }
      else if (containerType == "std::flat_map") {
        logger.log("std::flat_map is selected for internal representations of main order pool containers.\n");
        logger.log("Debug mode: %\n", isDbgMode);
        std::ifstream ifstr("tme_input.txt");
        Extractor< std::flat_map<unsigned, std::queue<BookOrder>, std::greater<unsigned>>, absl::btree_map<unsigned, std::queue<BookOrder>> > extractor(isDbgMode);
        extractor.process(ifstr);
    }
      else {
        std::cerr << "Unknown map type: " << containerType << "\n";
        return 2;
    }
}
