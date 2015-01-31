#include <stdio.h>
#include <sys/time.h> //For timeout
#include <ctime>      //For timeout
#include "ros/ros.h"
#include "std_msgs/Float32.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "uwesub_svs/serial_port.hpp"
#include "uwesub_svs/calibrate_covariance.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
//#include <boost/lexical_cast.hpp> // for converting strings to ...

#define STD_BAUDRATE 19200

#define TIMEOUT_PACKET 500
#define TIMEOUT_REPLY 500
#define TIMEOUT_EMERCENCY 1000

namespace uwe_sub {

	namespace svs {

		class svsInterface : protected uwe_sub::io::SerialPort {
			protected:
				void sendData(std::string const& data) {
					std::vector<unsigned char> vector_data(data.begin(), data.end());	// convert to string
					std::stringstream ss;		// print the data
					ss << "Sending - " << data;
					ROS_INFO(ss.str().c_str());
					writePort(vector_data);		// send the data
				}

				bool waitForData(std::string const& data, uint64_t timeout_ms) {
					uint64_t start_time = getTimeMs64();
					
					std::stringstream ss;		// print the data
					ss << "Waiting for - \"" << data << "\"";
					ROS_INFO(ss.str().c_str());

					while(1) {
						readPort();

						if(getTimeMs64() >= (start_time + timeout_ms)) return false;
						
						// make sure there's something in the buffer before trying to read it
						if(buffer.size() > 0) {
							//std::string buffer_str(buffer.begin(), buffer.end());
						
							std::stringstream buf_ss;
							for(size_t i = 0; i < buffer.size(); ++i)
							{
							  buf_ss << buffer[i];
							}
							std::string buf_s = buf_ss.str();

							std::size_t found = buf_s.find(data);

  							if (found!=std::string::npos) {
    							std::stringstream ss;		// print the reply
								ss << "SVP replied with - " << data;
								ROS_INFO(ss.str().c_str());
								buffer.erase(buffer.begin(), buffer.end()); //Delete some
								return true;
							} 
						}
					}				
				}

				uint64_t getTimeMs64() {
					 struct timeval tv;

					 gettimeofday(&tv, NULL);

					 uint64_t ret = tv.tv_usec;
					 /* Convert from micro seconds (10^-6) to milliseconds (10^-3) */
					 ret /= 1000;

					 /* Adds the seconds (10^0) after converting them to milliseconds (10^-3) */
					 ret += (tv.tv_sec * 1000);

					 return ret;
				}

			public:
				int initialize(std::string svsDeviceID, unsigned int baudrate) {
					
					if(openPort(svsDeviceID, STD_BAUDRATE, true)) {  // start serial
						
						// write config over serial

						sendData("M32\x0A");	// ensure the device isn't  already paused, by bringing it up
						
						// wait for '>'					
						if(waitForData("\n", TIMEOUT_REPLY) == true) {
							ROS_INFO("SVP ONLINE");
						} else { 
							ROS_ERROR("Expected SVP to reply with a packet, not recieved");
						}

						sendData("#");		// send a stop command 
						
						// wait for '>'						
						if(waitForData(">", TIMEOUT_REPLY) == true) {
							ROS_INFO("SVP ONLINE");
						} else { 
							ROS_ERROR("Expected SVP to reply with '>', not recieved");
						}


						// change the baudrate on the device to whatever is specified in the launchfile
						std::stringstream ss;		// print the data
						ss << "#059;"<<baudrate<<"\r";
						sendData(ss.str());		// send a change baudrate command
						ROS_INFO(ss.str().c_str() );
						if(set_baudrate(baudrate, true)) {
							ROS_INFO("changed the baudrate to %d", baudrate);
						}

						
						sendData("M32\x0A");	// ensure the device isn't  already paused, by bringing it up
						
						// wait for '>'						
						if(waitForData("\n", TIMEOUT_REPLY) == true) {
							ROS_INFO("SVP ONLINE");
						} else { 
							ROS_ERROR("Expected SVP to reply with a packet, not recieved");
						}

						sendData("#");		// send a stop command 
						
						// wait for '>'						
						if(waitForData(">", TIMEOUT_REPLY) == true) {
							ROS_INFO("SVP ONLINE");
						} else { 
							ROS_ERROR("Expected SVP to reply with '>', not recieved");
						}

						sendData("#082;off\x0A"); // SETS DATA FORMAT TO STANDARD VALEPORT MODE

						// wait for "OFF/n"
						if(waitForData("OFF", TIMEOUT_REPLY) == true) {
							ROS_INFO("DATA FORMAT TO STANDARD VALEPORT MODE");
						} else { 
							ROS_ERROR("Expected SVP to reply with \"OFF/n\", not recieved");
							return -1;
						}

						sendData("#018;1\x0A"); // SETS UNITS TO METRES - PRESSURE FORMAT

						// wait for "Metres/n"
						if(waitForData("Metres", TIMEOUT_REPLY) == true) {
							ROS_INFO("PRESSURE UNITS SET TO METERS");
						} else { 
							ROS_ERROR("Expected SVP to reply with \"Meters/n\", not recieved");
							return -1;
						}

						sendData("#083;3\x0A"); // SETS UNITS TO 3.D.P - PRESSURE FORMAT

						// wait for "3/n"
						if(waitForData("3", TIMEOUT_REPLY) == true) {
							ROS_INFO("PRESSURE UNITS TO 3.D.P");
						} else { 
							ROS_ERROR("Expected SVP to reply with \"3/n\", not recieved");
							return -1;
						}

						sendData("M32\x0A");	// configure for 32Hz 
						// dont wait for anything ... svp now starts 'spitting out data' (e.g. 09.951 0000000/n)
						
						return 0;
						
					} else {
						ROS_ERROR("Cannot connect to svp!!!");
						closePort();
						return -1;
					}
				}

				
				std::string waitForPacket(uint64_t timeout_ms) {
					uint64_t start_time = getTimeMs64();		
				
					ROS_INFO("Waiting for svp packet");

					while(1) {
						readPort();

						if(getTimeMs64() >= (start_time + timeout_ms)) return "-1";
					
						// make sure there's something in the buffer before trying to read it
						if(buffer.size() > 0) {
							std::string buffer_str(buffer.begin(), buffer.end());
						
							std::size_t found = buffer_str.find("\n");

							if (found!=std::string::npos) {
								buffer.erase(buffer.begin(), buffer.end()); //Delete some
								return buffer_str;
							} else {
								
							}
						} else {
							ROS_ERROR("buffer empty");
						}
					}				
				}
		}; // class svsInterface

	} // namespace svs
	
} // namespace uwe_sub

