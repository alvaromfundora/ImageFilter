#ifndef BILATERAL_FILTER_H
#define BILATERAL_FILTER_H

#include <vector>
#include <cmath>
#include <fstream>
#include <cstring>
#include <stdexcept>
#include <string>

// Simple RGB pixel structure
struct RGB {
    unsigned char r, g, b;
};

// Image class
class Image {
public:
    int width, height;
    std::vector<RGB> pixels;
    
    Image(int w, int h) : width(w), height(h), pixels(w * h) {
    }
    
    RGB& at(int x, int y) {
        return pixels[y * width + x];
    }
    
    const RGB& at(int x, int y) const {
        return pixels[y * width + x];
    }
    
    // Load PNG/JPEG/BMP using STB Image
    static Image load(const std::string& filename);
    
    // Save PNG
    void savePNG(const std::string& filename) const;
};

// Calculate spatial Gaussian weight
inline double spatialWeight(int dx, int dy, double sigmaSpatial) {
    return exp(-(dx * dx + dy * dy) / (2.0 * sigmaSpatial * sigmaSpatial));
}

// Calculate intensity/range Gaussian weight
inline double rangeWeight(int diff, double sigmaRange) {
    return exp(-(diff * diff) / (2.0 * sigmaRange * sigmaRange));
}

// Calculate color difference between two pixels
inline int colorDifference(const RGB& p1, const RGB& p2) {
    int dr = p1.r - p2.r;
    int dg = p1.g - p2.g;
    int db = p1.b - p2.b;
    return dr * dr + dg * dg + db * db;
}

// Apply Bilateral Filter
// sigmaSpatial: controls spatial smoothing (like Gaussian blur sigma)
// sigmaRange: controls how much color difference is preserved (edge preservation)
Image applyBilateralFilter(const Image& input, int kernelSize, double sigmaSpatial, double sigmaRange) {
    Image output(input.width, input.height);
    int offset = kernelSize / 2;
    
    for (int y = 0; y < input.height; y++) {
        for (int x = 0; x < input.width; x++) {
            const RGB& centerPixel = input.at(x, y);
            
            double sumR = 0.0, sumG = 0.0, sumB = 0.0;
            double sumWeight = 0.0;
            
            // Iterate over neighborhood
            for (int ky = -offset; ky <= offset; ky++) {
                for (int kx = -offset; kx <= offset; kx++) {
                    int nx = x + kx;
                    int ny = y + ky;
                    
                    // Clamp to image boundaries
                    nx = std::max(0, std::min(nx, input.width - 1));
                    ny = std::max(0, std::min(ny, input.height - 1));
                    
                    const RGB& neighborPixel = input.at(nx, ny);
                    
                    // Calculate spatial weight (based on distance)
                    double spatialW = spatialWeight(kx, ky, sigmaSpatial);
                    
                    // Calculate range weight (based on color similarity)
                    int colorDiff = colorDifference(centerPixel, neighborPixel);
                    double rangeW = rangeWeight(colorDiff, sigmaRange);
                    
                    // Combined weight
                    double weight = spatialW * rangeW;
                    
                    sumR += neighborPixel.r * weight;
                    sumG += neighborPixel.g * weight;
                    sumB += neighborPixel.b * weight;
                    sumWeight += weight;
                }
            }
            
            // Normalize by total weight
            RGB& outPixel = output.at(x, y);
            outPixel.r = static_cast<unsigned char>(sumR / sumWeight);
            outPixel.g = static_cast<unsigned char>(sumG / sumWeight);
            outPixel.b = static_cast<unsigned char>(sumB / sumWeight);
        }
    }
    
    return output;
}

// Fast approximate bilateral filter (using smaller kernel)
Image applyBilateralFilterFast(const Image& input, int kernelSize, double sigmaSpatial, double sigmaRange) {
    int fastKernelSize = std::min(kernelSize, 9);
    return applyBilateralFilter(input, fastKernelSize, sigmaSpatial, sigmaRange);
}

