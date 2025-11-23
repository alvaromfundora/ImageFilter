#include "image_quality.h"
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <iomanip>
#include <string>

/**
 * IMAGE QUALITY ENHANCEMENT AND EVALUATION PROGRAM
 * 
 * This program has two modes:
 * 
 * TESTING MODE:
 *   - Takes a clean reference image and a compressed/degraded version
 *   - Compares compressed to clean (baseline quality)
 *   - Enhances the compressed image
 *   - Compares enhanced to clean (enhanced quality)
 *   - Demonstrates that enhancement improves quality
 * 
 * PRACTICAL MODE:
 *   - Takes only a compressed/degraded image
 *   - Enhances the image
 *   - Compares enhanced to original compressed
 *   - Outputs the enhanced image
 */

void printUsage(const char* programName) {
    std::cout << "Usage:" << std::endl;
    std::cout << "  TESTING MODE:   " << programName << " --test <clean_image> <compressed_image>" << std::endl;
    std::cout << "  PRACTICAL MODE: " << programName << " --practical <compressed_image>" << std::endl;
    std::cout << std::endl;
    std::cout << "Modes:" << std::endl;
    std::cout << "  --test      : Compare compressed vs clean, then enhanced vs clean" << std::endl;
    std::cout << "  --practical : Enhance image and compare to original compressed" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " --test original.jpg compressed.jpg" << std::endl;
    std::cout << "  " << programName << " --practical noisy_image.jpg" << std::endl;
}

/**
 * TESTING MODE
 * 
 * Evaluates the enhancement algorithm by comparing against a clean reference.
 * This mode proves that the enhancement improves image quality.
 */
