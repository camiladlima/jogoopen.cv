g++ facedetect_simple.cpp `pkg-config --cflags opencv4` `pkg-config --libs --static opencv4` -o facedetect_simple

g++ facedetect_extra.cpp `pkg-config --cflags opencv4` `pkg-config --libs --static opencv4` -o facedetect_extra
g++ test_cam.cpp -o test_cam `pkg-config --cflags --libs opencv4` -o teste_cam
