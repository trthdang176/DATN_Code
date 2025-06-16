#include "12_MySD.h"

//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//================ Begin: LƯU DỮ LIỆU VÀO THẺ NHỚ SD CARD =================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
void SD_Card::KhoiTaoSD_Card(void) {
  // Thẻ SD Card dùng để lưu dữ liệu.
  // Chú ý: Khai báo sau các khai báo pinMode.
  //---------------------------------------------------------------
  // Khởi tạo SPI với các chân cụ thể
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
  // SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  if (!SD.begin(CS_PIN)) 
    {
      Serial.println("Khởi tạo thất bại!");
    } 
    else 
    {
      Serial.println("Khởi tạo SD Card thành công.");
    }
  //---------------------------------------------------------------
  //---------------------------------------------------------------
}
void SD_Card::WriteToFile(String Data) {
  myFile = SD.open("/ThongTinLichSuSac.txt", FILE_APPEND);
  if (myFile) 
  {
    myFile.println(Data);
    myFile.flush();  // Đảm bảo dữ liệu được ghi vào thẻ SD ngay lập tức
    myFile.seek(0);
    myFile.close();
    // Serial.println("Đã ghi lại dữ liệu thông tin sạc");
  } 
  else 
  {
    Serial.println("Lỗi mở file để ghi");
  } 
  
}

void SD_Card::GetDataFromFile(String* arr, int arrSize) {
  myFile = SD.open("/ThongTinLichSuSac.txt", FILE_READ);
  if (myFile) 
  {
    // Serial.println("Lấy dữ liệu từ file để hiển thị lên HMI:");
    // Đọc từng ký tự từ file và in ra màn hình
    int i = 0;
    while (myFile.available() && i < arrSize) 
    {
      arr[i] = myFile.readStringUntil('\n');
      i++;
    }
    myFile.close(); 
  } 
  else 
  {
    Serial.println("Lỗi mở file để đọc");
  }   
}
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//================ End: LƯU DỮ LIỆU VÀO THẺ NHỚ SD CARD ===================//
//MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
unsigned long SD_Card::SoDongTrongFile(void) {
  unsigned long SoDong = 0;
  myFile = SD.open("/ThongTinLichSuSac.txt", FILE_READ);
  if (myFile)
  {
    while (myFile.available())
    {
      char c = myFile.read();
      if (c == '\n')
      {
        SoDong++;
      }
    }
    myFile.seek(0);
    myFile.close();
  }
  else {
    Serial.println("Lỗi mở file để đếm");
  }
  
  return SoDong;  
  
}

void SD_Card::DeleteFile(void) {
  if (SD.exists("/ThongTinLichSuSac.txt"))
  {
    if (SD.remove("/ThongTinLichSuSac.txt"))
    {
      Serial.println("Đã xóa file thành công!");
    }
    else {
      Serial.println("Không thể xóa file.");
    }
  } 
  else {
    Serial.println("File không tồn tại.");
  }
}