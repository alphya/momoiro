#include <boost/coroutine2/coroutine.hpp>
#include <numeric>
#include <concepts>

namespace momoiro
{

using coro_t = boost::coroutines2::coroutine<int>;
using list_t = coro_t::pull_type;
using return_t = coro_t::push_type;

// 配列(遅延評価)の先頭から、num 個の要素を取り出した配列を返す
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

// 配列(遅延評価)の先頭から、num 個の要素を取り出した配列を作る関数を返す
// メソッドチェーン用
auto taked(int num)
{
   // 配列 list の、先頭から num 個の要素を取り出した配列を返す関数
   return [num](auto&& list) mutable -> list_t
      requires std::same_as<return_t, std::remove_cvref_t<decltype(ret)>> {
      // 配列 list のイテレータインクリメントしながら、その要素を返すコルーチン(遅延評価の配列を表現)
      return {
         [itr = boost::coroutines2::detail::begin(list), num] (auto&& ret) mutable
         requires std::same_as<return_t, std::remove_cvref_t<decltype(ret)>> {
            while (num --> 0) ret(*itr), itr++;
         }
      };
   };
}

// 配列(遅延評価)の先頭から、num 個の要素を除去した配列を返す
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

// 配列(遅延評価)の先頭から、num 個の要素を除去した配列を作る関数を返す
// メソッドチェーン用
auto droped(int num)
{
   // 配列 list の、先頭から num 個の要素を取り出した配列を返す関数
   return [num](auto&& list) mutable -> list_t
      requires std::same_as<return_t, std::remove_cvref_t<decltype(ret)>> {
      // 配列 list のイテレータインクリメントしながら、その要素を返すコルーチン(遅延評価の配列を表現)
      return {
         [itr = boost::coroutines2::detail::begin(list), num] (auto&& ret) mutable
         requires std::same_as<return_t, std::remove_cvref_t<decltype(ret)>> {
             while (num-- > 0) itr++;
             while (true) ret(*itr), itr++;
         }
      };
   };
}

// 遅延評価の配列の、先頭要素を返す
inline auto head = [](auto&& list) mutable
requires std::same_as<list_t, std::remove_cvref_t<decltype(list)>>
{
   return *begin(list);
};

// 遅延評価の配列の、先頭要素を除いたリストを返す
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

// 遅延評価の配列の、最後尾の要素を返す
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

// 遅延評価の配列の、最後尾の要素を除去した配列を返す
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

// 遅延評価の配列の、要素の合計を返す
inline auto sum = [](auto&& list) mutable
requires std::same_as<list_t, std::remove_cvref_t<decltype(list)>>
{
   return std::accumulate(begin(list), end(list), 0);
};

// lhs : 関数, rhs : コルーチン（配列を表現）
// 関数 lhs に rhs を適用して返す
template<typename T, typename U>
auto operator <<=(T&& lhs, U&& rhs)
requires std::same_as<list_t, std::remove_cvref_t<U>>
{
   return std::forward<T>(lhs)(std::forward<U>(rhs));
}

}