#include <boost/coroutine2/coroutine.hpp>
#include <numeric>
#include <concepts>

namespace momoiro
{

using coro_t = boost::coroutines2::coroutine<int>;
using list_t = coro_t::pull_type;
using return_t = coro_t::push_type;

// �z��(�x���]��)�̐擪����Anum �̗v�f�����o�����z���Ԃ�
template<typename T>
requires std::same_as<list_t, std::remove_cvref_t<T>>
auto take(int num, T&& list) -> list_t
{
   return {
      [itr = boost::coroutines2::detail::begin(list), num] (auto&& ret) mutable
      requires std::same_as<return_t, std::remove_cvref_t<decltype(ret)>> {
         while (num --> 0) ret(*itr), itr++;
      }
   };
}

// �z��(�x���]��)�̐擪����Anum �̗v�f�����o�����z������֐���Ԃ�
// ���\�b�h�`�F�[���p
auto taked(int num)
{
   // �z�� list �́A�擪���� num �̗v�f�����o�����z���Ԃ��֐�
   return [num](auto&& list) mutable -> list_t
      requires std::same_as<return_t, std::remove_cvref_t<decltype(ret)>> {
      // �z�� list �̃C�e���[�^�C���N�������g���Ȃ���A���̗v�f��Ԃ��R���[�`��(�x���]���̔z���\��)
      return {
         [itr = boost::coroutines2::detail::begin(list), num] (auto&& ret) mutable
         requires std::same_as<return_t, std::remove_cvref_t<decltype(ret)>> {
            while (num --> 0) ret(*itr), itr++;
         }
      };
   };
}

// �z��(�x���]��)�̐擪����Anum �̗v�f�����������z���Ԃ�
template<typename T>
requires std::same_as<list_t, std::remove_cvref_t<T>>
auto drop(int num, T&& list) -> list_t
{
   return {
      [itr = boost::coroutines2::detail::begin(list), num] (auto&& ret) mutable
      requires std::same_as<return_t, std::remove_cvref_t<decltype(ret)>> {
         while (num-- > 0) itr++;
         while (true) ret(*itr), itr++;
      }
   };
}

// �z��(�x���]��)�̐擪����Anum �̗v�f�����������z������֐���Ԃ�
// ���\�b�h�`�F�[���p
auto droped(int num)
{
   // �z�� list �́A�擪���� num �̗v�f�����o�����z���Ԃ��֐�
   return [num](auto&& list) mutable -> list_t
      requires std::same_as<return_t, std::remove_cvref_t<decltype(ret)>> {
      // �z�� list �̃C�e���[�^�C���N�������g���Ȃ���A���̗v�f��Ԃ��R���[�`��(�x���]���̔z���\��)
      return {
         [itr = boost::coroutines2::detail::begin(list), num] (auto&& ret) mutable
         requires std::same_as<return_t, std::remove_cvref_t<decltype(ret)>> {
             while (num-- > 0) itr++;
             while (true) ret(*itr), itr++;
         }
      };
   };
}

// �x���]���̔z��́A�擪�v�f��Ԃ�
inline auto head = [](auto&& list) mutable
requires std::same_as<list_t, std::remove_cvref_t<decltype(list)>>
{
   return *begin(list);
};

// �x���]���̔z��́A�擪�v�f�����������X�g��Ԃ�
inline auto tail = [](auto&& list) mutable
requires std::same_as<list_t, std::remove_cvref_t<decltype(list)>>
{
   return list_t{
         [itr = boost::coroutines2::detail::begin(list)] (auto&& ret) mutable
         requires std::same_as<return_t, std::remove_cvref_t<decltype(ret)>> {
            itr++;
            while (true) ret(*itr), itr++;
         }
   };
};

// �x���]���̔z��́A�Ō���̗v�f��Ԃ�
inline auto last = [](auto&& list) mutable
requires std::same_as<list_t, std::remove_cvref_t<decltype(list)>>
{
   auto itr = begin(list);
   auto result = *itr;
   itr++;

   while (itr != end(list))
   {
      result = *itr;
      itr++;
   };

   return result;
};

// �x���]���̔z��́A�Ō���̗v�f�����������z���Ԃ�
inline auto init = [](auto&& list) mutable
requires std::same_as<list_t, std::remove_cvref_t<decltype(list)>>
{
   return list_t{
         [list = std::forward<decltype(list)>(list)] (auto&& ret) mutable
         requires std::same_as<return_t, std::remove_cvref_t<decltype(ret)>> {
              auto itr = begin(list);
              auto result = *itr;
              itr++;

              while (itr != end(list))
              {
                 ret(result);
                 result = *itr;
                 itr++;
              }
         }
   };
};

// �x���]���̔z��́A�v�f�̍��v��Ԃ�
inline auto sum = [](auto&& list) mutable
requires std::same_as<list_t, std::remove_cvref_t<decltype(list)>>
{
   return std::accumulate(begin(list), end(list), 0);
};

// lhs : �֐�, rhs : �R���[�`���i�z���\���j
// �֐� lhs �� rhs ��K�p���ĕԂ�
template<typename T, typename U>
auto operator <<=(T&& lhs, U&& rhs)
requires std::same_as<list_t, std::remove_cvref_t<U>>
{
   return std::forward<T>(lhs)(std::forward<U>(rhs));
}

}