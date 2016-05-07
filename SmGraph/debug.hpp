#pragma once
#include "base_header.h"
#include <Windows.h>
using namespace std;
namespace debug
{
    class memory_bomb
    {
    public:
        enum class MEM_UNIT { Bytes, Kb, Mb };
        memory_bomb(std::ostream& out = std::cerr, MEM_UNIT mem_type = MEM_UNIT::Mb)
            :m_out(out),
            mtype(mem_type){
            _CrtMemCheckpoint(&s1);
        }
        void leakCheck()
        {
            _CrtMemCheckpoint(&s2);
            _CrtMemState s3;
            if (_CrtMemDifference(&s3, &s1, &s2))
            {
                _CrtMemDumpStatistics(&s3);
                m_out << "error: memory explosion: ";
                if (mtype == MEM_UNIT::Mb)
                    m_out << s3.lTotalCount / 1024 / 1024 << " Mb\n";
                else if (mtype == MEM_UNIT::Kb)
                    m_out << s3.lTotalCount / 1024 << "Kb\n";
                else
                    m_out << s3.lTotalCount << " Bytes\n";
            }
        }
        ~memory_bomb()
        {
            leakCheck();
        }
    private:
        _CrtMemState s1;
        _CrtMemState s2;
        std::ostream& m_out;
        MEM_UNIT mtype;
    };

    class time_bomb
    {
    public:
        time_bomb(std::ostream& out = std::cerr) :m_out(out)
        {
            birth = clock();
        };
        ~time_bomb()
        {
            m_out << (double)(clock() - birth) / CLOCKS_PER_SEC;
        }
    private:
        time_t birth;
        std::ostream& m_out;
    };

    template<class T>
    void print(const T& items, char del = ' ')
    {
        auto it = items.begin();
        while (it != items.end())
        {
            if (it != items.begin())
                std::cout << ' ';
            std::cout << *it;
            it++;
        }
    }
}