int runTestingMode(const std::string& cleanImagePath, const std::string& compressedImagePath) {
    std::cout << "========================================" << std::endl;
    std::cout << "TESTING MODE" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    // ================================================================
    // STEP 1: LOAD BOTH IMAGES
    // ================================================================
    
    std::cout << "Loading images..." << std::endl;
    
    // Load the clean reference image
    cv::Mat cleanImage = cv::imread(cleanImagePath, cv::IMREAD_COLOR);
    if (cleanImage.empty()) {
        std::cerr << "ERROR: Could not load clean image: " << cleanImagePath << std::endl;
        return -1;
    }
    std::cout << "✓ Loaded clean reference image: " << cleanImagePath << std::endl;
    std::cout << "  Dimensions: " << cleanImage.cols << " x " << cleanImage.rows << std::endl;
    
    // Load the compressed/degraded image
    cv::Mat compressedImage = cv::imread(compressedImagePath, cv::IMREAD_COLOR);
    if (compressedImage.empty()) {
        std::cerr << "ERROR: Could not load compressed image: " << compressedImagePath << std::endl;
        return -1;
    }
    std::cout << "✓ Loaded compressed image: " << compressedImagePath << std::endl;
    std::cout << "  Dimensions: " << compressedImage.cols << " x " << compressedImage.rows << std::endl << std::endl;
    
    // Verify images have the same dimensions
    if (cleanImage.size() != compressedImage.size()) {
        std::cerr << "ERROR: Images must have the same dimensions!" << std::endl;
        std::cerr << "  Clean: " << cleanImage.cols << "x" << cleanImage.rows << std::endl;
        std::cerr << "  Compressed: " << compressedImage.cols << "x" << compressedImage.rows << std::endl;
        return -1;
    }
    
    
    // ================================================================
    // STEP 2: BASELINE QUALITY (Compressed vs Clean)
    // ================================================================
    
    std::cout << "========================================" << std::endl;
    std::cout << "BASELINE QUALITY ASSESSMENT" << std::endl;
    std::cout << "Comparing: Compressed vs Clean Reference" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    std::cout << "Calculating baseline quality metrics..." << std::endl;
    
    // Calculate PSNR between compressed and clean
    std::cout << "  Computing PSNR..." << std::endl;
    double baselinePSNR = calculatePSNR(cleanImage, compressedImage);
    if (baselinePSNR < 0) {
        std::cerr << "ERROR: Baseline PSNR calculation failed!" << std::endl;
        return -1;
    }
    
    // Calculate SSIM between compressed and clean
    std::cout << "  Computing SSIM..." << std::endl;
    double baselineSSIM = computeSSIM(cleanImage, compressedImage);
    if (baselineSSIM < 0) {
        std::cerr << "ERROR: Baseline SSIM calculation failed!" << std::endl;
        return -1;
    }
    
    // Calculate baseline composite score
    double baselineComposite = calculateCompositeScore(baselinePSNR, baselineSSIM);
    
    std::cout << "✓ Baseline quality calculated!" << std::endl << std::endl;
    
    
    // ================================================================
    // STEP 3: ENHANCE THE COMPRESSED IMAGE
    // ================================================================
    
    std::cout << "========================================" << std::endl;
    std::cout << "IMAGE ENHANCEMENT" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    std::cout << "Applying enhancement filters to compressed image..." << std::endl;
    
    // Apply Gaussian Blur for noise reduction
    std::cout << "  [1/2] Applying Gaussian blur (noise reduction)..." << std::endl;
    int gaussianKernelSize = 5;
    double gaussianSigma = 1.0;
    cv::Mat blurredImage = applyGaussianBlur(compressedImage, gaussianKernelSize, gaussianSigma);
    
    if (blurredImage.empty()) {
        std::cerr << "ERROR: Gaussian blur failed!" << std::endl;
        return -1;
    }
    
    // Apply Unsharp Masking for sharpness enhancement
    std::cout << "  [2/2] Applying unsharp mask (sharpness enhancement)..." << std::endl;
    double sharpenAmount = 1.5;
    double sharpenThreshold = 0.0;
    cv::Mat enhancedImage = applyUnsharpMask(compressedImage, blurredImage, 
                                             sharpenAmount, sharpenThreshold);
    
    if (enhancedImage.empty()) {
        std::cerr << "ERROR: Unsharp masking failed!" << std::endl;
        return -1;
    }
    
    std::cout << "✓ Enhancement complete!" << std::endl << std::endl;
    
    // Save the enhanced image
    cv::imwrite("output_enhanced.jpg", enhancedImage);
    std::cout << "✓ Saved enhanced image: output_enhanced.jpg" << std::endl << std::endl;
    
    
    // ================================================================
    // STEP 4: ENHANCED QUALITY (Enhanced vs Clean)
    // ================================================================
    
    std::cout << "========================================" << std::endl;
    std::cout << "ENHANCED QUALITY ASSESSMENT" << std::endl;
    std::cout << "Comparing: Enhanced vs Clean Reference" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    std::cout << "Calculating enhanced quality metrics..." << std::endl;
    
    // Calculate PSNR between enhanced and clean
    std::cout << "  Computing PSNR..." << std::endl;
    double enhancedPSNR = calculatePSNR(cleanImage, enhancedImage);
    if (enhancedPSNR < 0) {
        std::cerr << "ERROR: Enhanced PSNR calculation failed!" << std::endl;
        return -1;
    }
    
    // Calculate SSIM between enhanced and clean
    std::cout << "  Computing SSIM..." << std::endl;
    double enhancedSSIM = computeSSIM(cleanImage, enhancedImage);
    if (enhancedSSIM < 0) {
        std::cerr << "ERROR: Enhanced SSIM calculation failed!" << std::endl;
        return -1;
    }
    
    // Calculate enhanced composite score
    double enhancedComposite = calculateCompositeScore(enhancedPSNR, enhancedSSIM);
    
    std::cout << "✓ Enhanced quality calculated!" << std::endl << std::endl;
    
    
    // ================================================================
    // STEP 5: DISPLAY RESULTS AND COMPARISON
    // ================================================================
    
    std::cout << std::fixed << std::setprecision(4);
    
    std::cout << "========================================" << std::endl;
    std::cout << "RESULTS COMPARISON" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    std::cout << "BASELINE (Compressed vs Clean):" << std::endl;
    std::cout << "  PSNR:            " << baselinePSNR << " dB" << std::endl;
    std::cout << "  SSIM:            " << baselineSSIM << std::endl;
    std::cout << "  Composite Score: " << baselineComposite << std::endl << std::endl;
    
    std::cout << "ENHANCED (Enhanced vs Clean):" << std::endl;
    std::cout << "  PSNR:            " << enhancedPSNR << " dB" << std::endl;
    std::cout << "  SSIM:            " << enhancedSSIM << std::endl;
    std::cout << "  Composite Score: " << enhancedComposite << std::endl << std::endl;
    
    std::cout << "IMPROVEMENT:" << std::endl;
    double psnrImprovement = enhancedPSNR - baselinePSNR;
    double ssimImprovement = enhancedSSIM - baselineSSIM;
    double compositeImprovement = enhancedComposite - baselineComposite;
    
    std::cout << "  PSNR Improvement:      " << std::showpos << psnrImprovement << " dB" << std::noshowpos << std::endl;
    std::cout << "  SSIM Improvement:      " << std::showpos << ssimImprovement << std::noshowpos << std::endl;
    std::cout << "  Composite Improvement: " << std::showpos << compositeImprovement << std::noshowpos << std::endl << std::endl;
    
    // Determine test result
    std::cout << "========================================" << std::endl;
    std::cout << "TEST RESULT: ";
    if (enhancedComposite > baselineComposite) {
        std::cout << "✓ PASS" << std::endl;
        std::cout << "Enhancement successfully improved image quality!" << std::endl;
    } else {
        std::cout << "✗ FAIL" << std::endl;
        std::cout << "Enhancement did not improve image quality." << std::endl;
    }
    std::cout << "========================================" << std::endl;
    
    return 0;
}

