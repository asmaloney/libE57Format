// $Id: e57unpack.cpp 332 2013-04-03 14:24:08Z roland_schwarz $
#include <config.h>

#define PROGRAM_NAME "e57unpack"
#define PROGRAM_VERSION "1.0"
#ifndef SVN_VERSION
#define SVN_VERSION unknown
#endif
#define STRING_HELPER2(a) #a
#define STRING_HELPER1(a) STRING_HELPER2(a)
#define BUILD_VERSION STRING_HELPER1(SVN_VERSION)

#include <E57Foundation.h>
using e57::Node;
using e57::ImageFile;
using e57::StructureNode;
using e57::VectorNode;
using e57::CompressedVectorNode;
using e57::StringNode;
using e57::IntegerNode;
using e57::ScaledIntegerNode;
using e57::FloatNode;
using e57::StringNode;
using e57::BlobNode;
using e57::E57Exception;
using e57::E57Utilities;
using e57::ustring;
using e57::SourceDestBuffer;
using e57::CompressedVectorReader;
using e57::int64_t;
using e57::uint64_t;
using e57::uint8_t;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
using std::ostream;
using std::ios_base;
using std::streamsize;

#include <exception>
using std::exception;

#include <stdexcept>
using std::runtime_error;

#if defined(_MSC_VER)
#   include <memory>
#else
#   include <tr1/memory>
#endif
using std::tr1::shared_ptr;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <limits>
using std::numeric_limits;

#include <boost/config.hpp>

#include <boost/program_options.hpp>
using boost::program_options::variables_map;
using boost::program_options::options_description;
using boost::program_options::positional_options_description;
using boost::program_options::store;
using boost::program_options::command_line_parser;
using boost::program_options::value;

#include <boost/filesystem.hpp>
using boost::filesystem::path;
using boost::filesystem::create_directories;
using boost::filesystem::portable_directory_name;

#include <boost/filesystem/fstream.hpp>
using boost::filesystem::ofstream;

#include <boost/variant.hpp>
using boost::variant;
using boost::get;
using boost::static_visitor;
using boost::apply_visitor;

#include <boost/format.hpp>
using boost::format;
using boost::io::too_many_args_bit;
using boost::io::all_error_bits;

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

variables_map opt;

void
print_version(
) {
    cout
    << PROGRAM_NAME" (" << BOOST_PLATFORM << ") "
    << PROGRAM_VERSION
    << "." << BUILD_VERSION
    << endl
    ;

    int astmMajor;
    int astmMinor;
    ustring libraryId;
    E57Utilities().getVersions(astmMajor, astmMinor, libraryId);

    cout << "E57 API " << astmMajor << "." << astmMinor << endl;
    cout << libraryId << endl;
}

void
print_help(
    const options_description& options
) {
    cout
    << "Usage:\n"
    << PROGRAM_NAME" [options] e57-file [unpack-directory]\n"
    << "  The purpose of this program is to break the e57 file into\n"
    << "  parts.\n"
    << endl
    << options
    ;
}

class get_at
    : public static_visitor<variant<double, int64_t> >
{
    size_t at;
public:
    get_at(size_t at_) : at(at_) {}
    template <typename T>
    variant<double, int64_t> operator()( T & operand ) const
    {
        return operand[at];
    }

};

