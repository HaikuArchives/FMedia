# FMedia

Originally from <https://f3c.com/software/FMedia/>.

 * **libFMedia**:  an abstraction layer for writing MediaKit add-ons, to make the development of MediaKit-addons much easier.
 * **FDifference**: MediaKit add-on for motion detection. FDifference compares the red channel (or blue?, unsure) of two consecutive frames of the video stream and stores the absolute difference in the alpha channel. The overall difference value is stored in the user_data field of the buffer header.
 * **FCrop**: MediaKit add-on to crop video to some smaller size.
