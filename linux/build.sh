cp -r ../src/res .
clang++ ../src/bg.cpp -I/usr/include/SDL2 -I/usr/include/GL -D_REENTRANT \
-g -L/usr/lib -lSDL2 -lGLEW -lGLU -lGL -obumblegumpers