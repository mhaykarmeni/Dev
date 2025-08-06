#include <string>
#include <istream>
#include <sstream>
#include <cstdlib>
#include <fstream>

#include "BookOrder.h"

#include <stdlib.h>
#include <chrono>

template<class MapContBuy, class MapContSell>
class Extractor {
    struct LineParser {
        BookOrder process(const std::string& line) {
            if(line.empty()) [[unlikely]] {
                std::cerr << "Invalid input. Exiting.\n";
                return BookOrder{};//invalid order
            }
            std::istringstream iss(line);
            unsigned trId{}; char side{}; unsigned quantity{}; unsigned price{};
            iss >> trId >> side >>  quantity >> price;
            BookOrder tmp{trId, quantity, price, side};
            if(!tmp.isValid()) {
                if(m_dbgMode) {
                std::cerr << "Invalid order. Dumping the order(id, side, quantity, price): " << trId <<" "<< side <<" "<< quantity<<" "<< price<<'\n';
                }
            }
            return tmp;
        }
        void setDbgMode(const bool flag) noexcept { m_dbgMode = flag; }
private:
        bool m_dbgMode = false;
    } m_lineParser;
public:
    void process(std::ifstream& input) {
        std::string currLine;
        std::chrono::nanoseconds total_time{};
        while(std::getline(input, currLine)) {
            BookOrder currOrder = m_lineParser.process(currLine);
            using clock = std::chrono::high_resolution_clock;
            auto start = clock::now();
            m_orderPool.tryExecute(currOrder);
            auto end = clock::now();
            auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            total_time += elapsed_ns;
        }
        std::cout << "Orders' total processed time(ns): " << total_time.count()<<std::endl;
    }

    constexpr Extractor() = default;
    explicit Extractor(bool dbgMode) :
        m_orderPool{}
    { 
        m_lineParser.setDbgMode(dbgMode); 
    }

private:
    OrderPool<MapContBuy, MapContSell>          m_orderPool;
};
