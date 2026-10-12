import std;

import ada;

using ada::i64;
using ada::u128;

namespace {

int& fallos() {
  static int valor = 0;
  return valor;
}

int& totales() {
  static int valor = 0;
  return valor;
}

template <typename T>
std::string texto(const T& valor) {
  if constexpr (std::formattable<T, char>) {
    return std::format("{}", valor);
  } else if constexpr (std::integral<T>) {
    return ada::a_texto(valor);
  } else {
    return "(sin representacion)";
  }
}

void verificar(bool condicion, std::string_view nombre) {
  ++totales();
  if (condicion) {
    std::cout << "[ok]    " << nombre << '\n';
    return;
  }
  ++fallos();
  std::cout << "[FALLO] " << nombre << '\n';
}

template <typename A, typename B>
void verificar_igual(const A& obtenido, const B& esperado, std::string_view nombre) {
  if (obtenido == esperado) {
    ++totales();
    std::cout << "[ok]    " << nombre << '\n';
    return;
  }
  ++fallos();
  ++totales();
  std::cout << "[FALLO] " << nombre << '\n'
            << "        esperado: " << texto(esperado) << '\n'
            << "        obtenido: " << texto(obtenido) << '\n';
}

template <typename F>
bool lanza_argumento_invalido(F funcion) {
  try {
    funcion();
  } catch (const std::invalid_argument&) {
    return true;
  } catch (...) {
    return false;
  }
  return false;
}

u128 potencia_de_dos(int k) {
  u128 valor = 1;
  for (int i = 0; i < k; ++i) {
    valor *= 2;
  }
  return valor;
}

ada::Mat2 a_mat2(u128 a, u128 b, u128 c, u128 d) {
  ada::Mat2 m;
  m.en(0, 0) = a;
  m.en(0, 1) = b;
  m.en(1, 0) = c;
  m.en(1, 1) = d;
  return m;
}

ada::Mat2 fibonacci_mat2() { return a_mat2(1, 1, 1, 0); }

ada::Matriz a_matriz(std::size_t n, std::initializer_list<i64> valores) {
  ada::Matriz m = ada::matriz_ceros(n);
  std::size_t i = 0;
  for (const i64 valor : valores) {
    m.d[i] = valor;
    ++i;
  }
  return m;
}

ada::Matriz identidad(std::size_t n) {
  ada::Matriz m = ada::matriz_ceros(n);
  for (std::size_t i = 0; i < n; ++i) {
    m.en(i, i) = 1;
  }
  return m;
}

void probar_potencia() {
  verificar_igual(ada::potencia_secuencial(2, 10), u128{1024}, "potencia: 2^10 secuencial");
  verificar_igual(ada::potencia_logaritmica(2, 10), u128{1024}, "potencia: 2^10 logaritmica");
  verificar_igual(ada::potencia_secuencial(5, 0), u128{1}, "potencia: exponente 0");
  verificar_igual(ada::potencia_logaritmica(0, 5), u128{0}, "potencia: base 0");
  verificar_igual(ada::potencia_logaritmica(1, 1000), u128{1}, "potencia: base 1");
  verificar_igual(ada::potencia_logaritmica(-1, 101), u128{-1}, "potencia: base -1 con exponente impar");
  verificar_igual(ada::potencia_logaritmica(-2, 10), u128{1024}, "potencia: (-2)^10");
  verificar_igual(ada::potencia_logaritmica(-3, 17), u128{-129140163}, "potencia: (-3)^17");
  verificar_igual(ada::potencia_logaritmica(2, 100), potencia_de_dos(100), "potencia: 2^100");
  verificar_igual(ada::potencia_logaritmica(-2, 101), -potencia_de_dos(100) * 2, "potencia: (-2)^101");

  bool coinciden = true;
  for (i64 base = -2; base <= 3; ++base) {
    for (unsigned exponente = 0; exponente <= 40; ++exponente) {
      coinciden = coinciden && ada::potencia_secuencial(base, exponente) ==
                                    ada::potencia_logaritmica(base, exponente);
    }
  }
  verificar(coinciden, "potencia: coinciden para bases -2..3 y exponentes 0..40");

  coinciden = true;
  for (unsigned exponente = 0; exponente <= 38; ++exponente) {
    coinciden = coinciden && ada::potencia_secuencial(10, exponente) ==
                                  ada::potencia_logaritmica(10, exponente);
  }
  verificar(coinciden, "potencia: coinciden para base 10 y exponentes 0..38");
}

void probar_fibonacci() {
  verificar_igual(ada::fib_secuencial(0), u128{0}, "fibonacci: F(0)");
  verificar_igual(ada::fib_secuencial(1), u128{1}, "fibonacci: F(1)");
  verificar_igual(ada::fib_logaritmico(2), u128{1}, "fibonacci: F(2)");
  verificar_igual(ada::fib_logaritmico(10), u128{55}, "fibonacci: F(10)");
  verificar_igual(ada::fib_logaritmico(20), u128{6765}, "fibonacci: F(20)");
  verificar_igual(ada::fib_logaritmico(50), u128{12586269025}, "fibonacci: F(50)");
  verificar_igual(ada::fib_logaritmico(90), u128{2880067194370816120}, "fibonacci: F(90)");

  bool coinciden = true;
  for (unsigned n = 0; n <= 183; ++n) {
    coinciden = coinciden && ada::fib_secuencial(n) == ada::fib_logaritmico(n);
  }
  verificar(coinciden, "fibonacci: secuencial y logaritmica coinciden para n = 0..183");

  verificar_igual(ada::fib_secuencial(184) % 1000000007, u128{501862063},
                  "fibonacci: F(184) mod 1000000007");
}

void probar_mat2() {
  const ada::Mat2 f = fibonacci_mat2();
  verificar(ada::potencia_mat2(f, 0) == ada::mat2_identidad(), "mat2: exponente 0 da la identidad");
  verificar(ada::potencia_mat2(f, 1) == f, "mat2: exponente 1 da la misma matriz");
  verificar(ada::potencia_mat2(a_mat2(2, 0, 0, 3), 3) == a_mat2(8, 0, 0, 27),
            "mat2: [[2,0],[0,3]]^3 == [[8,0],[0,27]]");
  verificar(ada::potencia_mat2(f, 5) == a_mat2(8, 5, 5, 3), "mat2: F^5 == [[8,5],[5,3]]");

  bool coinciden = true;
  for (unsigned n = 0; n <= 183; ++n) {
    coinciden = coinciden && ada::potencia_mat2(f, n).en(0, 1) == ada::fib_logaritmico(n);
  }
  verificar(coinciden, "mat2: potencia_mat2(F, n) coincide con fib_logaritmico(n) hasta n = 183");
}

void probar_matriz() {
  const ada::Matriz a = a_matriz(2, {1, 2, 3, 4});
  const ada::Matriz b = a_matriz(2, {5, 6, 7, 8});
  const ada::Matriz esperado = a_matriz(2, {19, 22, 43, 50});
  verificar_igual(ada::multiplicar_cubica(a, b), esperado, "matriz: cubica de [[1,2],[3,4]] por [[5,6],[7,8]]");
  verificar_igual(ada::multiplicar_strassen(a, b), esperado, "matriz: la misma con Strassen");

  for (const std::size_t n : {1UZ, 2UZ, 4UZ, 8UZ, 16UZ, 32UZ, 64UZ, 128UZ}) {
    const ada::Matriz x = ada::matriz_aleatoria(n, 9);
    const ada::Matriz y = ada::matriz_aleatoria(n, 9);
    verificar(ada::multiplicar_cubica(x, y) == ada::multiplicar_strassen(x, y),
              "matriz: cubica y Strassen coinciden en n = " + std::to_string(n));
  }

  const ada::Matriz m8 = ada::matriz_aleatoria(8, 9);
  verificar_igual(ada::multiplicar_cubica(identidad(8), m8), m8, "matriz: identidad por la izquierda");
  verificar_igual(ada::multiplicar_strassen(m8, identidad(8)), m8, "matriz: identidad por la derecha");

  verificar_igual(ada::multiplicar_cubica(ada::matriz_ceros(0), ada::matriz_ceros(0)),
                  ada::matriz_ceros(0), "matriz: el caso 0x0 no revienta");
  verificar(lanza_argumento_invalido([] {
             static_cast<void>(ada::multiplicar_cubica(ada::matriz_ceros(2), ada::matriz_ceros(3)));
           }),
            "matriz: la cubica rechaza dimensiones incompatibles");
  verificar(lanza_argumento_invalido([] {
             static_cast<void>(ada::multiplicar_strassen(ada::matriz_ceros(3), ada::matriz_ceros(3)));
           }),
            "matriz: Strassen exige una potencia de dos");
}

int ejecutar() {
  probar_potencia();
  probar_fibonacci();
  probar_mat2();
  probar_matriz();

  const int fallidos = fallos();
  const int total = totales();
  std::cout << "\n" << (total - fallidos) << " de " << total << " pruebas correctas\n";
  return fallidos == 0 ? 0 : 1;
}

}

int main() {
  try {
    return ejecutar();
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << '\n';
    return 1;
  } catch (...) {
    std::cerr << "error desconocido\n";
    return 1;
  }
}
