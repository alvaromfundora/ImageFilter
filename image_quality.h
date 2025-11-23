#ifndef IMAGE_QUALITY_H
#define IMAGE_QUALITY_H

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

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
double calculatePSNR(const cv::Mat& original, const cv::Mat& compressed);

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
double computeSSIM(const cv::Mat& imageA, const cv::Mat& imageB);

/**
 * Apply Gaussian blur to an image
 * 
 * This function reduces noise in the image by applying a Gaussian filter.
 * The Gaussian filter performs a weighted average of neighboring pixels.
 * 
 * @param input The input image to blur
 * @param kernelSize The size of the Gaussian kernel (must be odd, e.g., 5, 7, 11)
 * @param sigma The standard deviation of the Gaussian distribution
 * @return cv::Mat The blurred output image
 */
cv::Mat applyGaussianBlur(const cv::Mat& input, int kernelSize, double sigma);

/**
 * Apply unsharp masking filter to sharpen an image
 * 
 * Unsharp masking enhances edges and fine details by subtracting a blurred
 * version from the original and adding the difference back with amplification.
 * 
 * Formula: output = original + amount * (original - blurred)
 * 
 * @param original The original input image
 * @param blurred The Gaussian-blurred version of the original
 * @param amount Sharpening strength (typical values: 0.5 to 2.5)
 * @param threshold Minimum difference for sharpening (reduces noise amplification)
 * @return cv::Mat The sharpened output image
 */
cv::Mat applyUnsharpMask(const cv::Mat& original, const cv::Mat& blurred, 
                         double amount = 1.5, double threshold = 0.0);

/**
 * Calculate composite quality score
 * 
 * Combines PSNR and SSIM into a single quality metric.
 * The composite score is a weighted average of normalized PSNR and SSIM.
 * 
 * @param psnr PSNR value in dB
 * @param ssim SSIM value (0 to 1)
 * @return double Composite score (higher is better)
 */
double calculateCompositeScore(double psnr, double ssim);

#endif // IMAGE_QUALITY_H