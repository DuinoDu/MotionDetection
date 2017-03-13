#ifndef INTERFACE_H
#define INTERFACE_H

#include <vector>
#include <string>
//typedef void (_stdcall *MyFun)(std::vector<std::vector<int>>& result, void* context);
typedef void(_stdcall *MyFun)(int* px, int* py, size_t size, void* context);

typedef void* (*_createMotionDetector)();
typedef void (*_deleteMotionDetector) (void * detector);
typedef void (*_setRoomSize) (void* detector, int width, int height);
typedef void (*_setMotionSize) (void* _detector, float standup, float sitdown);
typedef void (*_setStudentRegion) (void *detector, int topleft_x, int topleft_y, int topright_x, int topright_y,
                                int bottomleft_x, int bottomleft_y, int bottomright_x, int bottomright_y);
typedef void (*_setCaffemodelPath) (void* detector, char* str);
typedef void (*_setCallback) (void* detector, MyFun functionName, void* context);
typedef void (*_setShowWindows) (void* detector, bool stage1, bool stage2);
typedef void (*_setFrame) (void* _detector, unsigned char* data, int height, int width, int length);

#endif // INTERFACE_H
