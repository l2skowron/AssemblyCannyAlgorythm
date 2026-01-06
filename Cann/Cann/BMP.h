#pragma once
#include <cstdint>
#include <vector>
#include <SFML/Graphics.hpp>
// Nag³ówek pliku BMP (14 bajtów)
struct BMPFileHeader {
    uint16_t file_type{ 0x4D42 };      // "BM" = 0x4D42
    uint32_t file_size{ 0 };      // Rozmiar ca³ego pliku w bajtach
    uint16_t reserved1{ 0 }; // Zawsze 0
    uint16_t reserved2{ 0 }; // Zawsze 0
    uint32_t offset_data{ 0 };   // Offset od pocz¹tku pliku do danych pikseli
} ; 

// Nag³ówek informacji BMP (BITMAPINFOHEADER, 40 bajtów)
struct BMPInfoHeader {
    uint32_t size{ 0 };          // Rozmiar nag³ówka informacji (40)
    int32_t  width{ 0 };         // Szerokoœæ obrazu w pikselach
    int32_t  height{ 0 };        // Wysokoœæ obrazu w pikselach
    uint16_t planes{ 1 };        // Zawsze 1
    uint16_t bit_count{ 0 };      // Liczba bitów na piksel (np. 24)
    uint32_t compression{ 0 };   // Kompresja (0 = brak)
    uint32_t size_image{ 0 };     // Rozmiar danych pikseli (mo¿e byæ 0 jeœli brak kompresji)
    int32_t  x_pixels_per_meter{ 0 }; // Rozdzielczoœæ pozioma w pikselach/m
    int32_t  y_pixels_per_meter{ 0 }; // Rozdzielczoœæ pionowa w pikselach/m
    uint32_t colors_used{ 0 };       // Liczba u¿ytych kolorów (0 = wszystkie)
    uint32_t colors_important{ 0 };  // Wa¿ne kolory (0 = wszystkie)
};
struct BMPColorHeader {
    uint32_t red_mask{ 0x00ff0000 };         // Bit mask for the red channel
    uint32_t green_mask{ 0x0000ff00 };       // Bit mask for the green channel
    uint32_t blue_mask{ 0x000000ff };        // Bit mask for the blue channel
    uint32_t alpha_mask{ 0xff000000 };       // Bit mask for the alpha channel
    uint32_t color_space_type{ 0x73524742 }; // Default "sRGB" (0x73524742)
    uint32_t unused[16]{ 0 };                // Unused data for sRGB color space
};
struct BMP {
	BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;
	BMPColorHeader colorHeader;
	std::vector<uint8_t> data;

};
bool sfImageToBMP(const sf::Image& img, BMP& outBmp);
