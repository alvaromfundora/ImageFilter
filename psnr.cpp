#include "image_quality.h"
#include <iostream>
#include <cmath>
#include <limits>

/**
 * Calculate the Peak Signal-to-Noise Ratio (PSNR) between two images
 * 
 * PSNR is a metric used to measure the quality of a reconstructed image
 * compared to the original. Higher PSNR values indicate better quality.
 * 
 * @param original The original reference image
 * @param compressed The compressed or modified image to compare
 * @return double The PSNR value in decibels (dB), or -1 if calculation fails
 */
double calculatePSNR(const cv::Mat& original, const cv::Mat& compressed) {
    // Check if both images are valid (not empty)
    // Empty images would cause errors in subsequent operations
    if (original.empty() || compressed.empty()) {
        std::cerr << "Error: One or both images are empty!" << std::endl;
        return -1.0;
    }
    
    // Verify that both images have the same dimensions
    // PSNR requires pixel-by-pixel comparison, so dimensions must match
    if (original.size() != compressed.size()) {
        std::cerr << "Error: Images must have the same dimensions!" << std::endl;
        return -1.0;
    }
    
    // Verify that both images have the same number of channels (e.g., both RGB or both grayscale)
    // This ensures we're comparing compatible image types
    if (original.channels() != compressed.channels()) {
        std::cerr << "Error: Images must have the same number of channels!" << std::endl;
        return -1.0;
    }
    
    // Convert images to CV_64F (64-bit floating point) format
    // This provides high precision for calculations and prevents overflow
    cv::Mat orig_float, comp_float;
    original.convertTo(orig_float, CV_64F);
    compressed.convertTo(comp_float, CV_64F);
    
    // Calculate the difference between the two images
    // This creates a new matrix where each pixel contains (original - compressed)
    cv::Mat diff = orig_float - comp_float;
    
    // Square each element in the difference matrix
    // diff.mul(diff) performs element-wise multiplication (squaring)
    // This gives us the squared error for each pixel
    diff = diff.mul(diff);
    
    // Sum all the squared differences across all channels and pixels
    // This reduces the entire matrix to a single scalar value per channel
    cv::Scalar sum_squared_error = cv::sum(diff);
    
    // Calculate the total sum of squared errors across all channels
    // For RGB images, this adds the errors from R, G, and B channels
    // For grayscale, there's only one channel, so sum_squared_error[0] is used directly
    double sse = 0.0;  // SSE = Sum of Squared Errors
    for (int i = 0; i < original.channels(); i++) {
        sse += sum_squared_error[i];
    }
    
    // Calculate the total number of pixel values in the image
    // total_pixels = width × height × number_of_channels
    // This is needed to compute the mean squared error (MSE)
    double total_pixels = original.total() * original.channels();
    
    // Calculate the Mean Squared Error (MSE)
    // MSE is the average squared difference between corresponding pixels
    double mse = sse / total_pixels;
    
    // Handle the special case where MSE is 0
    // This means the images are identical, so PSNR is infinite
    // We return a very high value (infinity) to indicate perfect match
    if (mse == 0.0) {
        std::cout << "Images are identical (MSE = 0), PSNR is infinite" << std::endl;
        return std::numeric_limits<double>::infinity();
    }
    
    // Determine the maximum possible pixel value
    // For 8-bit images (most common), this is 255
    // For 16-bit images, this would be 65535
    double max_pixel_value;
    if (original.depth() == CV_8U) {
        max_pixel_value = 255.0;  // 8-bit unsigned integer
    } else if (original.depth() == CV_16U) {
        max_pixel_value = 65535.0;  // 16-bit unsigned integer
    } else {
        max_pixel_value = 1.0;  // For floating-point images normalized to [0,1]
    }
    
    // Calculate PSNR using the formula:
    // PSNR = 10 × log₁₀(MAX²/MSE)
    // Where:
    //   - MAX is the maximum possible pixel value
    //   - MSE is the mean squared error
    //   - log₁₀ is the base-10 logarithm
    // 
    // The result is in decibels (dB)
    // Higher values indicate better quality (less noise/distortion)
    // Typical values:
    //   - > 40 dB: Excellent quality
    //   - 30-40 dB: Good quality
    //   - 20-30 dB: Acceptable quality
    //   - < 20 dB: Poor quality
    double psnr = 10.0 * log10((max_pixel_value * max_pixel_value) / mse);
    
    return psnr;
}