export module ada.tipos;

import std;

export namespace ada {

using u128 = __int128;
using u64 = std::uint64_t;
using i64 = std::int64_t;

std::string a_texto(u128 v) {
  if (v == 0) {
    return "0";
  }
  const bool negativo = v < 0;
  auto m = static_cast<unsigned __int128>(v);
  if (negativo) {
    m = static_cast<unsigned __int128>(0) - m;
  }
  std::string s;
  while (m > 0) {
    s.push_back(static_cast<char>('0' + static_cast<int>(m % 10)));
    m /= 10;
  }
  if (negativo) {
    s.push_back('-');
  }
  std::ranges::reverse(s);
  return s;
}

}  // namespace ada