// Gaussian blur for comparison
Image applyGaussianBlur(const Image& input, int kernelSize, double sigma) {
    Image output(input.width, input.height);
    int offset = kernelSize / 2;
    
    // Generate 1D kernel
    std::vector<double> kernel1D(kernelSize);
    double sum = 0.0;
    int center = kernelSize / 2;
    
    for (int i = 0; i < kernelSize; i++) {
        int x = i - center;
        kernel1D[i] = exp(-(x * x) / (2.0 * sigma * sigma));
        sum += kernel1D[i];
    }
    for (int i = 0; i < kernelSize; i++) {
        kernel1D[i] /= sum;
    }
    
    // Horizontal pass
    Image temp(input.width, input.height);
    
    for (int y = 0; y < input.height; y++) {
        for (int x = 0; x < input.width; x++) {
            double sumR = 0.0, sumG = 0.0, sumB = 0.0;
            
            for (int k = 0; k < kernelSize; k++) {
                int px = x + k - offset;
                px = std::max(0, std::min(px, input.width - 1));
                
                const RGB& pixel = input.at(px, y);
                double weight = kernel1D[k];
                
                sumR += pixel.r * weight;
                sumG += pixel.g * weight;
                sumB += pixel.b * weight;
            }
            
            RGB& outPixel = temp.at(x, y);
            outPixel.r = static_cast<unsigned char>(sumR);
            outPixel.g = static_cast<unsigned char>(sumG);
            outPixel.b = static_cast<unsigned char>(sumB);
        }
    }
    
    // Vertical pass
    for (int y = 0; y < temp.height; y++) {
        for (int x = 0; x < temp.width; x++) {
            double sumR = 0.0, sumG = 0.0, sumB = 0.0;
            
            for (int k = 0; k < kernelSize; k++) {
                int py = y + k - offset;
                py = std::max(0, std::min(py, temp.height - 1));
                
                const RGB& pixel = temp.at(x, py);
                double weight = kernel1D[k];
                
                sumR += pixel.r * weight;
                sumG += pixel.g * weight;
                sumB += pixel.b * weight;
            }
            
            RGB& outPixel = output.at(x, y);
            outPixel.r = static_cast<unsigned char>(sumR);
            outPixel.g = static_cast<unsigned char>(sumG);
            outPixel.b = static_cast<unsigned char>(sumB);
        }
    }
    
    return output;
}

// Sharpen filter
Image applySharpen(const Image& input, double amount = 1.0) {
    Image output(input.width, input.height);
    
    // Sharpening kernel
    double kernel[3][3] = {
        { 0, -1,  0},
        {-1,  5, -1},
        { 0, -1,  0}
    };
    
    // Scale kernel by amount
    kernel[1][1] = 1 + 4 * amount;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i != 1 || j != 1) {
                kernel[i][j] = -amount;
            }
        }
    }
    
    for (int y = 0; y < input.height; y++) {
        for (int x = 0; x < input.width; x++) {
            double sumR = 0.0, sumG = 0.0, sumB = 0.0;
            
            for (int ky = 0; ky < 3; ky++) {
                for (int kx = 0; kx < 3; kx++) {
                    int px = x + kx - 1;
                    int py = y + ky - 1;
                    
                    px = std::max(0, std::min(px, input.width - 1));
                    py = std::max(0, std::min(py, input.height - 1));
                    
                    const RGB& pixel = input.at(px, py);
                    
                    sumR += pixel.r * kernel[ky][kx];
                    sumG += pixel.g * kernel[ky][kx];
                    sumB += pixel.b * kernel[ky][kx];
                }
            }
            
            RGB& outPixel = output.at(x, y);
            outPixel.r = static_cast<unsigned char>(std::min(255.0, std::max(0.0, sumR)));
            outPixel.g = static_cast<unsigned char>(std::min(255.0, std::max(0.0, sumG)));
            outPixel.b = static_cast<unsigned char>(std::min(255.0, std::max(0.0, sumB)));
        }
    }
    
    return output;
}

#endif // BILATERAL_FILTER_H