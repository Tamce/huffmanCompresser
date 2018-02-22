#include <iostream>
#include <set>
#include <map>
#include <cstring>
#include <vector>
#ifdef DEBUG
#define log(x) std::cout << x << std::endl
#else
#define log(x)
#endif
namespace tmc
{
    struct HuffmanNode
    {
        HuffmanNode *parent;
        size_t weight;
        bool tag;
        HuffmanNode(HuffmanNode *p, size_t w, bool t):parent(p),weight(w),tag(t) {}
    };
    struct HuffmanCmp
    {
        bool operator()(HuffmanNode *t, HuffmanNode *o) const
        {
            return t->weight < o->weight;
        }
    };

    class Compresser
    {
    public:
        // 用于内存回收
        static std::vector<HuffmanNode *> pool;
        static void clean()
        {
            for (auto i : pool)
                delete i;
            dirty = false;
        }
        static bool dirty;
        static std::map<char, size_t> stat(std::istream &in)
        {
            // 统计词频
            std::map<char, size_t> dict;
            auto n = 10;
            while (n-->0)
            {
                static char c;
                c = in.get();
                if (!in) break;
                ++dict[c];
            }
            log("---stat done---");
            return dict;
        }
        static void buildHuffman(std::map<char, size_t> &dict, HuffmanNode *code[256])
        {
            if (dirty)
                clean();
            std::memset(code, 0, sizeof(HuffmanNode *) * 256);
            std::multiset<HuffmanNode *, HuffmanCmp> tree;
            // 预处理，生成森林
            for (auto i : dict)
            {
                log("map: " << i.first << " - " << i.second);
                code[i.first] = new HuffmanNode(nullptr, i.second, 0);
                tree.insert(code[i.first]);
                pool.push_back(code[i.first]);
            }

            // 合并森林构造哈夫曼树
            while (tree.size() > 1)
            {
                auto n1 = *tree.begin();
                tree.erase(tree.begin());
                auto n2 = *tree.begin();
                tree.erase(tree.begin());
                auto node = new HuffmanNode(nullptr, n1->weight + n2->weight, 0);
                n1->parent = n2->parent = node;
                log("merge: " << n1->weight << ", " << n2->weight);
                n1->tag = 0;
                n2->tag = 1;
                tree.insert(node);
                pool.push_back(node);
            }
            dirty = true;
            log("---build done---");
        }
        static void printHuffmanCodes(std::ostream &out, std::vector<bool> code[256])
        {
            // 输出哈夫曼树
            for (size_t i = 0; i < 256; ++i)
            {
                if (code[i].size() == 0) continue;
                out << i << ": ";
                for (auto j : code[i])
                    out << (int)j;
                out << std::endl;
            }
        }
        static std::vector<bool> getHuffmanCode(HuffmanNode *p)
        {
            std::vector<bool> code;
            if (p == nullptr) return code;
            while(p->parent != nullptr)
            {
                if (p->tag) code.insert(code.begin(), true);
                else code.insert(code.begin(), false);
                p = p->parent;
            }
            return code;
        }
        static void getHuffmanCodes(HuffmanNode *code[256], std::vector<bool> huffman[256])
        {
            for (int i = 0; i < 256; ++i)
                huffman[i] = getHuffmanCode(code[i]);
        }
        static void compress(std::istream &in, std::ostream &out, std::vector<bool> huffman[256])
        {
            // todo
            printHuffmanCodes(out, huffman);
        }
        static void compress(std::istream &in, std::ostream &out)
        {
            HuffmanNode *code[256];
            std::vector<bool> huffman[256];
            auto dict = stat(in);
            buildHuffman(dict, code);
            getHuffmanCodes(code, huffman);
            compress(in, out, huffman);
        }
        //static void decompress(istream &, ostream &);
    };
    bool Compresser::dirty = false;
    std::vector<HuffmanNode *> Compresser::pool;
}