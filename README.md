# ImageFilter
Guide For Using Software In Practical Mode: 
Upon running the program, the user is given a choice between running the testing mode or the practical mode, this section takes a step by step look at using the practical mode. To use the practical mode, the user must enter either “--practical” or “-p” onto the console to start the practical mode version. 
1.	Enter the name of the image file that the user is using. Make sure that the image file is in the same folder as the code that the user is using otherwise the code will not be able to access and process the image. 
2.	In case of any issues when loading the image, the message “ERROR: Could not load image: _____” will pop up and end the program. 
3.	Otherwise, the messages: “Loaded image: _____”, “Dimensions: ____”, and “Channels: ___” will appear. 
4.	After this, several other messages will appear as the Gaussian unmasking filter is applied to the image. If either of these modes fail, the messages “ERROR: Gaussian blur failed!” or “ERROR: Unsharp masking failed!” will be output to the console. 
5.	If everything goes smoothly, several messages describing the process will appear. 
6.	After, the blurred image that has undergone the gaussian filter and the final enhanced version are downloaded to the user’s computer as output_blurred.jpg and output_enhanced.jpg respectively. 
7.	With these images, the quality metrics PSNR (Peak Signal to Noise Ratio) and SSIM (Structural Similarity Index) are calculated. These measurements are a way of testing the fidelity of the new image to the original image to show that 
8.	If there are any issues with these, like the values going negative either “ERROR: PSNR calculation failed!” or “ERROR: SSIM calculation failed!” will pop up and cause the code to end. 
9.	The ranges for the PSNR range from 0 to over 40 where anything over 30 is good. 
10.	The ranges for the SSIM vary from 0 to 1 where anything above 0.8 is acceptable and above 0.9 is good. 
11.	Lastly, a summary of what has been done within the program is printed out to the console and the user is told about each of the scores used, along with a composite score to measure the image quality.
Please refer to the link if you need help installing OpenCV: [How to Install opencv in C++ on Linux? - GeeksforGeeks](https://www.geeksforgeeks.org/installation-guide/how-to-install-opencv-in-c-on-linux/)

Guide For Using Software In Tester Mode
The Tester mode of the Image Quality Improvement Software is designed to demonstrate the capability of the software to improve an image by comparing the compressed version of an original high quality image, and the improved version of the compressed image to the original, high quality version of said image. 
1. The code is compiled with the script ‘make’ followed by the return key.
2. It is then executed with the script ‘./image_enhancer -t’ (-t can be substituted with ‘-tester’), followed by the name of the original (high-quality) image and the name of the compressed image. It should look like this: ‘./image_enhancer -t original_image.jpg compressed_image.jpg’.
3. The tester mode handles a variety of edge cases, mostly involving the images passed into the function. If either image is empty, or if they two do not have the same dimensions, then the program will return -1. 
4. If the compressed image is empty, the message “ERROR: Could not load compressed image ”, followed by the name of the compressed image will be output to the terminal.
5. If the original high quality image is empty, the message “ERROR: Could not load clean image ”, followed by the name of the clean image will be output to the terminal. 
6. If the dimensions of the two images do not match, then the message “ERROR: Images must have the same dimensions!”, followed by the dimensions of each image on new lines.
7. If the PSNR or SSIM score returns a value below zero at any point in the program (should not be possible), then -1 is returned.
8. If one of the baseline calculations fails, the message “ERROR: Baseline ” followed by either SSIM or PSNR (depending on which one was below zero) “ calculation failed!” is output to the console.
9. If one of the enhanced calculations fails, the message “ERROR: Enhanced “ followed by PSNR or SSIM (depending on which one was below zero) “ calculation failed!” is output to the console.
10. On top of that, if the image is empty after it has been run through the unsharp masking filter or the Gaussian blur filter, then -1 is returned as well.
11. If the Gaussian blur fails, the message “ERROR: Gaussian Blur failed!” is output to the console.
12. If the Unsharp Masking fails, the message “ERROR: Unsharp Masking failed!” is output to the console.
13. If nothing fails, then the comparison metrics between the compressed and the clean image, the comparison metrics between the enhanced and the clean image, and the improvement metrics between both images will be output, along with whether the software was successful at enhancing the image quality.
Please refer to the link if you need help installing OpenCV: [How to Install opencv in C++ on Linux? - GeeksforGeeks](https://www.geeksforgeeks.org/installation-guide/how-to-install-opencv-in-c-on-linux/)
