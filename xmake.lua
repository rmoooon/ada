set_project("ada")
set_version("1.0.0")
set_languages("c++23")

add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", { outputdir = "." })

set_warnings("all", "extra", "pedantic")

set_policy("build.warning", true)
set_policy("build.c++.modules", true)
set_policy("build.c++.modules.reuse", true)

if is_mode("debug") then
  set_symbols("debug")
  set_optimize("none")
  set_policy("build.sanitizer.address", true)
  set_policy("build.sanitizer.undefined", true)
elseif is_mode("release") then
  set_symbols("hidden")
  set_optimize("fastest")
end
