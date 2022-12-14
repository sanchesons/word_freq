#include <fstream>
#include <iomanip>
#include <unordered_map>
#include <map>
#include <set>
#include <string_view>

namespace wf
{

using CounterType = uint32_t;
using CounterComparatorType = std::greater<CounterType>;

using GroupByFreqView = std::map<CounterType, std::set<std::string_view>, CounterComparatorType>;

class WordCounter
{
public:
    using Container = std::unordered_map<std::string, CounterType>;

private:
    Container m_counter;
    static const size_t APPROXIMATE_NUM_OF_WORDS = 10500000;

public:
    WordCounter(size_t approximate_num_of_words = APPROXIMATE_NUM_OF_WORDS)
    {
        m_counter.rehash(approximate_num_of_words);
    }

    void push(std::string&& word)
    {
        if (auto [it, inserted] = m_counter.try_emplace(std::move(word), 1); !inserted) {
            ++it->second;
        }
    }

    auto group_by_freq() const
    {
        auto word_freq = GroupByFreqView();
        for (const auto& [word, count] : m_counter) {
            auto it = word_freq.try_emplace(count).first;
            it->second.emplace(word);
        }
        return word_freq;
    }

    size_t size() const 
    {
        return m_counter.size();
    }

    size_t empty() const 
    {
        return m_counter.empty();
    }
};

}