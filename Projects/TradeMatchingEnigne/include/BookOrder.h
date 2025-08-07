#include <iostream>
#include <variant>
#include <cmath>
#include <utility>
#include <functional>


class alignas(16) BookOrder {
private:
        unsigned               m_traderId;
	unsigned               m_quantity;
	unsigned               m_price;
        char                   m_side;
	bool                   m_isValid;
public:
        constexpr BookOrder() noexcept :
     		m_traderId{},
            	m_quantity{},
            	m_price{},
            	m_side{},
            	m_isValid{false}
        {}
	constexpr explicit BookOrder(unsigned trId, unsigned quantity, unsigned price, char side) noexcept :
		m_traderId{trId},
            	m_quantity{quantity},
		m_price{price},
            	m_side{side},
            	m_isValid{(m_traderId > 0) && (m_quantity > 0) && (m_price > 0) && (m_side == 'B' || m_side == 'S')}
	{}
	BookOrder(const BookOrder&) = default;
	BookOrder& operator=(const BookOrder&) = delete;
	[[nodiscard]] constexpr unsigned getId() const noexcept { return m_traderId; }
	[[nodiscard]] constexpr unsigned getQuantity() const noexcept { return m_quantity; }
	[[nodiscard]] constexpr unsigned getPrice() const noexcept { return m_price; }
       	[[nodiscard]] constexpr char     getSide() const noexcept { return m_side; }
	void setQuantity(unsigned val) noexcept { m_quantity = val; }
	void setPrice(unsigned val) noexcept { m_price = val; }
        constexpr bool isValid() const noexcept { return m_isValid; }
        void print() const {std::cout << "TraderId: " << m_traderId << " Quantity: " << m_quantity << " Price: " << m_price << " Side: " << m_side <<'\n';}
};

template <class MapContBuy, class MapContSell>
class OrderPool {
    using buyContIterator =     typename MapContBuy::iterator;
    using sellContIterator =    typename MapContSell::iterator;
    MapContBuy                         m_buyOrders;
    MapContSell                        m_sellOrders;
public:
    OrderPool() = default;
private:
    void dumpOrders() const {
        std::cout << "Dumping Buy orders..."<<std::endl;
        for(const auto& elems : m_buyOrders) {
            auto cpy = elems.second;
            while( !cpy.empty()) {
                cpy.front().print();
                cpy.pop();
            }
        }
        std::cout << std::endl<<"Dumping Sell orders..."<<std::endl;
        for(const auto& elems : m_sellOrders) {
            auto cpy = elems.second; 
            while( !cpy.empty()) {
                cpy.front().print();
                cpy.pop();
            }
        }
        std::cout <<std::endl;    
    }
    void addOrder(const BookOrder& order) {
        if (order.getSide() == 'S') {
            m_sellOrders[order.getPrice()].push(order);
        }
        else {
            m_buyOrders[order.getPrice()].push(order);
        }          
    }
    void dumpExecutionMessage(const BookOrder& resting, const BookOrder& aggressor) const {
        const unsigned buyerId = (resting.getSide() == 'B') ? resting.getId() : aggressor.getId();
        const unsigned sellerId = (resting.getSide() == 'B') ? aggressor.getId() : resting.getId();
        const unsigned dealQuantity = std::min(resting.getQuantity(), aggressor.getQuantity());
        std::cout << "T" << buyerId << "+" << dealQuantity << "@" << aggressor.getPrice() 
                  << " T" << sellerId << "-" << dealQuantity << "@" << aggressor.getPrice() << std::endl;
    }
    template<class OrderTypeMap>
    bool updateAll(OrderTypeMap& cont, OrderTypeMap::iterator& it, BookOrder& order) {
        const bool mutuallyComplete = it->second.front().getQuantity() == order.getQuantity();
        bool isFinalUpdate = false;
        if(mutuallyComplete) [[unlikely]] {
            it->second.pop();
            if(it->second.empty()) {
                it = cont.erase(it);
            }
            isFinalUpdate = true;
        }
        else {
            const bool isOrderComplete = (order.getQuantity() <= it->second.front().getQuantity());
            if(isOrderComplete) {
                const int orderQuantity = static_cast<int>(order.getQuantity());
                const int remainedQuantity = static_cast<int>(it->second.front().getQuantity()) - orderQuantity;
                it->second.front().setQuantity(remainedQuantity);
                isFinalUpdate = true;
            }
            else {
                const int orderInPoolQuantity = static_cast<int>(it->second.front().getQuantity());
                const int remainedQuantity = static_cast<int>(order.getQuantity()) - orderInPoolQuantity;
                it->second.pop();
                if(it->second.empty()) {
                    it = cont.erase(it);
                }
                order.setQuantity(remainedQuantity);
                isFinalUpdate = false;//still need to be processed
            }
        }
        return isFinalUpdate;
    }
public:
    void tryExecute(BookOrder& order) {
        if(order.isValid()) [[likely]] {
            const char orderSide = order.getSide();
            std::variant<std::reference_wrapper<MapContBuy>, std::reference_wrapper<MapContSell>> curOrderMap = (order.getSide() == 'S') ? 
                                                                                                                std::variant<std::reference_wrapper<MapContBuy>, std::reference_wrapper<MapContSell>>(std::ref(m_buyOrders)) : 
                                                                                                                std::variant<std::reference_wrapper<MapContBuy>, std::reference_wrapper<MapContSell>>(std::ref(m_sellOrders));
            const bool isStorableOrder = std::visit([&](auto&& contRef) -> bool {
                                auto&& cont = contRef.get();
                                if(cont.empty()) [[unlikely]] {
                                    return true;
                                }                        
                                const unsigned curOrderMapPrice = cont.begin()->second.front().getPrice();
                                if (orderSide == 'S') {
                                    return curOrderMapPrice < order.getPrice();
                                } else {
                                    return curOrderMapPrice > order.getPrice();
                                }
                             }, curOrderMap);
            if(isStorableOrder) {//if comes order which is not matched with any resting order, so add it into orders pool
                addOrder(order);
                return;
            }
            executeOrder(curOrderMap, order);
        } 
    }
    

    void executeOrder(std::variant<std::reference_wrapper<MapContBuy>, std::reference_wrapper<MapContSell>>& cont, BookOrder& order) {
        auto comparator = (order.getSide() == 'S') ?    [](const unsigned priceInCont, const unsigned currPrice) { return priceInCont >= currPrice; } : 
                                                        [](const unsigned priceInCont, const unsigned currPrice) { return priceInCont <= currPrice; };

        std::visit([&](auto&& mapRef) {
            auto& cont = mapRef.get();  // Extract actual container
            auto it = cont.begin();
            bool isFinalUpdate = false;
            while (it != cont.end() && !isFinalUpdate) {
                const unsigned currContPrice = it->second.front().getPrice();
                const unsigned orderPrice = order.getPrice();

                if (comparator(currContPrice, orderPrice)) {
                    dumpExecutionMessage(it->second.front(), order);
                    isFinalUpdate = updateAll(cont, it, order);
                } else {
                    addOrder(order);
                    break; // Exit loop after adding order
                }
            }
        }, cont);
    }

};