/**
 * PRACTICAL MODE
 * 
 * Enhances a compressed/degraded image and compares the result
 * to the original compressed version.
 */
int runPracticalMode(const std::string& compressedImagePath) {
    std::cout << "========================================" << std::endl;
    std::cout << "PRACTICAL MODE" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    // ================================================================
    // STEP 1: LOAD THE COMPRESSED IMAGE
    // ================================================================
    
    std::cout << "Loading image..." << std::endl;
    
    cv::Mat compressedImage = cv::imread(compressedImagePath, cv::IMREAD_COLOR);
    if (compressedImage.empty()) {
        std::cerr << "ERROR: Could not load image: " << compressedImagePath << std::endl;
        return -1;
    }
    
    std::cout << "✓ Loaded image: " << compressedImagePath << std::endl;
    std::cout << "  Dimensions: " << compressedImage.cols << " x " << compressedImage.rows << std::endl;
    std::cout << "  Channels: " << compressedImage.channels() << std::endl << std::endl;
    
    
    // ================================================================
    // STEP 2: ENHANCE THE IMAGE
    // ================================================================
    
    std::cout << "========================================" << std::endl;
    std::cout << "IMAGE ENHANCEMENT" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    std::cout << "Applying enhancement filters..." << std::endl;
    
    // Apply Gaussian Blur for noise reduction
    std::cout << "  [1/2] Applying Gaussian blur (noise reduction)..." << std::endl;
    int gaussianKernelSize = 5;
    double gaussianSigma = 1.0;
    cv::Mat blurredImage = applyGaussianBlur(compressedImage, gaussianKernelSize, gaussianSigma);
    
    if (blurredImage.empty()) {
        std::cerr << "ERROR: Gaussian blur failed!" << std::endl;
        return -1;
    }
    
    // Apply Unsharp Masking for sharpness enhancement
    std::cout << "  [2/2] Applying unsharp mask (sharpness enhancement)..." << std::endl;
    double sharpenAmount = 1.5;
    double sharpenThreshold = 0.0;
    cv::Mat enhancedImage = applyUnsharpMask(compressedImage, blurredImage, 
                                             sharpenAmount, sharpenThreshold);
    
    if (enhancedImage.empty()) {
        std::cerr << "ERROR: Unsharp masking failed!" << std::endl;
        return -1;
    }
    
    std::cout << "✓ Enhancement complete!" << std::endl << std::endl;
    
    // Save processed images
    cv::imwrite("output_blurred.jpg", blurredImage);
    cv::imwrite("output_enhanced.jpg", enhancedImage);
    std::cout << "✓ Saved processed images:" << std::endl;
    std::cout << "  - output_blurred.jpg (after Gaussian blur)" << std::endl;
    std::cout << "  - output_enhanced.jpg (final enhanced image)" << std::endl << std::endl;
    
    
    // ================================================================
    // STEP 3: COMPARE ENHANCED TO ORIGINAL
    // ================================================================
    
    std::cout << "========================================" << std::endl;
    std::cout << "QUALITY EVALUATION" << std::endl;
    std::cout << "Comparing: Enhanced vs Original Compressed" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    std::cout << "Calculating quality metrics..." << std::endl;
    
    // Calculate PSNR
    std::cout << "  Computing PSNR..." << std::endl;
    double psnr = calculatePSNR(compressedImage, enhancedImage);
    if (psnr < 0) {
        std::cerr << "ERROR: PSNR calculation failed!" << std::endl;
        return -1;
    }
    
    // Calculate SSIM
    std::cout << "  Computing SSIM..." << std::endl;
    double ssim = computeSSIM(compressedImage, enhancedImage);
    if (ssim < 0) {
        std::cerr << "ERROR: SSIM calculation failed!" << std::endl;
        return -1;
    }
    
    // Calculate composite score
    double compositeScore = calculateCompositeScore(psnr, ssim);
    
    std::cout << "✓ Quality metrics calculated!" << std::endl << std::endl;
    
    
    // ================================================================
    // STEP 4: DISPLAY RESULTS
    // ================================================================
    
    std::cout << std::fixed << std::setprecision(4);
    
    std::cout << "========================================" << std::endl;
    std::cout << "RESULTS" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    std::cout << "Quality Metrics (Enhanced vs Original):" << std::endl;
    std::cout << "  PSNR:            " << psnr << " dB" << std::endl;
    std::cout << "  SSIM:            " << ssim << std::endl;
    std::cout << "  Composite Score: " << compositeScore << std::endl << std::endl;
    
    // Interpret the results
    std::cout << "Quality Assessment:" << std::endl;
    if (psnr > 40) {
        std::cout << "  PSNR Rating: Excellent (> 40 dB)" << std::endl;
    } else if (psnr > 30) {
        std::cout << "  PSNR Rating: Good (30-40 dB)" << std::endl;
    } else if (psnr > 20) {
        std::cout << "  PSNR Rating: Acceptable (20-30 dB)" << std::endl;
    } else {
        std::cout << "  PSNR Rating: Poor (< 20 dB)" << std::endl;
    }
    
    if (ssim > 0.95) {
        std::cout << "  SSIM Rating: Excellent (> 0.95)" << std::endl;
    } else if (ssim > 0.90) {
        std::cout << "  SSIM Rating: Good (0.90-0.95)" << std::endl;
    } else if (ssim > 0.80) {
        std::cout << "  SSIM Rating: Acceptable (0.80-0.90)" << std::endl;
    } else {
        std::cout << "  SSIM Rating: Poor (< 0.80)" << std::endl;
    }
    
    std::cout << std::endl;
    
    std::cout << "========================================" << std::endl;
    std::cout << "Filter Parameters Used:" << std::endl;
    std::cout << "  Gaussian Blur:" << std::endl;
    std::cout << "    - Kernel Size: " << gaussianKernelSize << "x" << gaussianKernelSize << std::endl;
    std::cout << "    - Sigma: " << gaussianSigma << std::endl;
    std::cout << "  Unsharp Mask:" << std::endl;
    std::cout << "    - Amount: " << sharpenAmount << std::endl;
    std::cout << "    - Threshold: " << sharpenThreshold << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    std::cout << "✓ Enhanced image saved as: output_enhanced.jpg" << std::endl;
    std::cout << std::endl;
    
    std::cout << "========================================" << std::endl;
    std::cout << "PROGRAM COMPLETE" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}

