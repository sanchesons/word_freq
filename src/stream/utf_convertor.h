#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>
#include <tuple>

using char8_t   = unsigned char;

enum State : uint8_t
{
    BGN = 0,    //- Start
    ERR = 12,   //- Invalid sequence
    CON = 13,   //- Continue sequence, when reach end of buffer
                //
    CS1 = 24,   //- Continuation state 1
    CS2 = 36,   //- Continuation state 2
    CS3 = 48,   //- Continuation state 3
                //
    P3A = 60,   //- Partial 3-byte sequence state A
    P3B = 72,   //- Partial 3-byte sequence state B
                //
    P4A = 84,   //- Partial 4-byte sequence state A
    P4B = 96,   //- Partial 4-byte sequence state B
                //
    END = BGN,  //- Start and End are the same state!
    err = ERR,  //- For readability in the state transition table
};

enum CharClass : uint8_t
{
    ILL = 0,    //- C0..C1, F5..FF  ILLEGAL octets that should never appear in a UTF-8 sequence
                //
    ASC = 1,    //- 00..7F          ASCII leading byte range
                //
    CR1 = 2,    //- 80..8F          Continuation range 1
    CR2 = 3,    //- 90..9F          Continuation range 2
    CR3 = 4,    //- A0..BF          Continuation range 3
                //
    L2A = 5,    //- C2..DF          Leading byte range A / 2-byte sequence
                //
    L3A = 6,    //- E0              Leading byte range A / 3-byte sequence
    L3B = 7,    //- E1..EC, EE..EF  Leading byte range B / 3-byte sequence
    L3C = 8,    //- ED              Leading byte range C / 3-byte sequence
                //
    L4A = 9,    //- F0              Leading byte range A / 4-byte sequence
    L4B = 10,   //- F1..F3          Leading byte range B / 4-byte sequence
    L4C = 11,   //- F4              Leading byte range C / 4-byte sequence
};

struct FirstUnitInfo
{
    char8_t  mFirstOctet;
    State    mNextState;
};

struct alignas(2048) LookupTables
{
    FirstUnitInfo   maFirstUnitTable[256];
    CharClass       maOctetCategory[256];
    State           maTransitions[108];
    std::uint8_t    maFirstOctetMask[16];
};

class UtfConvertor
{
private:
    static const LookupTables smTables;

public:
    // assume that sizeof(dst) >= 4*sizeof(src), then we don't need check end of dst
    std::tuple<const char8_t*, char32_t*, int> convert(const char8_t* src, const char8_t* src_end, char32_t* dst)
    {
        while (src < src_end) {
            if (*src < 0x80) {
                *dst++ = *src++;
            } else {
                switch (auto [state, next, cdpt] = advance(src, src_end); state)
                {
                default:
                    *dst++ = cdpt;
                    src = next;
                    break;
                case CON:
                    return {src, dst, 0};
                case ERR:
                    return {src, dst, 1};
                }
            }
        }
        return {src, dst, 0};
    }

private:
    std::tuple<State, const char8_t*, char32_t> advance(const char8_t* src, const char8_t* src_end)
    {
        FirstUnitInfo info = smTables.maFirstUnitTable[*src++];     //- The descriptor for the first code unit
        char32_t cdpt = info.mFirstOctet;                           //- The current code unit's character class, get the initial code point value
        State state = info.mNextState;                              //- The current DFA state, get the second state

        while (state > ERR)
        {
            if (src < src_end) {
                auto unit = char32_t(*src);                     //- Cache the current code unit
                cdpt = (cdpt << 6) | (unit & 0x3F);             //- Adjust code point with continuation bits
                auto type = smTables.maOctetCategory[unit];     //- Look up the code unit's character class
                state = smTables.maTransitions[state + type];   //- Look up the next state
                ++src;
            } else {
                return {CON, src, 0};
            }
        }
        return {state, src, cdpt};
    }
};