#include <iostream>
#include "momoiro.hpp"

int main()
{
   using namespace momoiro;
   // [n*n | n <- [1..]] ��\������R���[�`��
   coro_t::pull_type inf_list(
      [](coro_t::push_type& sink)
      {
         auto i = 1;
         while (true) sink(i * i), i++;
      });

   // �����̗v�f�������z�� inf_list �́A�ŏ����� 10 �Ԗڂ̗v�f�̍��v 
   std::cout << (head <<= taked(2) <<= droped(9) <<= inf_list );
}