/**
 * Main function - handles mode selection and argument parsing
 */
int main(int argc, char** argv) {
    // Check if sufficient arguments provided
    if (argc < 3) {
        printUsage(argv[0]);
        return -1;
    }
    
    std::string mode = argv[1];
    
    // TESTING MODE
    if (mode == "--test" || mode == "-t") {
        if (argc != 4) {
            std::cerr << "ERROR: Testing mode requires 2 image paths!" << std::endl << std::endl;
            printUsage(argv[0]);
            return -1;
        }
        
        std::string cleanImagePath = argv[2];
        std::string compressedImagePath = argv[3];
        
        return runTestingMode(cleanImagePath, compressedImagePath);
    }
    // PRACTICAL MODE
    else if (mode == "--practical" || mode == "-p") {
        if (argc != 3) {
            std::cerr << "ERROR: Practical mode requires 1 image path!" << std::endl << std::endl;
            printUsage(argv[0]);
            return -1;
        }
        
        std::string compressedImagePath = argv[2];
        
        return runPracticalMode(compressedImagePath);
    }
    // INVALID MODE
    else {
        std::cerr << "ERROR: Invalid mode '" << mode << "'" << std::endl << std::endl;
        printUsage(argv[0]);
        return -1;
    }
    
    return 0;
}