import std;

import ada;

using ada::i64;
using ada::u128;

namespace {

std::optional<i64> a_entero(std::string_view texto) {
  i64 valor = 0;
  const char* inicio = texto.data();
  const char* fin = texto.data() + texto.size();
  const auto [punto, error] = std::from_chars(inicio, fin, valor);
  if (error != std::errc{} || punto != fin) {
    return std::nullopt;
  }
  return valor;
}

int uso(std::string_view mensaje) {
  if (!mensaje.empty()) {
    std::cout << "error: " << mensaje << '\n';
  }
  std::cout << "uso:\n"
               "  ada_cli potencia <base> <exponente>\n";
  return 1;
}

int potencia(int argc, char* const* argv) {
  if (argc != 4) {
    return uso("se esperan base y exponente");
  }
  const auto base = a_entero(argv[2]);
  const auto exponente = a_entero(argv[3]);
  if (!base || !exponente) {
    return uso("la base y el exponente deben ser enteros");
  }
  if (*exponente < 0 || !std::in_range<unsigned>(*exponente)) {
    return uso("el exponente debe estar entre 0 y 4294967295");
  }
  const auto e = static_cast<unsigned>(*exponente);

  unsigned maximo_exponente = std::numeric_limits<unsigned>::max();
  const double magnitud = std::abs(static_cast<double>(*base));
  if (magnitud > 1.0) {
    maximo_exponente = static_cast<unsigned>(127.0 / std::log2(magnitud));
  }

  u128 secuencial = 0;
  u128 logaritmica = 0;
  const double ms_secuencial =
      ada::medir_ms([&] { secuencial = ada::potencia_secuencial(*base, e); });
  const double ms_logaritmica =
      ada::medir_ms([&] { logaritmica = ada::potencia_logaritmica(*base, e); });
  const bool coinciden = secuencial == logaritmica;

  std::cout << "base = " << *base << ", exponente = " << e << '\n'
            << "secuencial   O(n)      = " << ada::a_texto(secuencial) << "   " << ms_secuencial
            << " ms\n"
            << "logaritmica  O(log n)  = " << ada::a_texto(logaritmica) << "   " << ms_logaritmica
            << " ms\n"
            << "coinciden: " << (coinciden ? "si" : "no") << '\n';
  if (e > maximo_exponente) {
    std::cout << "aviso: el resultado no cabe en 128 bits con signo\n";
  }
  return coinciden ? 0 : 1;
}

int ejecutar(int argc, char* const* argv) {
  if (argc < 2) {
    return uso("falta la orden");
  }
  const std::string_view orden{argv[1]};
  if (orden == "potencia") {
    return potencia(argc, argv);
  }
  return uso("orden desconocida");
}

}  // namespace

int main(int argc, char** argv) {
  try {
    return ejecutar(argc, argv);
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << '\n';
    return 1;
  } catch (...) {
    std::cerr << "error desconocido\n";
    return 1;
  }
}