uwe_sub::svs::svsInterface svs;


int main(int argc, char **argv) 
{ // main (ros stuff)

	bool no_data = true;
	std::string svs_packet;

	// get the device data from the arguments send in the launchfile
	std::string linux_device_path = argv[1];

    unsigned baud = 0;
    sscanf(argv[2], "%u", &baud);


	ros::init(argc, argv, "svs");	

	ros::NodeHandle n;		
	// setup publisher for depth
	// setup publisher for sos
	ros::Publisher svs_depth_msg = n.advertise<geometry_msgs::PoseWithCovarianceStamped>("/svs/depth", 100); 
	ros::Publisher svs_sos_msg = n.advertise<std_msgs::Float32>("/svs/speed_of_sound", 100);

	std_msgs::Float32 svs_sos;
	unsigned int sequence_counter = 0;
	bool variance_found = false;
	calibration::calib_covariance calibration_tool(20);

	ros::Rate r(1);

	while(ros::ok())
		{
			if(no_data == true) {
				while(svs.initialize(linux_device_path, baud) != 0) {
					ROS_ERROR("SVS not ready yet.");
					r.sleep();
				}
				no_data = false;
			}
		
			
			// wait for packet... set no_data if timed out
			
			svs_packet = svs.waitForPacket(TIMEOUT_PACKET);

			if(svs_packet != "-1") { 	
				std::stringstream ss;		// print the data
				ss << "SVS packet found - \"" << svs_packet << "\"";
				ROS_INFO(ss.str().c_str());	

				//expecting: "<space>{pressure}<space>123456<cr><lf>" - e.g. " 09.981 0000000"
				
				if(svs_packet.size() == 16 || svs_packet.size() == 17) {
					std::string depth_str = svs_packet.substr(1,6);
					std::string speed_of_sound_str = svs_packet.substr(8,7);

					//convert to numbers
					long speed_of_sound = std::atol(speed_of_sound_str.c_str());
					float depth = std::atof(depth_str.c_str());

					//print values
					std::stringstream sos_ss;	// print the s_o_s
					sos_ss << "speed_of_sound = " << speed_of_sound;
					ROS_INFO(sos_ss.str().c_str());
					
					std::stringstream d_ss;		// print the depth
					d_ss << "depth = " << depth;
					ROS_INFO(d_ss.str().c_str());

					// publish values
					svs_sos.data = speed_of_sound / 1000.0;	// svs outputs in mm/s we want m/s

				
					// Calibrate
					if(calibration_tool.are_you_calibrated() == true) {
							// Fill in the geometry_msgs::PoseWithCovarianceStamped message
						geometry_msgs::PoseWithCovarianceStamped svs_depth;
						svs_depth.header.frame_id = "SVS";
						svs_depth.header.stamp = ros::Time::now();
						svs_depth.header.seq = sequence_counter;
						calibration_tool.get_variance(svs_depth.pose.covariance[14]);
						svs_depth.pose.pose.position.z = depth ;	// SVS needs to be tared now and again
						svs_depth_msg.publish(svs_depth);
					// we haven't got any variance data.
					} else {
						calibration_tool.put_in(depth);
						ROS_INFO("SVS: Determining depth variance");
					}

					svs_sos_msg.publish(svs_sos);
				
				} else {
					std::stringstream ss;		// print the reply
					ss << "SVS PACKET INVALID (EXPECTED LEN = 16 - IS " << svs_packet.size() << "- \"" << svs_packet << "\"";
					ROS_WARN(ss.str().c_str());
				}
				
			} else {
				ROS_ERROR("SVS packet timed out!");
				no_data = true;
			}
		}
		
	return 0;
}

