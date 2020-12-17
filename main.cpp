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
    // if (!root.isDefined("/data3D"))
    // {
    //   cout << "File doesn't contain 3D images" << endl;
    //   return 0;
    // }
    // Node n = root.get("/data3D");
    // if (n.type() != E57_VECTOR)
    // {
    //   cout << "bad file" << endl;
    //   return 0;
    // }

    // /// The node is a vector so we can safely get a VectorNode handle to it.
    // /// If n was not a VectorNode, this would raise an exception.
    // VectorNode data3D(n);

    // /// Print number of children of data3D.  This is the number of scans in file.
    // int64_t scanCount = data3D.childCount();
    // cout << "Number of scans in file:" << scanCount << endl;

    // /// For each scan, print out first 4 points in either Cartesian or Spherical coordinates.
    // for (int scanIndex = 0; scanIndex < scanCount; scanIndex++)
    // {
    //   /// Get scan from "/data3D", assume its a Structure (else get exception)
    //   StructureNode scan(data3D.get(scanIndex));
    //   cout << "got:" << scan.pathName() << endl;

    //   /// Get "points" field in scan.  Should be a CompressedVectorNode.
    //   CompressedVectorNode points(scan.get("points"));
    //   cout << "got:" << points.pathName() << endl;

    //   /// Call subroutine in this file to print the points
    //   //printSomePoints(imf, points);
    // }

    // imf.close();
    
}