#include <fstream>
#include <iomanip>
#include <unordered_map>
#include <map>
#include <set>
#include <string_view>

namespace wf
{

class WordCounter
{
public:
    using Container = std::unordered_map<std::string, uint32_t>;
    using GroupByFreq = std::map<uint32_t, std::set<std::string_view>, std::greater<uint32_t>>;

public:
    WordCounter()
    {
        m_counter.rehash(1500000);
    }

    template<typename T>
    void push(T&& word)
    {
        if (auto [it, inserted] = m_counter.try_emplace(std::forward<T>(word), 1); !inserted) {
            ++it->second;
            std::string().swap(word);
        }
    }

    auto group_by_freq() const
    {
        auto word_freq = GroupByFreq();
        for (const auto& [word, count] : m_counter) {
            auto it = word_freq.try_emplace(count).first;
            it->second.emplace(static_cast<std::string_view>(word));
        }
        return word_freq;
    }

    const Container& words() const
    {
        return m_counter;
    }

private:
    Container m_counter;
};

WordCounter count_words(std::wistream& in)
{
    WordCounter word_counter;
    std::string word;
    for (auto c = wchar_t(); in >> c; ) {
        if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') {
            word.push_back(std::tolower(c));
        } else if (!word.empty()) {
            word_counter.push(std::move(word));
        }
    }
    if (!word.empty()) {
        word_counter.push(std::move(word));
    }
    return word_counter;
}

}