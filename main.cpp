#include <iostream>
#include "E57SimpleData.h"
#include "E57SimpleReader.h"
using namespace std;
using namespace e57;
int main(int argc, char **argv){

    //truct E57_DLL ColorLimits;
    Reader reader("point cloud filepath");
    // StructureNode root = imf.root();
    cout<<"Total contain images: "<<reader.GetImage2DCount()<<endl;
    //Iterate to read the images and save them
    for (int64_t img = 0; img < reader.GetImage2DCount(); ++img){
      Image2D img_header;
      reader.ReadImage2D(img, img_header);
      if (img_header.visualReferenceRepresentation.jpegImageSize != 0){
       ustring savePath = "/filepath/"+ std::to_string(img)+ ".jpg";
       int64_t size = img_header.visualReferenceRepresentation.jpegImageSize;
       uint8_t* imageBuffer = new uint8_t[size];
       reader.ReadImage2DData(img, E57_VISUAL, E57_JPEG_IMAGE, imageBuffer, 0, size);
       reader.SaveImage2DData(imageBuffer ,size, savePath);
      } 
      else if (img_header.visualReferenceRepresentation.pngImageSize != 0){
       ustring savePath = "/filepath/"+ std::to_string(img)+ ".png";
       int64_t size = img_header.visualReferenceRepresentation.pngImageSize;
       uint8_t* imageBuffer = new uint8_t[size];
       reader.ReadImage2DData(img, E57_VISUAL, E57_PNG_IMAGE, imageBuffer, 0, size);
       reader.SaveImage2DData(imageBuffer, size, savePath);
      }
			else if (img_header.pinholeRepresentation.jpegImageSize != 0){
       ustring savePath = "/filepath/"+ std::to_string(img)+ ".jpg";
       int64_t size = img_header.pinholeRepresentation.jpegImageSize;
       uint8_t* imageBuffer = new uint8_t[size];
       reader.ReadImage2DData(img, E57_PINHOLE, E57_JPEG_IMAGE, imageBuffer, 0, size);
       reader.SaveImage2DData(imageBuffer, size, savePath);
      }
      else if (img_header.pinholeRepresentation.pngImageSize != 0){
       ustring savePath = "/filepath/"+ std::to_string(img)+ ".png";
       int64_t size = img_header.pinholeRepresentation.pngImageSize;
       uint8_t* imageBuffer = new uint8_t[size];
       reader.ReadImage2DData(img, E57_PINHOLE, E57_PNG_IMAGE, imageBuffer, 0, size);
       reader.SaveImage2DData(imageBuffer, size, savePath);
      }
			else if (img_header.sphericalRepresentation.jpegImageSize != 0){
       ustring savePath = "/filepath/"+ std::to_string(img)+ ".jpg";
       int64_t size = img_header.sphericalRepresentation.jpegImageSize;
       uint8_t* imageBuffer = new uint8_t[size];
       reader.ReadImage2DData(img, E57_SPHERICAL, E57_JPEG_IMAGE, imageBuffer, 0, size);
       reader.SaveImage2DData(imageBuffer, size, savePath);
      } 
      else if (img_header.sphericalRepresentation.pngImageSize != 0){
       ustring savePath = "/filepath/"+ std::to_string(img)+ ".png";
       int64_t size = img_header.sphericalRepresentation.pngImageSize;
       uint8_t* imageBuffer = new uint8_t[size];
       reader.ReadImage2DData(img, E57_SPHERICAL, E57_PNG_IMAGE, imageBuffer, 0, size);
       reader.SaveImage2DData(imageBuffer, size, savePath);
      }
			else if (img_header.cylindricalRepresentation.jpegImageSize != 0){
       ustring savePath = "/filepath/"+ std::to_string(img)+ ".jpg";
       int64_t size = img_header.cylindricalRepresentation.jpegImageSize;
       uint8_t* imageBuffer = new uint8_t[size];
       reader.ReadImage2DData(img, E57_CYLINDRICAL, E57_JPEG_IMAGE, imageBuffer, 0, size);
       reader.SaveImage2DData(imageBuffer, size, savePath);
      }
      else if (img_header.cylindricalRepresentation.pngImageSize != 0){
       ustring savePath = "/filepath/"+ std::to_string(img)+ ".png";
       int64_t size = img_header.cylindricalRepresentation.jpegImageSize;
       uint8_t* imageBuffer = new uint8_t[size];
       reader.ReadImage2DData(img, E57_CYLINDRICAL, E57_PNG_IMAGE, imageBuffer, 0, size);
       reader.SaveImage2DData(imageBuffer, size, savePath);

      } 
    }
    
}