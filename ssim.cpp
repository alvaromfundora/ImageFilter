#include "image_quality.h"
#include <iostream>

/**
 * Structural Similarity Index (SSIM) Function
 * 
 * This function calculates the SSIM between two images, which measures
 * perceptual similarity based on luminance, contrast, and structure.
 * 
 * @param imageA - First input image (reference)
 * @param imageB - Second input image (comparison)
 * @return double - SSIM score ranging from -1 to 1 (typically 0 to 1)
 *                  where 1.0 indicates identical images
 *                  Returns -1.0 on error
 */
double computeSSIM(const cv::Mat& imageA, const cv::Mat& imageB) {
    
    // ============================================================
    // PART 1: INPUT VALIDATION
    // ============================================================
    
    // Ensure neither image is empty
    if (imageA.empty() || imageB.empty()) {
        std::cerr << "ERROR: One or both input images are empty!" << std::endl;
        return -1.0;
    }
    
    // Verify dimensions match between the two images
    // SSIM requires identical dimensions for pixel-wise comparison
    if (imageA.rows != imageB.rows || imageA.cols != imageB.cols) {
        std::cerr << "ERROR: Image dimensions do not match!" << std::endl;
        std::cerr << "Image A: " << imageA.cols << " x " << imageA.rows << std::endl;
        std::cerr << "Image B: " << imageB.cols << " x " << imageB.rows << std::endl;
        return -1.0;
    }
    
    // Verify both images have the same number of channels
    // (e.g., both grayscale or both color)
    if (imageA.type() != imageB.type()) {
        std::cerr << "ERROR: Image types do not match!" << std::endl;
        return -1.0;
    }
    
    
    // ============================================================
    // PART 2: DEFINE SSIM CONSTANTS
    // ============================================================
    
    // These constants stabilize the SSIM calculation when denominators
    // approach zero. Values are from Wang et al.'s original SSIM paper.
    // L = 255 (dynamic range for 8-bit images)
    // K1 = 0.01, K2 = 0.03 (small constants)
    const double C1 = (0.01 * 255) * (0.01 * 255);  // C1 = (K1*L)^2 = 6.5025
    const double C2 = (0.03 * 255) * (0.03 * 255);  // C2 = (K2*L)^2 = 58.5225
    
    
    // ============================================================
    // PART 3: CONVERT IMAGES TO FLOATING POINT
    // ============================================================
    
    // Convert from integer pixel values (0-255) to floating point
    // This prevents integer overflow and provides precision for calculations
    cv::Mat floatImageA, floatImageB;
    
    // CV_32F = 32-bit floating point data type
    imageA.convertTo(floatImageA, CV_32F);
    imageB.convertTo(floatImageB, CV_32F);
    
    
    // ============================================================
    // PART 4: CALCULATE PIXEL-WISE PRODUCTS
    // ============================================================
    
    // We need these products for variance and covariance calculations
    // mul() performs element-wise multiplication (each pixel multiplied independently)
    
    cv::Mat imageA_squared = floatImageA.mul(floatImageA);  // A^2: squaring image A
    cv::Mat imageB_squared = floatImageB.mul(floatImageB);  // B^2: squaring image B
    cv::Mat imageA_times_B = floatImageA.mul(floatImageB);  // A*B: product of A and B
    
    
    // ============================================================
    // PART 5: COMPUTE LOCAL MEANS USING GAUSSIAN BLUR
    // ============================================================
    
    // GaussianBlur creates a weighted average of neighboring pixels
    // This gives us the local mean (mu) for each pixel region
    // Parameters:
    //   - cv::Size(11, 11): 11x11 pixel window (standard for SSIM)
    //   - 1.5: sigma (standard deviation of Gaussian kernel)
    
    cv::Mat meanA, meanB;
    
    // Calculate local mean of image A at each pixel location
    cv::GaussianBlur(floatImageA, meanA, cv::Size(11, 11), 1.5);
    
    // Calculate local mean of image B at each pixel location
    cv::GaussianBlur(floatImageB, meanB, cv::Size(11, 11), 1.5);
    
    
    // ============================================================
    // PART 6: COMPUTE PRODUCTS OF MEANS
    // ============================================================
    
    // We need these for the SSIM formula calculations
    
    cv::Mat meanA_squared = meanA.mul(meanA);  // mu_A^2
    cv::Mat meanB_squared = meanB.mul(meanB);  // mu_B^2
    cv::Mat meanA_times_meanB = meanA.mul(meanB);  // mu_A * mu_B
    
    
    // ============================================================
    // PART 7: COMPUTE VARIANCE AND COVARIANCE
    // ============================================================
    
    // Variance formula: Var(X) = E[X^2] - (E[X])^2
    //   where E[X] is the expected value (mean) of X
    //
    // Covariance formula: Cov(X,Y) = E[XY] - E[X]*E[Y]
    
    cv::Mat varianceA, varianceB, covariance;
    
    // Variance of image A: sigma_A^2 = E[A^2] - (E[A])^2
    cv::GaussianBlur(imageA_squared, varianceA, cv::Size(11, 11), 1.5);  // E[A^2]
    varianceA = varianceA - meanA_squared;  // Subtract (E[A])^2
    
    // Variance of image B: sigma_B^2 = E[B^2] - (E[B])^2
    cv::GaussianBlur(imageB_squared, varianceB, cv::Size(11, 11), 1.5);  // E[B^2]
    varianceB = varianceB - meanB_squared;  // Subtract (E[B])^2
    
    // Covariance: sigma_AB = E[A*B] - E[A]*E[B]
    cv::GaussianBlur(imageA_times_B, covariance, cv::Size(11, 11), 1.5);  // E[A*B]
    covariance = covariance - meanA_times_meanB;  // Subtract E[A]*E[B]
    
    
    // ============================================================
    // PART 8: APPLY THE SSIM FORMULA
    // ============================================================
    
    // SSIM formula:
    // SSIM = [(2*mu_A*mu_B + C1) * (2*sigma_AB + C2)] /
    //        [(mu_A^2 + mu_B^2 + C1) * (sigma_A^2 + sigma_B^2 + C2)]
    //
    // This combines:
    //   - Luminance comparison (means)
    //   - Contrast comparison (variances)
    //   - Structure comparison (covariance)
    
    // --- Calculate Numerator ---
    
    // Luminance component: 2*mu_A*mu_B + C1
    cv::Mat luminance_numerator = 2.0 * meanA_times_meanB + C1;
    
    // Structure component: 2*sigma_AB + C2
    cv::Mat structure_numerator = 2.0 * covariance + C2;
    
    // Multiply luminance and structure components
    cv::Mat numerator = luminance_numerator.mul(structure_numerator);
    
    
    // --- Calculate Denominator ---
    
    // Luminance component: mu_A^2 + mu_B^2 + C1
    cv::Mat luminance_denominator = meanA_squared + meanB_squared + C1;
    
    // Contrast component: sigma_A^2 + sigma_B^2 + C2
    cv::Mat contrast_denominator = varianceA + varianceB + C2;
    
    // Multiply luminance and contrast components
    cv::Mat denominator = luminance_denominator.mul(contrast_denominator);
    
    
    // --- Compute SSIM Map ---
    
    // Divide numerator by denominator to get SSIM value at each pixel
    cv::Mat ssimMap;
    cv::divide(numerator, denominator, ssimMap);
    
    
    // ============================================================
    // PART 9: CALCULATE THE MEAN SSIM VALUE
    // ============================================================
    
    // Average all pixel-wise SSIM values to get a single metric
    // cv::mean() returns a Scalar with one value per channel
    cv::Scalar meanSSIM = cv::mean(ssimMap);
    
    // Extract the SSIM value from the first channel
    // For grayscale images, this is the only channel
    // For color images, you might want to average all channels
    double ssimValue = meanSSIM[0];
    
    
    // ============================================================
    // PART 10: RETURN THE RESULT
    // ============================================================
    
    // Return the final SSIM score
    // Values close to 1.0 indicate high similarity
    // Values close to 0.0 indicate low similarity
    return ssimValue;
}