#include <random>
#include <vector>
#include <unordered_set>
namespace random
{
    int Int(int min_v, int max_v)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(min_v, max_v);
        return dis(gen);
    }
    double Double(double min_v, double max_v)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(min_v, max_v);
        return dis(gen);
    }
    std::vector<int> UniqueIntSet(int n, int min_v, int max_v)
    {
        std::unordered_set<int> exist;
        for (int i = 0; i < n; ++i)
        {
            int v = random::Int(min_v, max_v);
            exist.emplace(v);
        }
        return vector<int>(exist.begin(), exist.end());
    }
}