#include "image_quality.h"
#include <iostream>
#include <algorithm>
#include <cmath>

/**
 * Apply Gaussian blur to an image
 * 
 * This function reduces noise in the image by applying a Gaussian filter.
 * The Gaussian filter performs a weighted average of neighboring pixels,
 * with weights determined by a Gaussian (bell curve) distribution.
 * 
 * Pixels closer to the center have more influence than pixels farther away.
 * This creates a smooth blur that reduces high-frequency noise while
 * preserving overall image structure.
 * 
 * @param input The input image to blur
 * @param kernelSize The size of the Gaussian kernel (must be odd, e.g., 5, 7, 11)
 * @param sigma The standard deviation of the Gaussian distribution
 *              - Larger values create more blur
 *              - Typical values: 0.5 to 3.0
 * @return cv::Mat The blurred output image
 */
cv::Mat applyGaussianBlur(const cv::Mat& input, int kernelSize, double sigma) {
    // Create an output matrix to store the blurred image
    cv::Mat output;
    
    // Ensure kernel size is odd (required for symmetric filter)
    // If even, increment by 1 to make it odd
    if (kernelSize % 2 == 0) {
        kernelSize += 1;
    }
    
    // Apply OpenCV's built-in Gaussian blur function
    // Parameters:
    //   - input: source image
    //   - output: destination image (automatically allocated)
    //   - cv::Size(kernelSize, kernelSize): square kernel dimensions
    //   - sigma: standard deviation in X direction
    //   - sigma: standard deviation in Y direction (same as X for isotropic blur)
    cv::GaussianBlur(input, output, cv::Size(kernelSize, kernelSize), sigma, sigma);
    
    return output;
}

/**
 * Apply unsharp masking filter to sharpen an image
 * 
 * Unsharp masking is a classic sharpening technique that works by:
 * 1. Creating a blurred version of the image (already provided as parameter)
 * 2. Subtracting the blur from the original to isolate edges/details
 * 3. Adding these amplified details back to the original image
 * 
 * The mathematical formula is:
 *     sharpened = original + amount × (original - blurred)
 * 
 * Where:
 *     (original - blurred) = high-frequency details (edges and fine features)
 *     amount = amplification factor for these details
 * 
 * This enhances edges and makes the image appear crisper without adding
 * artificial patterns like some other sharpening methods.
 * 
 * @param original The original input image (before any filtering)
 * @param blurred The Gaussian-blurred version of the original image
 * @param amount Sharpening strength - how much to amplify the details
 *               - Values < 1.0: subtle sharpening
 *               - Values 1.0-2.0: moderate sharpening (typical)
 *               - Values > 2.0: aggressive sharpening (may amplify noise)
 * @param threshold Minimum pixel difference required to apply sharpening
 *                  - Helps reduce noise amplification
 *                  - Values close to 0: sharpen everything
 *                  - Higher values: only sharpen significant edges
 * @return cv::Mat The sharpened output image
 */
cv::Mat applyUnsharpMask(const cv::Mat& original, const cv::Mat& blurred, 
                         double amount, double threshold) {
    // Validate input images
    if (original.empty() || blurred.empty()) {
        std::cerr << "Error: Input images cannot be empty!" << std::endl;
        return cv::Mat();
    }
    
    // Verify dimensions match
    if (original.size() != blurred.size()) {
        std::cerr << "Error: Original and blurred images must have same dimensions!" << std::endl;
        return cv::Mat();
    }
    
    // Verify same number of channels
    if (original.channels() != blurred.channels()) {
        std::cerr << "Error: Original and blurred images must have same number of channels!" << std::endl;
        return cv::Mat();
    }
    
    // Create output image with same dimensions and type as input
    cv::Mat output = cv::Mat::zeros(original.size(), original.type());
    
    // Get image dimensions
    int rows = original.rows;
    int cols = original.cols;
    int channels = original.channels();
    
    // Process each pixel
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            // Process each color channel independently
            for (int c = 0; c < channels; c++) {
                // Get pixel values as doubles for precision
                double orig_val = static_cast<double>(original.at<cv::Vec3b>(y, x)[c]);
                double blur_val = static_cast<double>(blurred.at<cv::Vec3b>(y, x)[c]);
                
                // Calculate the detail (high-frequency component)
                // This is the difference between original and blurred
                // Positive values = original was brighter (edge going up)
                // Negative values = original was darker (edge going down)
                double detail = orig_val - blur_val;
                
                // Apply threshold to reduce noise amplification
                // Only sharpen if the detail exceeds the threshold
                // Small differences (likely noise) are ignored
                if (std::abs(detail) < threshold) {
                    detail = 0.0;
                }
                
                // Apply unsharp mask formula:
                // output = original + amount × detail
                // The 'amount' controls how much sharpening to apply
                double sharpened = orig_val + amount * detail;
                
                // Clamp result to valid pixel range [0, 255]
                // This prevents overflow (values > 255) and underflow (values < 0)
                sharpened = std::min(255.0, std::max(0.0, sharpened));
                
                // Store the result in the output image
                output.at<cv::Vec3b>(y, x)[c] = static_cast<unsigned char>(sharpened);
            }
        }
    }
    
    return output;
}

/**
 * Calculate composite quality score
 * 
 * This function combines PSNR and SSIM metrics into a single quality score.
 * 
 * The composite score provides a balanced assessment of image quality by:
 * - PSNR: Measures pixel-level accuracy (objective measurement)
 * - SSIM: Measures perceptual similarity (how humans perceive quality)
 * 
 * The formula normalizes PSNR to a 0-1 scale and averages it with SSIM:
 *     composite = 0.5 × (normalized_PSNR) + 0.5 × SSIM
 * 
 * Where normalized_PSNR = min(PSNR / 50, 1.0)
 * This assumes PSNR values above 50 dB are essentially perfect quality.
 * 
 * @param psnr PSNR value in decibels (dB)
 *             - Typical range: 20-50 dB
 *             - Higher is better
 * @param ssim SSIM value (0 to 1)
 *             - 1.0 = identical images
 *             - 0.0 = completely different
 * @return double Composite score ranging from 0 to 1 (higher is better)
 */
double calculateCompositeScore(double psnr, double ssim) {
    // Handle invalid inputs
    if (psnr < 0 || ssim < 0) {
        std::cerr << "Error: Invalid PSNR or SSIM values!" << std::endl;
        return -1.0;
    }
    
    // Normalize PSNR to 0-1 scale
    // We use 50 dB as the reference point for "perfect" quality
    // PSNR values above 50 dB are clamped to 1.0
    double normalized_psnr = std::min(psnr / 50.0, 1.0);
    
    // Calculate weighted average of normalized PSNR and SSIM
    // Equal weights (0.5 each) give balanced importance to both metrics
    // You can adjust these weights based on your application:
    //   - More weight on PSNR: emphasizes pixel accuracy
    //   - More weight on SSIM: emphasizes perceptual quality
    double composite = 0.5 * normalized_psnr + 0.5 * ssim;
    
    return composite;
}