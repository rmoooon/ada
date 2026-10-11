export module ada.cronometro;

import std;

export namespace ada {

double medir_ms(const std::function<void()>& tarea, int repeticiones = 1) {
  const auto inicio = std::chrono::steady_clock::now();
  for (int i = 0; i < repeticiones; ++i) {
    tarea();
  }
  const auto fin = std::chrono::steady_clock::now();
  const double total = std::chrono::duration<double, std::milli>(fin - inicio).count();
  return total / static_cast<double>(std::max(repeticiones, 1));
}

}  // namespace ada
