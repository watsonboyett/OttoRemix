
#include "SdFatHelper.h"

static Adafruit_FlashTransport_QSPI flashTransport(PIN_QSPI_SCK, PIN_QSPI_CS,
                                                   PIN_QSPI_IO0, PIN_QSPI_IO1,
                                                   PIN_QSPI_IO2, PIN_QSPI_IO3);

static Adafruit_SPIFlash flash(&flashTransport);
static FatFileSystem fatfs;

SdFatHelper::SdFatHelper()
{

}

bool SdFatHelper::begin(FatFileSystem *filesys)
{
    if (filesys) {
        theFS = filesys;
    } 
    else {
        if (!flash.begin()) {
            return false;
        }
        Serial.print("JEDEC ID: ");
        Serial.println(flash.getJEDECID(), HEX);
        Serial.print("Flash size: ");
        Serial.println(flash.size());

        if (!fatfs.begin(&flash)) {
            Serial.println("Error, failed to mount newly formatted filesystem!");
            Serial.println("Was it formatted with the fatfs_format example?");
            return false;
        }
        theFS = &fatfs;
    }

    Serial.println("Mounted filesystem!");

    return true;
}

void SdFatHelper::printFS()
{
    File root;
    char file[80];
    root = theFS->open("/");
    while (1) {
        File entry = root.openNextFile();
        if (!entry) {
            break; // no more files
        }
        entry.getName(file, 80);
        Serial.print("\t");
        Serial.print(file);
        if (entry.isDirectory()) {
            Serial.println("/");
        } 
        else {
            // files have sizes, directories do not
            Serial.print(" : ");
            Serial.print(entry.size(), DEC);
            Serial.println(" bytes");
        }
        entry.close();
    }
}

bool SdFatHelper::open(const char *filename, File &file, oflag_t oflag)
{
  if (!theFS)
    return false;

  file = theFS->open(filename, oflag);
  if (!file) {
    Serial.println(F("Failed to open file"));
    return false;
  }

  return true;
}

bool SdFatHelper::open_new(const char *filename, File &file)
{
    return open(filename, file, O_WRITE | O_CREAT | O_TRUNC);
}

bool SdFatHelper::open_append(const char *filename, File &file)
{
    return open(filename, file, O_WRITE | O_APPEND);
}

bool SdFatHelper::open_read(const char *filename, File &file)
{
    return open(filename, file, O_READ);
}

bool SdFatHelper::close(File &file)
{
    // Close the file (File's destructor doesn't close the file)
    return file.close();
}

bool SdFatHelper::exists(const char *filepath)
{
    if (!theFS)
        return false;

    return theFS->exists(filepath);
}
