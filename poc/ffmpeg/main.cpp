#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <opencv2/opencv.hpp>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

// Forward declaration for the save_frame_as_image function
void save_frame_as_image(AVFrame *frame, const char *filename);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s input.h264 output_folder\n", argv[0]);
        return 1;
    }

    const char *inputFileName = argv[1];
    const char *outputFolder = argv[2];

    av_register_all();

    // Open the input file
    AVFormatContext *formatContext = avformat_alloc_context();
    if (avformat_open_input(&formatContext, inputFileName, NULL, NULL) != 0) {
        fprintf(stderr, "Couldn't open input file.\n");
        return 1;
    }

    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        fprintf(stderr, "Couldn't find stream information.\n");
        return 1;
    }

    // Find the first video stream
    int videoStream = -1;
    for (int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }

    if (videoStream == -1) {
        fprintf(stderr, "No video stream found.\n");
        return 1;
    }

    // Get a pointer to the codec context for the video stream
    AVCodecContext *codecContext = avcodec_alloc_context3(NULL);
    if (codecContext == NULL) {
        fprintf(stderr, "Couldn't allocate codec context.\n");
        return 1;
    }

    if (avcodec_parameters_to_context(codecContext, formatContext->streams[videoStream]->codecpar) < 0) {
        fprintf(stderr, "Couldn't copy codec parameters to codec context.\n");
        return 1;
    }

    // Find the decoder for the video stream
    AVCodec *codec = avcodec_find_decoder(codecContext->codec_id);
    if (codec == NULL) {
        fprintf(stderr, "Codec not found.\n");
        return 1;
    }

    // Open codec
    if (avcodec_open2(codecContext, codec, NULL) < 0) {
        fprintf(stderr, "Couldn't open codec.\n");
        return 1;
    }

    // Initialize a packet for reading video frames
    AVPacket packet;
    av_init_packet(&packet);

    // Initialize the frame
    AVFrame *frame = av_frame_alloc();
    if (frame == NULL) {
        fprintf(stderr, "Couldn't allocate frame.\n");
        return 1;
    }

    // Read frames from the input file and decode
    int frameNumber = 0;
    while (av_read_frame(formatContext, &packet) >= 0) {
        if (packet.stream_index == videoStream) {
            // Send packet to the decoder
            if (avcodec_send_packet(codecContext, &packet) < 0) {
                fprintf(stderr, "Error sending a packet to the decoder.\n");
                return 1;
            }

            // Receive decoded frame from the decoder
            while (avcodec_receive_frame(codecContext, frame) == 0) {
                // Save the frame as an image
                char filename[256];
                snprintf(filename, sizeof(filename), "%s/frame%d.png", outputFolder, frameNumber);
                save_frame_as_image(frame, filename);
                frameNumber++;
            }
        }

        av_packet_unref(&packet);
    }

    // Cleanup
    av_frame_free(&frame);
    avcodec_close(codecContext);
    avformat_close_input(&formatContext);

    return 0;
}

/*
void save_frame_as_image(AVFrame* frame, const char* filename) {
    // Create an OpenCV Mat from the AVFrame data
    cv::Mat cvFrame(frame->height, frame->width, CV_8UC3, frame->data[0]);

    // Convert the BGR frame to RGB format (if necessary)
    cv::cvtColor(cvFrame, cvFrame, cv::COLOR_BGR2RGB);

    // Save the frame as an image using OpenCV
    bool success = cv::imwrite(filename, cvFrame);

    if (!success) {
        fprintf(stderr, "Failed to save frame as an image: %s\n", filename);
        return;
    }

    printf("Saved frame as an image: %s\n", filename);
}
*/


void save_frame_as_image(AVFrame *frame, const char *filename) {
    // Determine required buffer size and allocate the buffer
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, frame->width, frame->height, 1);
    uint8_t *buffer = static_cast<uint8_t*>(av_malloc(numBytes * sizeof(uint8_t)));


    // Allocate an AVFrame for the RGB image
    AVFrame *rgbFrame = av_frame_alloc();
    if (rgbFrame == NULL) {
        fprintf(stderr, "Couldn't allocate RGB frame.\n");
        av_free(buffer);
        return;
    }

    // Allocate and fill the RGB frame's data pointers and linesizes
    if (av_image_alloc(rgbFrame->data, rgbFrame->linesize, frame->width, frame->height, AV_PIX_FMT_RGB24, 1) < 0) {
        fprintf(stderr, "Couldn't allocate RGB frame data.\n");
        av_frame_free(&rgbFrame);
        av_free(buffer);
        return;
    }

    // Create the sws context for RGB conversion
    struct SwsContext *swsContext = sws_getContext(frame->width, frame->height, (AVPixelFormat)frame->format,
        frame->width, frame->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);

    if (!swsContext) {
        fprintf(stderr, "Couldn't initialize SwsContext.\n");
        av_freep(&rgbFrame->data[0]);
        av_frame_free(&rgbFrame);
        av_free(buffer);
        return;
    }

    // Convert the frame to RGB format
    sws_scale(swsContext, frame->data, frame->linesize, 0, frame->height,
        rgbFrame->data, rgbFrame->linesize);

    // Save the RGB frame as a PNG image using an external library or your preferred method
    // You can use external libraries like libpng or libjpeg to save the frame as an image
    // Here, we are not including the image-saving code; you should add it based on your needs.
     // Create an OpenCV Mat from the AVFrame data
    cv::Mat cvFrame(frame->height, frame->width, CV_8UC3, rgbFrame->data[0]);


    // Save the frame as an image using OpenCV
    bool success = cv::imwrite(filename, cvFrame);

    if (!success) {
        fprintf(stderr, "Failed to save frame as an image: %s\n", filename);
        return;
    }
    printf("Saved frame as an image: %s\n", filename);  
    // Cleanup
    av_freep(&rgbFrame->data[0]);
    av_frame_free(&rgbFrame);
    sws_freeContext(swsContext);
    av_free(buffer);
}
