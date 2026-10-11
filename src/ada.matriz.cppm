export module ada.matriz;

import std;

export import ada.tipos;

export namespace ada {

struct Mat2 {
  std::array<std::array<u128, 2>, 2> m{};

  u128& en(std::size_t fila, std::size_t columna) { return m.at(fila).at(columna); }
  u128 en(std::size_t fila, std::size_t columna) const { return m.at(fila).at(columna); }

  bool operator==(const Mat2& otro) const = default;
};

Mat2 mat2_identidad();
Mat2 mat2_multiplicar(const Mat2& a, const Mat2& b);

Mat2 potencia_mat2(const Mat2& a, unsigned exponente);

struct Matriz {
  std::size_t n = 0;
  std::vector<i64> d;

  i64& en(std::size_t fila, std::size_t columna) { return d[fila * n + columna]; }
  i64 en(std::size_t fila, std::size_t columna) const { return d[fila * n + columna]; }

  bool operator==(const Matriz& otra) const = default;
};

Matriz matriz_ceros(std::size_t n);
Matriz matriz_aleatoria(std::size_t n, i64 maximo, unsigned semilla = 20261009);

Matriz multiplicar_cubica(const Matriz& a, const Matriz& b);
Matriz multiplicar_strassen(const Matriz& a, const Matriz& b);

}  // namespace ada

namespace ada::detalle {

constexpr std::size_t UMBRAL_STRASSEN = 64;

inline Matriz bloque(const Matriz& a, std::size_t fila0, std::size_t col0, std::size_t n) {
  Matriz r = matriz_ceros(n);
  for (std::size_t i = 0; i < n; ++i) {
    for (std::size_t j = 0; j < n; ++j) {
      r.en(i, j) = a.en(fila0 + i, col0 + j);
    }
  }
  return r;
}

inline void colocar(Matriz& destino, const Matriz& parte, std::size_t fila0, std::size_t col0) {
  for (std::size_t i = 0; i < parte.n; ++i) {
    for (std::size_t j = 0; j < parte.n; ++j) {
      destino.en(fila0 + i, col0 + j) = parte.en(i, j);
    }
  }
}

inline Matriz sumar(const Matriz& a, const Matriz& b) {
  Matriz r = matriz_ceros(a.n);
  for (std::size_t i = 0; i < a.d.size(); ++i) {
    r.d[i] = a.d[i] + b.d[i];
  }
  return r;
}

inline Matriz restar(const Matriz& a, const Matriz& b) {
  Matriz r = matriz_ceros(a.n);
  for (std::size_t i = 0; i < a.d.size(); ++i) {
    r.d[i] = a.d[i] - b.d[i];
  }
  return r;
}

Matriz strassen(const Matriz& a, const Matriz& b);

}  // namespace ada::detalle

namespace ada {

Mat2 mat2_identidad() {
  Mat2 r;
  r.en(0, 0) = 1;
  r.en(1, 1) = 1;
  return r;
}

Mat2 mat2_multiplicar(const Mat2& a, const Mat2& b) {
  Mat2 r;
  for (std::size_t i = 0; i < 2; ++i) {
    for (std::size_t k = 0; k < 2; ++k) {
      for (std::size_t j = 0; j < 2; ++j) {
        r.en(i, j) += a.en(i, k) * b.en(k, j);
      }
    }
  }
  return r;
}

Mat2 potencia_mat2(const Mat2& a, unsigned exponente) {
  if (exponente == 0) {
    return mat2_identidad();
  }
  const Mat2 mitad = potencia_mat2(a, exponente / 2);
  const Mat2 cuadrado = mat2_multiplicar(mitad, mitad);
  return (exponente % 2 == 0) ? cuadrado : mat2_multiplicar(cuadrado, a);
}

Matriz matriz_ceros(std::size_t n) {
  Matriz r;
  r.n = n;
  r.d.assign(n * n, 0);
  return r;
}

Matriz matriz_aleatoria(std::size_t n, i64 maximo, unsigned semilla) {
  Matriz r = matriz_ceros(n);
  std::mt19937 generador(semilla);
  std::uniform_int_distribution<i64> valores(-maximo, maximo);
  for (i64& valor : r.d) {
    valor = valores(generador);
  }
  return r;
}

Matriz multiplicar_cubica(const Matriz& a, const Matriz& b) {
  if (a.n != b.n) {
    throw std::invalid_argument("multiplicar_cubica: dimensiones incompatibles");
  }
  Matriz r = matriz_ceros(a.n);
  for (std::size_t i = 0; i < a.n; ++i) {
    for (std::size_t k = 0; k < a.n; ++k) {
      const i64 factor = a.en(i, k);
      for (std::size_t j = 0; j < a.n; ++j) {
        r.en(i, j) += factor * b.en(k, j);
      }
    }
  }
  return r;
}

Matriz multiplicar_strassen(const Matriz& a, const Matriz& b) {
  if (a.n != b.n) {
    throw std::invalid_argument("multiplicar_strassen: dimensiones incompatibles");
  }
  if (!std::has_single_bit(a.n)) {
    throw std::invalid_argument("multiplicar_strassen: n debe ser potencia de dos");
  }
  return detalle::strassen(a, b);
}

}  // namespace ada

namespace ada::detalle {

Matriz strassen(const Matriz& a, const Matriz& b) {
  const std::size_t n = a.n;
  if (n <= UMBRAL_STRASSEN) {
    return multiplicar_cubica(a, b);
  }
  const std::size_t m = n / 2;

  const Matriz a11 = bloque(a, 0, 0, m);
  const Matriz a12 = bloque(a, 0, m, m);
  const Matriz a21 = bloque(a, m, 0, m);
  const Matriz a22 = bloque(a, m, m, m);
  const Matriz b11 = bloque(b, 0, 0, m);
  const Matriz b12 = bloque(b, 0, m, m);
  const Matriz b21 = bloque(b, m, 0, m);
  const Matriz b22 = bloque(b, m, m, m);

  const Matriz m1 = strassen(sumar(a11, a22), sumar(b11, b22));
  const Matriz m2 = strassen(sumar(a21, a22), b11);
  const Matriz m3 = strassen(a11, restar(b12, b22));
  const Matriz m4 = strassen(a22, restar(b21, b11));
  const Matriz m5 = strassen(sumar(a11, a12), b22);
  const Matriz m6 = strassen(restar(a21, a11), sumar(b11, b12));
  const Matriz m7 = strassen(restar(a12, a22), sumar(b21, b22));

  Matriz r = matriz_ceros(n);
  colocar(r, sumar(restar(sumar(m1, m4), m5), m7), 0, 0);
  colocar(r, sumar(m3, m5), 0, m);
  colocar(r, sumar(m2, m4), m, 0);
  colocar(r, sumar(restar(sumar(m1, m3), m2), m6), m, m);
  return r;
}

}  // namespace ada::detalle