int
main(
    int argc
    , char *argv[]
) {
    try {
        options_description options("options");
        options.add_options()
            (
                "version,v"
                , "show version"
            )
            (
                "help,h"
                , "show help"
            )
            (
                "format,F"
                , value<std::string>()
                ,"a boost::format format string for\n"
                 "the pointrecord data.\n"
            )
            (
                "src"
                , value<string>()
                , "define e57 file\n"
                  "  (--src may be omitted)\n"
            )
            (
                "dst"
                , value<string>()
                , "the destination directory\n"
                  "  (--dst may be omitted)"
            )
            (
                "no-images"
                , "supress image output"
            )
            (
                "no-points"
                , "supress pointcloud output"
            )
            ;

        positional_options_description  positional;
        positional.add("src",1);
        positional.add("dst",2);

        store(
            command_line_parser(argc, argv)
                . options(options)
                . positional(positional)
                . run()
            , opt
        );

        if (opt.count("version")) {
            print_version();
            return 0;
        }

        if (opt.count("help")) {
            print_help(options);
            return 0;
        }

        if (opt.count("src")) {
            string fmt;
            if (opt.count("format"))
                fmt = opt["format"].as<string>();

            ImageFile imf(opt["src"].as<string>(), "r");
            StructureNode root = imf.root();

            path dst;
            if (opt.count("dst"))
                dst = path(opt["dst"].as<string>());
            else {
                dst = path(opt["src"].as<string>());
                dst.replace_extension();
            }

            create_directories(dst);

            ofstream root_inf(dst/"root.inf");
            root_inf << "formatName = " << StringNode(root.get("formatName")).value() << endl;
            root_inf << "guid = " << StringNode(root.get("guid")).value() << endl;
            root_inf << "versionMajor = " << IntegerNode(root.get("versionMajor")).value() << endl;
            root_inf << "versionMinor = " << IntegerNode(root.get("versionMinor")).value() << endl;
            if (root.isDefined("e57LibraryVersion")) {
                root_inf << "e57LibraryVersion = " << StringNode(root.get("e57LibraryVersion")).value() << endl;
            }
            if (root.isDefined("coordinateMetadata")) {
                root_inf << "coordinateMetadata = " << StringNode(root.get("coordinateMetadata")).value() << endl;
            }
            if (root.isDefined("creationDateTime")) {
                StructureNode t(root.get("creationDateTime"));
                root_inf << format("creationDateTime.dateTimeValue = %.15g\n") % FloatNode(t.get("dateTimeValue")).value();
                if (t.isDefined("isAtomicClockReferenced")) {
                    root_inf << "creationDateTime.isAtomicClockReferenced = " << IntegerNode(t.get("isAtomicClockReferenced")).value() << endl;
                }
            }
            root_inf.close();

            if (root.isDefined("data3D") && !opt.count("no-points")) {
                VectorNode data3D(root.get("data3D"));
                for (int64_t child=0; child<data3D.childCount(); ++child) {
                    StructureNode            scan(data3D.get(child));
                    CompressedVectorNode     points(scan.get("points"));
                    StructureNode            prototype(points.prototype());
                    vector<SourceDestBuffer> sdb;
                    const size_t buf_size = 1024;
                    vector<variant<vector<double>, vector<int64_t> > > buf;
                    string pointrecord;

                    string comma;
                    if (!opt.count("format")) {
                        fmt.clear();
                        for (int64_t i=0; i<prototype.childCount(); ++i) {
                            switch(prototype.get(i).type()) {
                                case e57::E57_FLOAT:
                                case e57::E57_SCALED_INTEGER:
                                    buf.push_back(vector<double>(buf_size));
                                    if (!opt.count("format"))
                                        fmt += comma +"%."+lexical_cast<string>(numeric_limits<double>::digits10)+"g";
                                    break;
                                case e57::E57_INTEGER:
                                    buf.push_back(vector<int64_t>(buf_size));
                                    if (!opt.count("format"))
                                        fmt += comma +"%d";
                                    break;
                            }
                            if (comma.empty()) comma = ",";
                        }
                    }
                    comma.clear();
                    for (int i=0; i<prototype.childCount(); ++i) {
                        Node n(prototype.get(i));
                        pointrecord += comma + n.elementName();
                        if (comma.empty()) comma = ",";
                        switch(n.type()) {
                            case e57::E57_FLOAT:
                            case e57::E57_SCALED_INTEGER:
                                sdb.push_back(
                                    SourceDestBuffer(
                                        imf
                                        , n.elementName()
                                        , &get<vector<double> >(buf[i])[0]
                                        , buf_size
                                        , true
                                        , true
                                    )
                                );
                                break;
                            case e57::E57_INTEGER:
                                sdb.push_back(
                                    SourceDestBuffer(
                                        imf
                                        , n.elementName()
                                        , &get<vector<int64_t> >(buf[i])[0]
                                        , buf_size
                                        , true
                                        , true
                                    )
                                );
                            break;
                            default:
                                throw(runtime_error(
                                    "prototype contains illegal type")
                            );
                        }
                    }

                    ofstream inf(
                        dst/path(string("image3d-")+lexical_cast<string>(child)+".inf")
                    );
                    inf << "pointrecord = " << pointrecord << endl; // can be used as a header line for the csv file
                    inf << "pointrecord.format = " << fmt << endl;
                    if (scan.isDefined("name")) {
                        inf << "name = " << StringNode(scan.get("name")).value() << endl;
                    }
                    inf << "guid = " << StringNode(scan.get("guid")).value() << endl;
                    if (scan.isDefined("description")) {
                        inf << "description = " << StringNode(scan.get("description")).value() << endl;
                    }
                    if (scan.isDefined("pose")) {
                        StructureNode pose(scan.get("pose"));
                        StructureNode translation(pose.get("translation"));
                        StructureNode rotation(pose.get("rotation"));
                        inf << "pose.translation.x = " << FloatNode(translation.get("x")).value() << endl;
                        inf << "pose.translation.y = " << FloatNode(translation.get("y")).value() << endl;
                        inf << "pose.translation.z = " << FloatNode(translation.get("z")).value() << endl;
                        inf << "pose.rotation.w = " << FloatNode(rotation.get("w")).value() << endl;
                        inf << "pose.rotation.x = " << FloatNode(rotation.get("x")).value() << endl;
                        inf << "pose.rotation.y = " << FloatNode(rotation.get("y")).value() << endl;
                        inf << "pose.rotation.z = " << FloatNode(rotation.get("z")).value() << endl;
                    }
                    if (scan.isDefined("acquisitionStart")) {
                        StructureNode t(scan.get("acquisitionStart"));
                        inf << format("acquisitionStart.dateTimeValue = %.15g\n") % FloatNode(t.get("dateTimeValue")).value();
                        if (t.isDefined("isAtomicClockReferenced")) {
                            inf << "acquisitionStart.isAtomicClockReferenced = " << IntegerNode(t.get("isAtomicClockReferenced")).value() << endl;
                        }
                    }
                    if (scan.isDefined("acquisitionEnd")) {
                        StructureNode t(scan.get("acquisitionEnd"));
                        inf << format("acquisitionEnd.dateTimeValue = %.15g\n") % FloatNode(t.get("dateTimeValue")).value();
                        if (t.isDefined("isAtomicClockReferenced")) {
                            inf << "acquisitionEnd.isAtomicClockReferenced = " << IntegerNode(t.get("isAtomicClockReferenced")).value() << endl;
                        }
                    }
                    if (scan.isDefined("sensorVendor")) {
                        inf << "sensorVendor = " << StringNode(scan.get("sensorVendor")).value() << endl;
                    }
                    if (scan.isDefined("sensorModel")) {
                        inf << "sensorModel = " << StringNode(scan.get("sensorModel")).value() << endl;
                    }
                    if (scan.isDefined("sensorSerialNumber")) {
                        inf << "sensorSerialNumber = " << StringNode(scan.get("sensorSerialNumber")).value() << endl;
                    }
                    if (scan.isDefined("HardwareVersion")) {
                        inf << "HardwareVersion = " << StringNode(scan.get("HardwareVersion")).value() << endl;
                    }
                    if (scan.isDefined("SoftwareVersion")) {
                        inf << "SoftwareVersion = " << StringNode(scan.get("SoftwareVersion")).value() << endl;
                    }
                    if (scan.isDefined("FirmwareVersion")) {
                        inf << "FirmwareVersion = " << StringNode(scan.get("FirmwareVersion")).value() << endl;
                    }
                    if (scan.isDefined("temperature")) {
                        inf << "temperature = " << FloatNode(scan.get("temperature")).value() << endl;
                    }
                    if (scan.isDefined("relativeHumidity")) {
                        inf << "relativeHumidity = " << FloatNode(scan.get("relativeHumidity")).value() << endl;
                    }
                    if (scan.isDefined("atmosphericPressure")) {
                        inf << "atmosphericPressure = " << FloatNode(scan.get("atmosphericPressure")).value() << endl;
                    }
                    inf.close();

                    CompressedVectorReader rd(points.reader(sdb));
                    path csvname(string("image3d-")+lexical_cast<string>(child)+".csv");
                    ofstream ocsv(dst/csvname);
                    ostream& out(ocsv); // needed to fix ambiguity for << operator on msvc
                    cout << "unpacking: " << dst/csvname << " ... ";
                    unsigned count;
                    uint64_t total_count(0);

                    format tfmt(fmt);
                    tfmt.exceptions( all_error_bits ^ too_many_args_bit );
                    while(count = rd.read()) {
                        total_count += count;
                        for (size_t i=0; i<count; ++i) {
                            for (size_t j=0; j<buf.size(); ++j)
                                tfmt = tfmt % apply_visitor(get_at(i),buf.at(j));
                            out << tfmt << endl;
                        }
                    }
                    cout << " total points: " << total_count << endl;

                    ocsv.close();
                }
            }

            if (root.isDefined("images2D") && !opt.count("no-images")) {
                VectorNode images2D(root.get("images2D"));
                for (int64_t child=0; child<images2D.childCount(); ++child) {
                    StructureNode  image(images2D.get(child));
                    path inf_path = dst/path(string("image2d-")+lexical_cast<string>(child)+".inf");
                    ofstream inf(inf_path);
                    string reptype;
                    if (image.isDefined("visualReferenceRepresentation"))
                        reptype = "visualReferenceRepresentation";
                    else if (image.isDefined("pinholeRepresentation"))
                        reptype = "pinholeRepresentation";
                    else if (image.isDefined("sphericalRepresentation"))
                        reptype = "sphericalRepresentation";
                    else if (image.isDefined("cylindricalRepresentation"))
                        reptype = "cylindricalRepresentation";
                    StructureNode rep(image.get(reptype));
                    string imgtype;
                    path img_path(dst/path(string("image2d-")+lexical_cast<string>(child)+".img"));
                    if (rep.isDefined("jpegImage")) {
                        img_path.replace_extension(".jpg");
                        imgtype = "jpegImage";
                    }
                    else if (rep.isDefined("pngImage")) {
                        img_path.replace_extension(".png");
                        imgtype = "pngImage";
                    }
                    // extract image blob
                    BlobNode blob(rep.get(imgtype));
                    ofstream img(img_path, ios_base::out|ios_base::binary);
                    const streamsize buffer_size = 1024*1024;
                    vector<uint8_t> buffer(buffer_size);
                    int64_t offset = 0;
                    int64_t remaining = blob.byteCount();
                    size_t get;
                    cout << "unpacking: " << img_path << " ... ";
                    while(img && remaining > 0) {
                        get = (remaining > buffer_size)?buffer_size:remaining;
                        blob.read(&buffer[0], offset, get);
                        img.write(reinterpret_cast<char*>(&buffer[0]), get);
                        offset += get;
                        remaining -= get;
                    }
                    img.close();
                    // extract meta data
                    inf << "guid = " << StringNode(image.get("guid")).value() << endl;
                    if (image.isDefined("name")) {
                        inf << "name = " << StringNode(image.get("name")).value() << endl;
                    }
                    if (image.isDefined("description")) {
                        inf << "description = " << StringNode(image.get("description")).value() << endl;
                    }
                    if (image.isDefined("sensorVendor")) {
                        inf << "sensorVendor = " << StringNode(image.get("sensorVendor")).value() << endl;
                    }
                    if (image.isDefined("sensorModel")) {
                        inf << "sensorModel = " << StringNode(image.get("sensorModel")).value() << endl;
                    }
                    if (image.isDefined("sensorSerialNumber")) {
                        inf << "sensorSerialNumber = " << StringNode(image.get("sensorSerialNumber")).value() << endl;
                    }
                    if (image.isDefined("pose")) {
                        StructureNode pose(image.get("pose"));
                        StructureNode translation(pose.get("translation"));
                        StructureNode rotation(pose.get("rotation"));
                        inf << "pose.translation.x = " << FloatNode(translation.get("x")).value() << endl;
                        inf << "pose.translation.y = " << FloatNode(translation.get("y")).value() << endl;
                        inf << "pose.translation.z = " << FloatNode(translation.get("z")).value() << endl;
                        inf << "pose.rotation.w = " << FloatNode(rotation.get("w")).value() << endl;
                        inf << "pose.rotation.x = " << FloatNode(rotation.get("x")).value() << endl;
                        inf << "pose.rotation.y = " << FloatNode(rotation.get("y")).value() << endl;
                        inf << "pose.rotation.z = " << FloatNode(rotation.get("z")).value() << endl;
                    }
                    if (rep.isDefined("imageHeight")) {
                        inf << reptype+".imageHeight = " << IntegerNode(rep.get("imageHeight")).value() << endl;
                        cout << IntegerNode(rep.get("imageHeight")).value() << " x ";
                    }
                    if (rep.isDefined("imageWidth")) {
                        inf << reptype+".imageWidth = " << IntegerNode(rep.get("imageWidth")).value() << endl;
                        cout << IntegerNode(rep.get("imageWidth")).value() << " pixels" << endl;
                    }
                    if (rep.isDefined("focalLength")) {
                        inf << reptype+".focalLength = " << FloatNode(rep.get("focalLength")).value() << endl;
                    }
                    if (rep.isDefined("pixelWidth")) {
                        inf << reptype+".pixelWidth = " << FloatNode(rep.get("pixelWidth")).value() << endl;
                    }
                    if (rep.isDefined("pixelHeight")) {
                        inf << reptype+".pixelHeight = " << FloatNode(rep.get("pixelHeight")).value() << endl;
                    }
                    if (rep.isDefined("principalPointX")) {
                        inf << reptype+".principalPointX = " << FloatNode(rep.get("principalPointX")).value() << endl;
                    }
                    if (rep.isDefined("principalPointY")) {
                        inf << reptype+".principalPointY = " << FloatNode(rep.get("principalPointY")).value() << endl;
                    }
                    if (rep.isDefined("radius")) {
                        inf << reptype+".radius = " << FloatNode(rep.get("radius")).value() << endl;
                    }

                    inf.close();
                }
            }

            return 0;
        }
        else {
            print_help(options);
            return -1;
        }

    } catch(E57Exception& e) {
        e.report(__FILE__, __LINE__, __FUNCTION__);
        return -1;
    }
    catch(exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
    catch(...) {
        cerr << "unknown exception" << endl;
        return -1;
    }
    return 0;
}
