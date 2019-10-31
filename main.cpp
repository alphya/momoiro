#include <iostream>
#include "momoiro.hpp"

int main()
{
   using namespace momoiro;
   // [n*n | n <- [1..]] を表現するコルーチン
   coro_t::pull_type inf_list(
      [](coro_t::push_type& sink)
      {
         auto i = 1;
         while (true) sink(i * i), i++;
      });

   // 無限の要素数をもつ配列 inf_list の、最初から 10 番目の要素の合計 
   std::cout << (head <<= taked(2) <<= droped(9) <<= inf_list );
}