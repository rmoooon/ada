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
  if (error == std::errc::invalid_argument || error == std::errc::result_out_of_range ||
      punto != fin) {
    return std::nullopt;
  }
  return valor;
}

double exponente_estimado(double ms1, double ms2, std::size_t n1, std::size_t n2) {
  return std::log(ms2 / ms1) / std::log(static_cast<double>(n2) / static_cast<double>(n1));
}

void comparar_potencias() {
  std::cout << "\npotencia con base 1 (mismo trabajo en las dos versiones)\n";
  std::cout << std::setw(12) << "exponente" << std::setw(16) << "secuencial" << std::setw(16)
            << "logaritmica" << std::setw(12) << "ratio" << '\n';

  u128 comprobacion = 0;
  for (const unsigned exponente : {1000U, 10000U, 100000U, 1000000U, 10000000U}) {
    u128 secuencial = 0;
    u128 logaritmica = 0;
    const double ms_secuencial =
        ada::medir_ms([&] { secuencial = ada::potencia_secuencial(1, exponente); });
    const double ms_logaritmica =
        ada::medir_ms([&] { logaritmica = ada::potencia_logaritmica(1, exponente); }, 1000);
    comprobacion += secuencial + logaritmica;

    std::cout << std::setw(12) << exponente << std::setw(16) << ms_secuencial << std::setw(16)
              << ms_logaritmica << std::setw(12) << ms_secuencial / ms_logaritmica << '\n';
  }
  std::cout << "comprobacion: " << ada::a_texto(comprobacion) << '\n';
}

void comparar_fibonacci() {
  std::cout << "\nfibonacci exacto (el ultimo que cabe es F(183) por la via logaritmica)\n";
  std::cout << std::setw(6) << "n" << std::setw(16) << "secuencial" << std::setw(16)
            << "logaritmica" << std::setw(12) << "ratio" << '\n';

  u128 comprobacion = 0;
  for (const unsigned n : {60U, 90U, 120U, 183U}) {
    u128 secuencial = 0;
    u128 logaritmica = 0;
    const double ms_secuencial = ada::medir_ms([&] { secuencial = ada::fib_secuencial(n); }, 20000);
    const double ms_logaritmica =
        ada::medir_ms([&] { logaritmica = ada::fib_logaritmico(n); }, 20000);
    comprobacion += secuencial + logaritmica;

    std::cout << std::setw(6) << n << std::setw(16) << ms_secuencial << std::setw(16)
              << ms_logaritmica << std::setw(12) << ms_secuencial / ms_logaritmica << '\n';
  }
  std::cout << "comprobacion mod 1000000007: " << ada::a_texto(comprobacion % 1000000007) << '\n'
            << "nota: con n <= 183 el factor constante de la version matricial pesa mas que el\n"
               "logaritmo; la ventaja asintotica aparece para n mucho mayor\n";
}

void comparar_matrices(std::size_t maximo) {
  std::cout << "\nmultiplicacion de matrices cuadradas (potencias de dos)\n";
  std::cout << std::setw(8) << "n" << std::setw(16) << "cubica" << std::setw(16) << "strassen"
            << std::setw(13) << "exp cubica" << std::setw(13) << "exp strassen" << '\n';

  double anterior_cubica = 0;
  double anterior_strassen = 0;
  std::size_t anterior_n = 0;
  bool coinciden_todas = true;

  for (std::size_t n = 32; n <= maximo; n *= 2) {
    const ada::Matriz a = ada::matriz_aleatoria(n, 9);
    const ada::Matriz b = ada::matriz_aleatoria(n, 9);
    ada::Matriz cubica;
    ada::Matriz strassen;
    const double ms_cubica = ada::medir_ms([&] { cubica = ada::multiplicar_cubica(a, b); });
    const double ms_strassen = ada::medir_ms([&] { strassen = ada::multiplicar_strassen(a, b); });
    coinciden_todas = coinciden_todas && cubica == strassen;

    double exp_cubica = 0;
    double exp_strassen = 0;
    if (anterior_n != 0) {
      exp_cubica = exponente_estimado(anterior_cubica, ms_cubica, anterior_n, n);
      exp_strassen = exponente_estimado(anterior_strassen, ms_strassen, anterior_n, n);
    }
    std::cout << std::setw(8) << n << std::setw(16) << ms_cubica << std::setw(16) << ms_strassen
              << std::setw(12) << exp_cubica << std::setw(12) << exp_strassen << '\n';

    anterior_cubica = ms_cubica;
    anterior_strassen = ms_strassen;
    anterior_n = n;
  }
  std::cout << "todas coinciden con la cubica: " << (coinciden_todas ? "si" : "no") << '\n';
}

int ejecutar(int argc, char* const* argv) {
  std::size_t maximo = 256;
  if (argc > 1) {
    const auto valor = a_entero(argv[1]);
    if (!valor || *valor < 32 || !std::in_range<std::size_t>(*valor)) {
      std::cout << "uso: bench [tamano_maximo de las matrices, potencia de dos >= 32]\n";
      return 1;
    }
    maximo = static_cast<std::size_t>(*valor);
  }

  comparar_potencias();
  comparar_fibonacci();
  comparar_matrices(maximo);
  return 0;
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
