#pragma once

#include <SdFat.h>
#include <Adafruit_SPIFlash.h>

/**!  @brief Class for managing SDFat-compatible storage **/
class SdFatHelper {
public:
    SdFatHelper();
    bool begin(FatFileSystem *filesys = NULL);
    void printFS();
    bool open(const char *filename, File &file, oflag_t oflag);
    bool open_new(const char *filename, File &file);
    bool open_append(const char *filename, File &file);
    bool open_read(const char *filename, File &file);
    bool close(File &file);
    bool exists(const char *filename);

private:
    FatFileSystem *theFS = NULL;
};

