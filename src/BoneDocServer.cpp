#include "BoneDocServer.h"

static string BONEDOC_PATH;

BoneDocServer::BoneDocServer(const char* path)
{
    // extract application path 
    boost::filesystem::path full_path(boost::filesystem::initial_path<boost::filesystem::path>());
    full_path = boost::filesystem::system_complete(boost::filesystem::path(path));
    size_t found = full_path.string().find_last_of("/\\");
    BONEDOC_PATH = full_path.string().substr(0, found);
}

void session(boost::asio::ip::tcp::socket socket)
{
    try
    {
        for (;;)
        {
            // read the request header (until blank line)
            boost::asio::streambuf request;
            boost::asio::read_until(socket, request, "\r\n");

            // put request to input stream
            istream request_stream(&request);

			// vars
			string request_header = "";

			// mesh to be loaded, may be the model's mean or a random mesh created from the pca model
			string mesh = "";
			string dataset = "";
			string anatomy = "";
			string side = "";
			string gender = "";
			string ethnic_group = "";
			string study = "";
			string anatomical_landmarks = "";

            // print the request (header), each line is separated by carriage return symbol '\r'.
            while (getline(request_stream, request_header) && request_header != "\r")
            {
                if (request_header.compare(0, 7, "Anatomy") == 0)
                    anatomy = request_header.substr(9, request_header.length() - 10);
                else if (request_header.compare(0, 11, "EthnicGroup") == 0)
                    ethnic_group = request_header.substr(13, request_header.length() - 14);
                else if (request_header.compare(0, 6, "Gender") == 0)
                    gender = request_header.substr(8, request_header.length() - 9);
                else if (request_header.compare(0, 4, "Mean") == 0)
                    mesh = request_header.substr(6, request_header.length() - 7);
                else if (request_header.compare(0, 6, "Random") == 0)
                    mesh = request_header.substr(8, request_header.length() - 9);
                else if (request_header.compare(0, 4, "Side") == 0)
                    side = request_header.substr(6, request_header.length() - 7);
                else if (request_header.compare(0, 5, "Study") == 0)
                    study = request_header.substr(7, request_header.length() - 8);
				else if (request_header.compare(0, 7, "Dataset") == 0) {
					dataset = request_header.substr(9, request_header.length() - 10);
				}
				else
				{
					// unknown request
				}
            }

            // put server's response to output stream
            boost::asio::streambuf response;
            ostream response_stream(&response);

            response_stream << "HTTP/1.1 200 OK\r\n";
            response_stream << "Content-Type: text/plain\r\n";

            response_stream << "Access-Control-Allow-Origin: *\r\n";
            response_stream << "Connection: keep alive\r\n";
            response_stream << "Access-Control-Allow-Headers: Dataset,Anatomy,EthnicGroup,Gender,Mean,Random,Side,Study\r\n\r\n";

            // client message is only considered for analysis if header consists of meta infos
            if ((anatomy != "") && (study == "Thesis"))
            {
                // concatenate correct path's (DEBUG: support for stl/vtk)
                stringstream anatomicalMeshPath;
                anatomicalMeshPath << BONEDOC_PATH << "\\Data\\" << dataset << ".vtk";

                stringstream anatomicallandmarksPath;
                anatomicallandmarksPath << BONEDOC_PATH << "\\Data\\" << dataset << "-landmarks.csv";

                stringstream configPath;
                configPath << BONEDOC_PATH << "\\bonehost.conf";

                // DEBUG: recognize anatomy from dataset's name
				if (anatomy.compare("Femur") == 0)
                {
                    Femur femur(anatomicalMeshPath.str(), anatomicallandmarksPath.str(), configPath.str());
					femur.Thesis();

                    response_stream << "bone length: " << float(int(femur.bone_length * 100)) / 100 << "mm" << endl;
                    response_stream << "medial offset: " << float(int(femur.medial_offset * 100)) / 100 << "mm" << endl;
                    response_stream << "lateral offset: " << float(int(femur.lateral_offset * 100)) / 100 << "mm" << endl;
                    response_stream << "ml width: " << float(int(femur.ML_width * 100)) / 100 << "mm" << endl;
                    response_stream << "ap width: " << float(int(femur.AP_width * 100)) / 100 << "mm" << endl;
                    response_stream << "head radius: " << float(int(femur.head_radius * 100)) / 100 << "mm" << endl;
                    response_stream << "inclination: " << float(int(femur.inclination * 100)) / 100 << "°" << endl;
                    response_stream << "anteversion: " << float(int(femur.anteversion * 100)) / 100 << "°" << endl;
                    response_stream << "asian: " << femur.asian << "%" << endl;
                    response_stream << "caucasian: " << femur.caucasian << "%" << endl;
                }
                else if (anatomy.compare("Humerus") == 0)
                {
                    Humerus humerus(anatomicalMeshPath.str(), anatomicallandmarksPath.str(), configPath.str());
                    humerus.Thesis();

                    response_stream << "bone length: " << float(int(humerus.bone_length * 100)) / 100 << "mm" << endl;
                    response_stream << "medial offset: " << float(int(humerus.medial_offset * 100)) / 100 << "mm" << endl;
                    response_stream << "lateral offset: " << float(int(humerus.lateral_offset * 100)) / 100 << "mm" << endl;
                    response_stream << "ml width: " << float(int(humerus.ML_width * 100)) / 100 << "mm" << endl;
                    response_stream << "ap width: " << float(int(humerus.AP_width * 100)) / 100 << "mm" << endl;
                    response_stream << "head radius: " << float(int(humerus.head_radius * 100)) / 100 << "mm" << endl;
                    response_stream << "inclination: " << float(int(humerus.inclination * 100)) / 100 << "°" << endl;
                    response_stream << "retroversion: " << float(int(humerus.retroversion * 100)) / 100 << "°" << endl;
                    response_stream << "asian: " << humerus.asian << "%" << endl;
                    response_stream << "caucasian: " << humerus.caucasian << "%" << endl;
				}
				else if (anatomy.compare("Tibia") == 0)
                {
                    Tibia tibia(anatomicalMeshPath.str(), anatomicallandmarksPath.str(), configPath.str());
                    tibia.Thesis();

                    response_stream << "bone length: " << float(int(tibia.bone_length * 100)) / 100 << "mm" << endl;
                    response_stream << "medial offset: " << float(int(tibia.medial_offset * 100)) / 100 << "mm" << endl;
                    response_stream << "lateral offset: " << float(int(tibia.lateral_offset * 100)) / 100 << "mm" << endl;
                    response_stream << "ml width: " << float(int(tibia.ML_width * 100)) / 100 << "mm" << endl;
                    response_stream << "ap width: " << float(int(tibia.AP_width * 100)) / 100 << "mm" << endl;
                    response_stream << "torsion: " << float(int(tibia.torsion * 100)) / 100 << "°" << endl;
                    response_stream << "asian: " << tibia.asian << "%" << endl;
                    response_stream << "caucasian: " << tibia.caucasian << "%" << endl;
                }

            }

            // write to socket
            boost::asio::write(socket, response);

            // break finally sends message
            break;
        }

    }
    catch (exception& e)
    {
        cerr << "Exception in thread: " << e.what() << "\n";
    }

}

void BoneDocServer::Start()
{
    cout << "BoneDocServer started. Listening on port " << BONEDOC_SERVER_PORT_STR << " ..." << endl << endl;

    try
    {
        boost::asio::io_service io_service;
        boost::asio::ip::tcp::acceptor a(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), BONEDOC_SERVER_PORT));

        for (;;)
        {
            boost::asio::ip::tcp::socket socket(io_service);
            a.accept(socket);
            thread(session, move(socket)).detach();
        }

    }
    catch (exception& e)
    {
        cerr << "Exception: " << e.what() << "\n";
    }

    cout << "Server closed." << endl;
}

BoneDocServer::~BoneDocServer() {
    
}
