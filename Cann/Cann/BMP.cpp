#include "BMP.h"


bool sfImageToBMP(const sf::Image& img, BMP& outBmp)
{
    // wymiary
    const uint32_t width = img.getSize().x;
    const uint32_t height = img.getSize().y;
    if (width == 0 || height == 0) return false;

    // ustawiamy infoHeader (BITMAPINFOHEADER)
    outBmp.infoHeader.size = 40;                 // rozmiar BITMAPINFOHEADER
    outBmp.infoHeader.width = static_cast<int32_t>(width);
    outBmp.infoHeader.height = static_cast<int32_t>(height);
    outBmp.infoHeader.planes = 1;
    outBmp.infoHeader.bit_count = 24;            // 24-bit (R,G,B)
    outBmp.infoHeader.compression = 0;           // BI_RGB (brak kompresji)

    // wiersz BMP musi byæ wyrównany do 4 bajtów
    const uint32_t bytesPerPixel = 3; // BGR
    const uint32_t rowSizeNoPad = width * bytesPerPixel;
    const uint32_t rowSizePadded = (rowSizeNoPad + 3) & ~3u; // round up to multiple of 4
    outBmp.infoHeader.size_image = rowSizePadded * height;

    // pozosta³e pola infoHeader
    outBmp.infoHeader.x_pixels_per_meter = 2835; // ~72 DPI
    outBmp.infoHeader.y_pixels_per_meter = 2835;
    outBmp.infoHeader.colors_used = 0;
    outBmp.infoHeader.colors_important = 0;

    // ustawiamy fileHeader
    const uint32_t fileHeaderSize = sizeof(outBmp.fileHeader);   // powinno byæ 14 bytes
    const uint32_t infoHeaderSize = outBmp.infoHeader.size;      // 40 bytes
    outBmp.fileHeader.offset_data = fileHeaderSize + infoHeaderSize; // 54
    outBmp.fileHeader.file_size = outBmp.fileHeader.offset_data + outBmp.infoHeader.size_image;

    // colorHeader zostawiamy domyœlnie (zerowy) — nie u¿ywamy go dla 24-bit BMP
    // clear data and allocate
    outBmp.data.clear();
    outBmp.data.resize(outBmp.infoHeader.size_image);

    // wype³niamy dane pikseli: BMP = wiersze od do³u do góry, ka¿dy piksel B,G,R
    // ka¿dy wiersz ma rowSizePadded bajtów (mo¿e zawieraæ padding na koñcu)
    for (uint32_t y = 0; y < height; ++y) {
        uint32_t bmpRowIndex = (height - 1 - y) * rowSizePadded; // BMP stores bottom-up
        for (uint32_t x = 0; x < width; ++x) {
            sf::Color c = img.getPixel(sf::Vector2u(x, y));
            uint32_t pixelOffset = bmpRowIndex + x * bytesPerPixel;
            outBmp.data[pixelOffset + 0] = c.b; // blue
            outBmp.data[pixelOffset + 1] = c.g; // green
            outBmp.data[pixelOffset + 2] = c.r; // red
        }
        // padding bytes already zero-initialized because we resized vector; if not, ensure zero:
        // for (uint32_t p = rowSizeNoPad; p < rowSizePadded; ++p) outBmp.data[bmpRowIndex + p] = 0;
    }

    return true;
}
