#ifndef STYLE_LOADER_HPP
#define STYLE_LOADER_HPP

#include <QString>


enum class Theme {
    Light,
    Dark
};

QString loadStyleSheet(Theme theme);


#endif // STYLE_LOADER_HPP